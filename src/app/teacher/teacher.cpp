/*
    Используется.
    TODO Переделать в teacher_payload
*/
#include <iostream>

/* User libraries */
#include "teacher.h"
#include "../../../../../lib/core/math.h"
#include "../../../../../lib/core/rnd.h"
#include "../../../../../lib/core/utils.h"
#include "teacher_consts.h"

using namespace std;



/*
    Constructor
*/
Teacher::Teacher
(
    Net* aNet
)
/* Call parent constructor */
: Payload( aNet -> getApplication() )
{
    getLog() -> trace( "Create teacher" );

    mon = Mon::create( aNet -> getMonPath( "teacher.json" ))
    -> now( Path{ "start" })
    -> startTimer( Path{ "startMks" });

    limb = LimbTeacher::create( aNet );
    batches = ParamList::create();
}



/*
    Destructor
*/
Teacher::~Teacher()
{
    limb -> destroy();
    batches -> destroy();
    mon -> destroy();
    getLog() -> trace( "Teacher destroyd" );
}



/*
    Create the teacher object
*/
Teacher* Teacher::create
(
    Net* a
)
{
    return new Teacher( a );
}



/*
    Destroy the teache
*/
void Teacher::destroy()
{
    delete this;
}



/*
    Return application
*/
ShoggothRoleApplication* Teacher::getApplication()
{
    return (ShoggothRoleApplication*) Payload::getApplication();
}



/******************************************************************************
    Setters and getters
*/



double Teacher::getErrorLimit()
{
    limb -> getNet() -> lock();
    auto result = limb
    -> getNet()
    -> getConfig()
    -> getDouble( Path{ "teacher", "errorLimit" }, 1.0 );
    limb -> getNet() -> unlock();

    return result;
}



Teacher* Teacher::setIdErrorLayer
(
    string a
)
{
    idErrorLayer = a;
    return this;
}



Teacher* Teacher::setMode
(
    string a
)
{
    mode = a;
    return this;
}



string Teacher::getIdErrorLayer()
{
    return idErrorLayer;
}



ParamList* Teacher::getBatches()
{
    return batches;
}






/*
    Fill layer with value noise
*/
Teacher* Teacher::cmdValueToLayer
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
Teacher* Teacher::cmdValuesToLayer
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
Teacher* Teacher::cmdImageToLayer
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
Teacher* Teacher::cmdFolderToLayer
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



/******************************************************************************
    Events
*/



/*
    Run teacher loop
*/
void Teacher::onLoop()
{
    auto errorLimit = getErrorLimit();

    mon
    -> startTimer( Path{ "currentMks" })
    -> interval( Path{ "uptime" }, Path{ "currentMks" }, Path{ "startMks" })
    -> addInt( Path{ "count" })
    -> now( Path{ "last", "moment" } )
    -> setString( Path{ "config", "errorLayer" }, idErrorLayer )
    -> setDouble( Path{ "config", "errorLimit" }, errorLimit )
    -> setString( Path{ "config", "mode" }, mode )
    ;

    getLog() -> trapOn() -> begin( "Check error level" );

    /* Prepare Limb */
    limb
    -> getNet()
    -> syncToLimb( limb, false )
    -> swapValuesAndErrors
    (
        Actions{ READ_VALUES }, /* Action */
        TASK_TEACHER,           /* Role */
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

        mon
        -> setDouble( Path{ "last", "error" }, error )
        -> setDouble( Path{ "last", "errorDelta" }, error - errorLimit );

        /* Check error limit */
        if
        (
            error <= errorLimit ||
            lastChange < limb -> getLastChange()
        )
        {
            lastChange = limb -> getLastChange();

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
                            { WRITE_VALUES, WRITE_ERRORS }, /* Action */
                            TASK_TEACHER,       /* Role */
                            limb,               /* Participant object */
                            false
                        );
                    }
                    else
                    {
                        getLog()
                        -> warning( getCode() )
                        -> prm( "message", getMessage() )
                        -> dump( getDetails() );
                        setOk();
                    }
                }
                else
                {
                    getLog() -> warning( "Batch is not a object" );
                }
            }
            else
            {
                getLog() -> warning( "Batch section 'all' and 'list' not found " );
            }
            getLog() -> end();
        }
        else
        {
            getLog() -> trace( "Hight error rate" );
        }
    }
    else
    {
        getLog()
        -> warning( "Error layer not found" )
        -> prm( "id", idErrorLayer );
    }

    limb -> unlock();

    mon -> flush();

    getLog() -> end() -> trapOff();
}
