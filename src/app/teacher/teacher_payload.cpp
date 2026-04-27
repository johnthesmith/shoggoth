/*
    Teacher payload
*/

/* Vanilla libraris */
#include <iostream>
#include <cmath>

/* Shoggoth libraries */
#include "../../../../../lib/core/str.h"

/* User libraries */
#include "teacher_payload.h"
#include "teacher_consts.h"



using namespace std;



/*
    Constructor
*/
TeacherPayload::TeacherPayload
(
    ShoggothApplication* aApp,
    std::string aId
)
/* Call parent constructor */
: PayloadEngine
(
    (Application*) aApp,
    aId
)
{
    /* Set main net */
    net = aApp -> getNet();

    /* Define mon file */
    auto monFile = aApp -> getMonPath( aId + ".json" );

    /* Begin log */
    aApp -> getLog()
    -> trace( "Teacher creating" )
    -> prm( "id", aId )
    -> prm( "log", monFile );

    /* Create teacher monitor */
    mon = Mon::create( monFile )
    -> setString( Path{ "start", "source" }, aId )
    -> startTimer( Path{ "start", "moment" })
    -> flush()
    ;

    /* Create the limb */
    limb = LimbTeacher::create( (Payload*) this );

    aApp -> getLog() -> trace( "Teacher created" );
}



