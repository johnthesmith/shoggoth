/*
    Teacher payload
*/

/* Vanilla libraris */
#include <iostream>
#include <cmath>

/* Shoggoth libraries */
#include "../../shoggoth/io.h"
#include "../../../../../lib/core/str.h"
/* User libraries */
#include "teacher_application.h"
#include "teacher_payload.h"
#include "teacher_consts.h"


using namespace std;



/*
    Constructor
*/
TeacherPayload::TeacherPayload
(
    TeacherApplication* aApplication,
    string aPayloadId,
    string aNetId,
    string aNetVersion
)
: PayloadEngine( aApplication, aPayloadId ) /* Call parent constructor */
{
    net = Net::create
    (
        aApplication,
        aApplication -> getSockManager(),
        aNetId,
        aNetVersion,
        TASK_TEACHER
    );
    limb = LimbTeacher::create( net );
}



/*
    Destructor
*/
TeacherPayload::~TeacherPayload()
{
    limb -> destroy();
    net -> destroy();
}



/*
    Creator
*/
TeacherPayload* TeacherPayload::create
(
    TeacherApplication* a,
    string aPayloadId,
    string aNetId,
    string aNetVersion
)
{
    return new TeacherPayload( a, aPayloadId, aNetId, aNetVersion );
}



/*
    Destructor
*/
void TeacherPayload::destroy()
{
    delete this;
}



/*
    Return application object
*/
TeacherApplication* TeacherPayload::getApplication()
{
    return ( TeacherApplication* ) Payload::getApplication();
}



/******************************************************************************
    Payload events
*/



/*
    Main loop event
*/
void TeacherPayload::onEngineLoop
(
    bool aConfigUpdate,
    bool aEnabled
)
{
    if( aEnabled )
    {
        getMon()
        -> setString( Path{ "net", "id" }, net -> getId() )
        -> setString( Path{ "net", "version" }, net -> getVersion() )
        ;

        /* Check server net config */
        auto netConfig = ParamList::create();

        /* Read net config from server */
        net -> setOk();
        net -> readNet( netConfig );
        if
        (
            net -> isConfigUpdate( netConfig ) ||
            net -> isVersionChanged()
        )
        {
            getLog()
            -> begin( "Net config updated" )
            -> prm( "File", getApplication() -> getConfigFileName() )
            -> dump( netConfig, "Net config" )
            -> lineEnd();
            net -> applyNet( netConfig );
            getLog() -> end();
        }
        netConfig -> destroy();

        net -> resultTo( this );

        if( isOk() )
        {
            /* Synchronize net from the Shoggoth server */
            net
            -> syncWithServer()
            -> resultTo( this );
        }

        getLog() -> trace( "Server tick" ) -> prm( "number", net -> getTick() );

        if( isOk() )
        {
            /* Processing Teacher */
            getLog() -> begin( "Teacher processing" );

            auto errorLimit     = getErrorLimit();
            auto idErrorLayer   = getIdErrorLayer();
            auto mode           = getMode();
            auto batches        = getBatches();

            getMon()
            -> startTimer( Path{ "currentMks" })
            -> addInt( Path{ "count" })
            -> now( Path{ "last", "moment" } )
            -> setString( Path{ "config", "errorLayer" }, idErrorLayer )
            -> setDouble( Path{ "config", "errorLimit" }, errorLimit )
            -> setString( Path{ "config", "mode" }, mode )
            ;

            getLog() -> trace( "Check error level" );

            /* Prepare Limb */
            limb
            -> getNet()
            -> syncToLimb( limb, false )
            -> swapValuesAndErrors
            (
                Actions{ READ_VALUES }, /* Action */
                limb,                   /* Destination participant object */
                false
            );

            limb -> lock();

            /* Retrive error layer by id */
            auto errorLayer = limb -> getLayerById( idErrorLayer );

            if( errorLayer != NULL )
            {
                auto error = errorLayer -> calcRmsValue();

                getLog()
                -> trace( "Compare" )
                -> prm( "error", error )
                -> prm( "error limit", errorLimit )
                ;

                getMon()
                -> setDouble( Path{ "last", "error" }, error )
                -> setDouble( Path{ "last", "errorDelta" }, error - errorLimit );

                /* Check error limit */
                if
                (
                    error <= errorLimit ||
                    lastChange < limb -> getLastChangeStructure()
                )
                {
                    lastChange = limb -> getLastChangeStructure();

                    /* Check new batch */
                    getLog() -> begin( "New batch" );

                    auto list = batches -> getObject( Path{ mode, "list" } );
                    auto all = batches -> getObject( Path{ mode, "all" } );

                    if( list != NULL && all != NULL )
                    {
                        auto item = list -> getRnd();
                        if( item != NULL && item -> isObject() )
                        {
                            auto batch = ParamList::create()
                            -> copyFrom( all )
                            -> copyFrom( item -> getObject() );

                            /* Batch precessing */
                            batch -> loop
                            (
                                [ this ]
                                ( Param* aParam )
                                {
                                    auto obj = aParam -> getObject();
                                    if( obj != NULL )
                                    {
                                        auto command = obj -> getString( "cmd" );
                                        getLog()
                                        -> begin( "Command" )
                                        -> prm( "cmd", command )
                                        -> dump( obj, "Arguments" );

                                        switch( stringToTeacherTask( command ))
                                        {
                                            case TEACHER_CMD_VALUE_TO_LAYER:
                                                cmdValueToLayer( obj );
                                            break;
                                            case TEACHER_CMD_VALUES_TO_LAYER:
                                                cmdValuesToLayer( obj );
                                            break;
                                            case TEACHER_CMD_IMAGE_TO_LAYER:
                                                cmdImageToLayer( obj );
                                            break;
                                            case TEACHER_CMD_FOLDER_TO_LAYER:
                                                cmdFolderToLayer( obj );
                                            break;
                                            case TEACHER_CMD_GUID_TO_LAYER:
                                            break;
                                            case TEACHER_CMD_HID_TO_LAYER:
                                            break;
                                            default:
                                                setResult( "Unknown command" )
                                                -> getDetails()
                                                -> setString( "command", command );
                                            break;
                                        }
                                        getLog() -> end();
                                    }
                                    return false;
                                }
                            );

                            /* Destroy the batch */
                            batch -> destroy();

                            if( isOk() )
                            {
                                /* Upload values and errors to net */
                                limb -> getNet() -> swapValuesAndErrors
                                (
                                    /* Action */
                                    { WRITE_VALUES, WRITE_ERRORS },
                                    /* Participant object */
                                    limb,
                                    false
                                );
                            }
                        }
                        else
                        {
                            setCode( "batch_is_not_a_object" );
                        }
                    }
                    else
                    {
                        setCode( "batch_sections_not_found" );
                    }
                    getLog() -> end();
                }
                else
                {
                    setCode( "hight_error_rate" );
                }
            }
            else
            {
                setCode( "error_layer_not_found" );
            }

            limb -> unlock();
            getLog() -> end();
        }
    }
}



