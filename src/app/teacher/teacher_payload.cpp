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
: PayloadEngine( (Application*) aApp, aId )
{
    aApp -> getLog() -> trace( "Teacher creating" );

    net = aApp -> getNet();

    /* Create the limb */
    limb = LimbTeacher::create( (Payload*) this, net );
}



/*
    Destructor
*/
TeacherPayload::~TeacherPayload()
{
    waitStop();

    /* Destroy limb */
    limb -> destroy();

    /* Log report */
    getLog() -> trace( "Teacher destroyd" );
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
    TODO Its need to analize. We must use onEngineLoop !!!
*/
void TeacherPayload::onLoop()
{
//    getMon()
//    -> setString( Path{ "net", "id" }, net -> getId() )
//    -> setString( Path{ "net", "version" }, net -> getVersion() )
//    ;
//
//    /* Check server net config */
//    auto netConfig = ParamList::create();
//    /* Read net config from server */
//    net -> setOk();
//    net -> readNet( netConfig );
//
//    if
//    (
//        net -> isConfigUpdate( netConfig ) ||
//        net -> isVersionChanged()
//    )
//    {
//        getLog()
//        -> begin( "Net config updated" )
//        -> prm( "File", getApplication() -> getConfigFileName() )
////            -> dump( netConfig, "Net config" )
//        -> lineEnd();
//        net -> applyNet( netConfig );
//        getLog() -> end();
//    }
//    netConfig -> destroy();
//
//    net -> resultTo( this );
//
//    if( isOk() )
//    {
//        /* Synchronize net from the Shoggoth server */
//        net
//        -> syncWithServer()
//        -> resultTo( this );
//    }
//
//    getLog() -> trace( "Server tick" ) -> prm( "number", net -> getTick() );
//
//    if( isOk() )
//    {
//        /* Processing Teacher */
//        getLog() -> begin( "Teacher processing" );
//
//        auto errorLimit     = getErrorLimit();
//        auto idErrorLayer   = getIdErrorLayer();
//        auto batches        = getBatches();
//
//        auto currentBatches = getApplication() -> getConfig() -> getString
//        (
//            Path{ "engine", "teacher", "currentBatches" },
//            requestCurrentBatches()
//        );
//
//        getMon()
//        -> startTimer( Path{ "currentMks" } )
//        -> addInt( Path{ "count" })
//        -> now( Path{ "last", "moment" } )
//        -> setString( Path{ "config", "errorLayer" }, idErrorLayer )
//        -> setDouble( Path{ "config", "errorLimit" }, errorLimit )
//        -> setString( Path{ "config", "currentBatches" }, currentBatches )
//        ;
//
//        /* Prepare Limb */
//        limb -> getNet() -> syncToLimb( limb, false );
//
//        bool readedValues = false;
//        bool writedValues = false;
//
//        limb -> getNet() -> swapValuesAndErrors
//        (
//            Actions{ READ_VALUES },
//            /* Destination participant object */
//            limb,
//            false,
//            readedValues,
//            writedValues
//        );
//
//        limb -> lock();
//
//        /* Retrive error layer by id */
//        auto errorLayer = limb -> getLayerById( idErrorLayer );
//        if( errorLayer != NULL )
//        {
//            auto error = errorLayer -> calcRmsValue();
//
//            getLog()
//            -> trace( "Compare" )
//            -> prm( "error", error )
//            -> prm( "error limit", errorLimit )
//            ;
//
//            getMon()
//            -> setDouble( Path{ "last", "error" }, error )
//            -> setDouble( Path{ "last", "errorDelta" }, error - errorLimit );
//
//
//////TODO 
////
////// При сохранении тичеровского конфига тичер падает
////
////// надо сделать функцию Не
////
//////https://chat.deepseek.com/a/chat/s/987737ac-396f-4d10-8e6c-16ccac9fd86e
////// writeErrorsBeforeChange - теперь этот RMS и надо проверить где мы его юзаем в эволюшене
////
//////ранше ошибка 0 в процессе обучения тичером воспринималась бы как гарантия завершения обучения
//////сейчас это не так - потому точ ошибка может быть отключена
////
//////поставить еррорлимит только для режима случайного обучения а для ORDER он не применим
//////.... надо подумть!
////
////                /*
////                    Write layers values from config:batches.*.controlDump
////                */
//////                if
//////                (
//////                    error <= errorLimit &&
//////                    lastNetMode == NET_MODE_TEST
//////                )
//////                {
//////                    getLog() -> trace( "error < error limit" );
//////
//////                    auto layers = batches -> getObject( Path{ mode, "controlDump" } );
//////                    if( layers != NULL )
//////                    {
//////                        vector <string> values;
//////
//////                        layers -> loop
//////                        (
//////                            [ this, &values ]
//////                            ( Param* item )
//////                            {
//////                                auto layerId = item -> getString();
//////                                auto layer = net -> getLayerList() -> getById( layerId );
//////                                if( layer != NULL )
//////                                {
//////                                    values.push_back( layer -> getValuesString() );
//////                                }
//////                                return false;
//////                            }
//////                        );
//////
//////                        /* Send resul to server */
//////                        Io::create( net )
//////                        -> testResult
//////                        (
//////                            net -> getVersion(),
//////                            net -> getTick(),
//////                            implode( values, " | " )
//////                        )
//////                        -> resultTo( this )
//////                        -> destroy();
//////
//////                    }
//////                    testId++;
//////                }
//
//            /* Reset all batch parameter for net mode changing */
//            if( lastBatches != currentBatches )
//            {
//                currentIndexBatch   = -1;
//                orderIndex          = 0;
//                loopIndex           = 0;
//                repeatIndex         = 0;
//                net -> setRndSeedFromConfig();
//            }
//
//            /* Select new batch */
//            if
//            (
//                /* Check error limit */
//                error <= errorLimit ||
//                /* Change net mode on server */
//                lastBatches != currentBatches ||
//                /* Change net structure on server */
//                lastChange < limb -> getLastChangeStructure()
//            )
//            {
//                /* Check new batch */
//                getLog() -> begin( "New batch" );
//
//                lastBatches = currentBatches;
//                lastChange = limb -> getLastChangeStructure();
//
//                auto all = batches -> getObject
//                (
//                    Path{ currentBatches, "all" }
//                );
//
//                if( all != NULL )
//                {
//                    auto currentItem = nextBatchItem
//                    (
//                        batches -> getObject( Path{ currentBatches } )
//                    );
//
//                    if( currentItem != NULL )
//                    {
//                        if( currentItem -> isObject() )
//                        {
//                            auto batch = ParamList::create()
//                            -> copyFrom( all )
//                            -> copyFrom( currentItem -> getObject() );
//
//                            /* Batch precessing */
//                            batch -> loop
//                            (
//                                [ this ]
//                                ( Param* aParam )
//                                {
//                                    auto obj = aParam -> getObject();
//                                    if( obj != NULL )
//                                    {
//                                        auto command = obj -> getString( Path{ "cmd" });
//                                        getLog()
//                                        -> begin( "Command" )
//                                        -> prm( "cmd", command )
//                                        -> dump( obj, "Arguments" );
//
//                                        switch( stringToTeacherTask( command ))
//                                        {
//                                            case TEACHER_CMD_VALUE_TO_LAYER:
//                                                cmdValueToLayer( obj );
//                                            break;
//                                            case TEACHER_CMD_VALUES_TO_LAYER:
//                                                cmdValuesToLayer( obj );
//                                            break;
//                                            case TEACHER_CMD_IMAGE_TO_LAYER:
//                                                cmdImageToLayer( obj );
//                                            break;
//                                            case TEACHER_CMD_FOLDER_TO_LAYER:
//                                                cmdFolderToLayer( obj );
//                                            break;
//                                            case TEACHER_CMD_GUID_TO_LAYER:
//                                            break;
//                                            case TEACHER_CMD_HID_TO_LAYER:
//                                            break;
//                                            default:
//                                                setResult( "Unknown command" )
//                                                -> getDetails()
//                                                -> setString( "command", command );
//                                            break;
//                                        }
//                                        getLog() -> end();
//                                    }
//                                    return false;
//                                }
//                            );
//
//                            /* Destroy the batch */
//                            batch -> destroy();
//
//                            if( isOk() )
//                            {
//                                /* Upload values and errors to net */
//                                bool readedValues = false;
//                                bool writedValues = false;
//                                limb -> getNet() -> swapValuesAndErrors
//                                (
//                                    /* Action */
//                                    { WRITE_VALUES, WRITE_ERRORS },
//                                    /* Participant object */
//                                    limb,
//                                    false,
//                                    readedValues,
//                                    writedValues
//                                );
//                                setCode("new_batch_" + currentBatches );
//                            }
//                        }
//                        else
//                        {
//                            setCode( "batch_is_not_a_object" );
//                        }
//                    }
//                    else
//                    {
//                        setCode( "batch_not_found" );
//                    }
//                }
//                else
//                {
//                    setCode( "batch_sections_not_found" );
//                }
//                getLog() -> end();
//            }
//            else
//            {
//                setCode( "hight_error_rate" );
//            }
//        }
//        else
//        {
//            setCode( "error_layer_not_found" );
//            getDetails() -> setString( "layer_id", idErrorLayer );
//        }
//
//        limb -> unlock();
//        getLog() -> end();
//    }
//
//    getMon()
//    -> setInt( Path{ "last", "orderIndex" }, orderIndex )
//    -> setInt( Path{ "last", "loopIndex" }, loopIndex )
//    -> setInt( Path{ "last", "repeatIndex" }, repeatIndex )
//    ;
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
    return getApplication() -> getConfig() -> getString
    (
        Path{ "engine", "teacher", "idErrorLayer" }
    );
}


/*
*/
std::string TeacherPayload::requestCurrentBatches()
{
    std::string result = "";
//
//    ShoggothRpcClient::create( net )
//    -> getNetMode( result )
//    -> resultTo( this )
//    -> destroy();

    return result;
}




bool TeacherPayload::getEnabled()
{
    return getApplication()
    -> getConfig()
    -> getBool( Path{ "enabled" }, true );
}



ParamList* TeacherPayload::getBatches()
{
    return getApplication() -> getConfig() -> getObject
    (
        Path{ "engine", "teacher", "batches" }
    );
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
        -> setString( Path{ "layer id" }, layerId );
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