/*
    Destructor
*/
TeacherPayload::~TeacherPayload()
{
    waitStop();

    /* Destroy limb */
    limb -> destroy();

    /* Destroy Processor monitor */
    mon -> destroy();

    /* Log report */
    getLog() -> trace( "Teacher destroyed" );
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
ShoggothApplication* TeacherPayload::getApplication()
{
    return ( ShoggothApplication* ) PayloadEngine::getApplication();
}



LimbTeacher* TeacherPayload::getLimb()
{
    return limb;
}


/******************************************************************************
    Payload events
*/



/*
    Main loop event
*/
void TeacherPayload::onEngineLoop
(
    bool aConfigUpdated
)
{
    getLog() -> trapOn() -> begin( "Teacher processing" );
    /* Configuration */
    auto errorLimit     = getErrorLimit();
    auto idErrorLayer   = getConfig() -> getString( Path{ "idErrorLayer" });
    auto batches        = getConfig() -> getObject( Path{ "batches" });
    auto currentBatches = getConfig() -> getString( Path{ "currentBatches" });

    /* Monitoring */
    mon
    -> startTimer( Path{ "current-mks" } )
    -> now( Path{ "last", "moment" } )
    -> addInt( Path{ "count" })
    -> setString( Path{ "config", "error-layer" }, idErrorLayer )
    -> setDouble( Path{ "config", "error-limit" }, errorLimit )
    -> setString( Path{ "config", "current-batches" }, currentBatches )
    -> setString( Path{ "config", "log" }, getLog() -> getFileName() )
    ;

    /*
        Load data form net
    */
    net -> lock();
    /* Prepare structure */
    net -> syncToLimb( limb );
    /* Read values from net in to limb */
    auto readedValues = net
    -> valuesAndErrorsToLimb
    (
        limb,
        getConfig() -> getStringVector( Path{ "layers", "read-values" }),
        false
    );
    net -> unlock();

    /* Retrive error layer by id */
    auto errorLayer = limb -> getLayerById( idErrorLayer );

    if( errorLayer == NULL )
    {
        setResult( "error_layer_not_found" )
        -> getDetails()
        -> setString( "layer_id", idErrorLayer );
    }
    else
    {
        /* Calc error from error layer */
        auto error = errorLayer -> calcRmsValue();

        getLog()
        -> trace( "Compare" )
        -> prm( "error", error )
        -> prm( "error limit", errorLimit )
        ;

        mon
        -> setDouble( Path{ "last", "error" }, error )
        -> setDouble( Path{ "last", "error-delta" }, error - errorLimit );

        /* Reset all batch parameter for net mode changing */

        if( lastBatches != currentBatches )
        {
            currentIndexBatch   = -1;
            orderIndex          = 0;
            loopIndex           = 0;
            repeatIndex         = 0;
            net -> setRndSeedFromConfig();
        }

        /* Select new batch */
        if
        (
            /* Check error limit */
            error <= errorLimit ||
            /* Change net mode  */
            lastBatches != currentBatches ||
            /* Change net structure */
            lastChange < limb -> getLastChangeStructure()
        )
        {
            /* Check new batch */
            getLog() -> begin( "New batch" );

            lastBatches = currentBatches;
            lastChange = limb -> getLastChangeStructure();

            auto all = batches -> getObject
            (
                Path{ currentBatches, "all" }
            );

            if( all != NULL )
            {
                /* Find batch */
                auto currentItem = nextBatchItem
                (
                    batches -> getObject( Path{ currentBatches } )
                );

                if( currentItem != NULL )
                {
                    if( currentItem -> isObject() )
                    {
                        auto batch = ParamList::create()
                        -> copyFrom( all )
                        -> copyFrom( currentItem -> getObject() );

                        /* Batch precessing */
                        batch -> loop
                        (
                            [ this ]
                            ( Param* aParam )
                            {
                                auto obj = aParam -> getObject();
                                if( obj != NULL )
                                {
                                    auto command = obj -> getString( Path{ "cmd" });
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
                                            setResult( "unknown-command" )
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
                            net -> lock();
                            net -> valuesAndErrorsFromLimb
                            (
                                limb,
                                getConfig() -> getStringVector( Path{ "layers", "write-values" }),
                                false
                            );
                            net -> unlock();

                            setCode( "new_batch_" + currentBatches );
                        }
                    }
                    else
                    {
                        setCode( "batch_is_not_a_object" );
                    }
                }
                else
                {
                    setCode( "batch_not_found" );
                }
            }
            else
            {
                setCode( "all_sections_not_found" );
            }
            getLog() -> end();
        }
        else
        {
            setCode( "hight_error_rate" );
        }
    }

    mon
    -> setString( Path{ "net", "version" }, net -> getVersion() )

    -> setInt( Path{ "net", "layers" }, net -> getLayerList() -> getCount() )
    -> setInt( Path{ "net", "nerves" }, net -> getNerveList() -> getCount() )

    -> setInt( Path{ "limb", "layers" }, limb -> getLayerList() -> getCount() )
    -> setInt( Path{ "limb", "nerves" }, limb -> getNerveList() -> getCount() )

    -> setInt( Path{ "last", "order-index" }, orderIndex )
    -> setInt( Path{ "last", "loop-index" }, loopIndex )
    -> setInt( Path{ "last", "repeat-index" }, repeatIndex )

    -> addInt( Path{ "results",  getCode() }, 1)

    -> stopTimer( Path{ "current-mks" } )
    -> timerToString( Path{ "current-mks" }, Path{ "last", "processing" } )
    -> dumpResult( Path{ "result" }, this )
    -> flush()
    ;

    getLog() -> end() -> trapOff();
}



Param* TeacherPayload::nextBatchItem
(
    ParamList* aBatch
)
{
    Param* result = NULL;

    if( aBatch != NULL )
    {
        auto list = aBatch -> getObject( Path{ "list" });
        auto repeatCount = aBatch -> getInt( Path{ "repeat-count" });
        auto strategy = stringToTeacherStrategy
        (
            aBatch -> getString( Path{ "strategy" })
        );

        getLog() -> trace() -> prm
        (
            "Teacher strategy",
            teacherStrategyToString( strategy )
        );

        if( list != NULL )
        {
            if( repeatIndex == 0 || currentIndexBatch == -1 )
            {
                switch( strategy )
                {
                     default:
                     case TEACHER_STRATEGY_UNKNOWN :
                     case TEACHER_STRATEGY_LOOP:
                     {
                        if( loopIndex >= list -> getCount() )
                        {
                            loopIndex = 0;
                        }
                        result = list -> getByIndex( loopIndex );
                        currentIndexBatch = loopIndex;
                        loopIndex++;
                     }
                     break;
                     case TEACHER_STRATEGY_RANDOM:
                     {
                        auto c = list -> getCount();
                        currentIndexBatch = -1;
                        if( c > 0 )
                        {
                            currentIndexBatch = net -> getRnd() -> get( 0, c - 1 );
                            result = list -> getByIndex( currentIndexBatch );
                        }
                     }
                     break;
                     case TEACHER_STRATEGY_ORDER:
                     {
                        auto order = aBatch -> getObject( Path{ "order" });
                        if( order != NULL && order -> getCount() > 0 )
                        {
                            if( orderIndex >= order -> getCount() )
                            {
                                orderIndex = 0;
                            }

                            currentIndexBatch = order -> getInt( orderIndex );
                            if( currentIndexBatch >= 0 )
                            {
                                result = list -> getByIndex
                                (
                                    currentIndexBatch
                                );
                            }

                            orderIndex ++;
                        }
                     }
                     break;
                     case TEACHER_STRATEGY_PER:
                        /* TODO */
                        currentIndexBatch   = -1;
                     break;
                }
            }
            else
            {
                if( currentIndexBatch >= 0 )
                {
                    result = list -> getByIndex( currentIndexBatch );
                }
            }
        }

        repeatIndex++;
        if( repeatIndex >= repeatCount )
        {
            repeatIndex = 0;
        }
    }

    return result;
}


/******************************************************************************
    Getters and setters
*/


/*
    Return current error limit from Net config
*/
real TeacherPayload::getErrorLimit()
{
    net -> lock();
    auto result = net
    -> getConfig()
    -> getDouble( Path{ "teacher", "errorLimit" }, 1.0 );
    net -> unlock();
    return result;
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

    auto layerId = a -> getString( Path{ "layer" });
    auto layer = limb -> getLayerById( layerId );
    if( layer != NULL )
    {
        layer -> noiseValue
        (
            net -> getRnd(),
            a -> getDouble( Path{ "min" }, 0.0 ),
            a -> getDouble( Path{ "max" }, 1.0 )
        );
    }
    else
    {
        setResult( "layer_not_found" )
        -> getDetails()
        -> copyFrom( "task", a )
        ;
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

    auto layerId = a -> getString( Path{ "layer" });
    auto layer = limb -> getLayerById( layerId );

    if( layer != NULL )
    {
        layer -> fillValue( a -> getObject( Path{ "values" }, 0 ));
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
    auto layerId = a -> getString( Path{ "layer" });

    limb -> lock();
    auto layer = limb -> getLayerById( layerId );
    if( layer != NULL )
    {
        auto files = a -> getObject( Path{ "files" });
        if( files != NULL )
        {
            auto file = files -> getRndItem( net -> getRnd() ) -> getString();
            if( file != "" )
            {
                layer -> imageToValue
                (
                    file,
                    a -> getDouble( Path{ "rotate" }),
                    a -> getDouble( Path{ "zoomMin" }),
                    a -> getDouble( Path{ "zoomMax" }),
                    a -> getDouble( Path{ "shift" }),
                    this,
                    net -> getRnd()
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
    auto layerId = a -> getString( Path{ "layer" });

    limb -> lock();
    auto layer = limb -> getLayerById( layerId );
    if( layer != NULL )
    {
        auto folder = a -> getString( Path{ "folder" }, "" );
        if( folder != "" )
        {
            auto files = ParamList::create() -> filesFromPath( folder );
            auto file = files -> getRndItem( net -> getRnd() );
            if( file != NULL )
            {
                layer -> imageToValue
                (
                    folder +  "/" + file -> getString(),
                    a -> getDouble( Path{ "rotate" }),
                    a -> getDouble( Path{ "zoomMin" }),
                    a -> getDouble( Path{ "zoomMax" }),
                    a -> getDouble( Path{ "shift" }),
                    this,
                    net -> getRnd()
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