/******************************************************************************
    Getters and setters
*/


/*
    Return current error limit from Net config
*/
double TeacherPayload::getErrorLimit()
{
    limb -> getNet() -> lock();
    auto result = limb
    -> getNet()
    -> getConfig()
    -> getDouble( Path{ "teacher", "errorLimit" }, 1.0 );
    limb -> getNet() -> unlock();

    return result;
}



/*
    Return error layer id from application config
*/
string TeacherPayload::getIdErrorLayer()
{
    return getApplication() -> getConfig() -> getString( "idErrorLayer" );
}



string TeacherPayload::getMode()
{
    return getApplication() -> getConfig() -> getString( "mode" );
}



bool TeacherPayload::getEnabled()
{
    return getApplication() -> getConfig() -> getBool( "enabled", true );
}



ParamList* TeacherPayload::getBatches()
{
    return getApplication() -> getConfig() -> getObject( "batches" );
}



/*
    Fill layer with value noise
*/
TeacherPayload* TeacherPayload::cmdValueToLayer
(
    ParamList* a
)
{
    limb -> lock();

    auto layerId = a -> getString( "layer" );
    auto layer = limb -> getLayerById( layerId );
    if( layer != NULL )
    {
        layer -> noiseValue
        (
            a -> getInt( "seed", 0 ),
            a -> getDouble( "min", 0.0 ),
            a -> getDouble( "max", 1.0 )
        );
    }
    else
    {
        setResult( "layer_not_found" )
        -> getDetails()
        -> setString( "id", layerId );
    }

    limb -> unlock();

    return this;
}




/*
    Fill layer with values from array
*/
TeacherPayload* TeacherPayload::cmdValuesToLayer
(
    ParamList* a
)
{
    limb -> lock();

    auto layerId = a -> getString( "layer" );
    auto layer = limb -> getLayerById( layerId );

    if( layer != NULL )
    {
        layer -> fillValue( a -> getObject( "values", 0 ));
    }
    else
    {
        setResult( "layer_not_found" )
        -> getDetails()
        -> setString( "id", layerId );
    }

    limb -> unlock();

    return this;
}



/*
    Fill layer from image
*/
TeacherPayload* TeacherPayload::cmdImageToLayer
(
    ParamList* a
)
{
    auto layerId = a -> getString( "layer" );

    limb -> lock();
    auto layer = limb -> getLayerById( layerId );
    if( layer != NULL )
    {
        auto files = a -> getObject( "files" );
        if( files != NULL )
        {
            auto file = files -> getRnd() -> getString();
            if( file != "" )
            {
                layer -> imageToValue
                (
                    file,
                    a -> getDouble( "rotate" ),
                    a -> getDouble( "zoomMin" ),
                    a -> getDouble( "zoomMax" ),
                    a -> getDouble( "shift" ),
                    this
                );
            }
            else
            {
                setResult( "file_name_is_empty" );
            }
        }
        else
        {
            setResult( "files_section_not_found" );
        }
    }
    else
    {
        setResult( "layer_not_found" )
        -> getDetails()
        -> setString( "id", layerId );
    }
    limb -> unlock();
    return this;
}




/*
    Fill layer from folder image
*/
TeacherPayload* TeacherPayload::cmdFolderToLayer
(
    ParamList* a
)
{
    auto layerId = a -> getString( "layer" );

    limb -> lock();
    auto layer = limb -> getLayerById( layerId );
    if( layer != NULL )
    {
        auto folder = a -> getString( "folder", "" );
        if( folder != "" )
        {
            auto files = ParamList::create() -> filesFromPath( folder );
            auto file = files -> getRnd();
            if( file != NULL )
            {
                layer -> imageToValue
                (
                    folder +  "/" + file -> getString(),
                    a -> getDouble( "rotate" ),
                    a -> getDouble( "zoomMin" ),
                    a -> getDouble( "zoomMax" ),
                    a -> getDouble( "shift" ),
                    this
                );
            }
            else
            {
                setResult( "file_not_found" );
            }
            files -> destroy();
        }
        else
        {
            setResult( "folder_is_empty" );
        }
    }
    else
    {
        setResult( "layer_not_found" )
        -> getDetails()
        -> setString( "id", layerId );
    }
    limb -> unlock();
    return this;
}

