#include <iostream>

/* User libraries */
#include "teacher.h"
#include "../../../../lib/core/math.h"
#include "../../../../lib/core/rnd.h"
#include "../../../../lib/json/param_list_log.h"
#include "../../../../lib/core/utils.h"
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
    batches = ParamList::create();
}



/*
    Destructor
*/
Teacher::~Teacher()
{
    batches -> destroy();
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
ShoggothApplication* Teacher::getApplication()
{
    return (ShoggothApplication*) Payload::getApplication();
}



/******************************************************************************
    Setters and getters
*/



Teacher* Teacher::setErrorLimit
(
    double a
)
{
    errorLimit = a;
    return this;
}



double Teacher::getErrorLimit()
{
    return errorLimit;
}



Teacher* Teacher::setIdErrorLayer
(
    string a
)
{
    idErrorLayer = a;
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
    Fill layer with values
*/
Teacher* Teacher::cmdValueToLayer
(
    ParamList* a
)
{
//    auto layerId = a -> getString( "layer" );
//    auto layer = localNet -> getLayers() -> getById( layerId );
//    if( layer != NULL )
//    {
//        layer -> noiseValue
//        (
//            a -> getInt( "seed", 0 ),
//            a -> getDouble( "min", 0.0 ),
//            a -> getDouble( "max", 1.0 )
//        );
//    }
//    else
//    {
//        getLog()
//        -> warning( "Layer not found" )
//        -> prm( "id", layerId );
//    }
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
//    auto layerId = a -> getString( "layer" );
//    auto layer = localNet -> getLayers() -> getById( layerId );
//    if( layer != NULL )
//    {
//        auto files = a -> getObject( "files" );
//        if( files != NULL )
//        {
//            auto file = files -> getRnd() -> getString();
//            if( file != "" )
//            {
//                layer -> imageToValue( file );
//            }
//        }
//    }
//    else
//    {
//        getLog()
//        -> warning( "Layer not found" )
//        -> prm( "id", layerId );
//    }
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
//    auto layerId = a -> getString( "layer" );
//    auto layer = localNet -> getLayers() -> getById( layerId );
//    if( layer != NULL )
//    {
//        auto folder = a -> getString( "folder", "" );
//        if( folder != "" )
//        {
//            auto files = ParamList::create() -> filesFromPath( folder );
//            auto file = files -> getRnd();
//            if( file != NULL )
//            {
//                layer -> imageToValue( folder +  "/" + file -> getString() );
//            }
//            files -> destroy();
//        }
//    }
//    else
//    {
//        getLog()
//        -> warning( "Layer not found" )
//        -> prm( "id", layerId );
//    }
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
//    getLog() -> begin( "Check error level" );
//
//    localNet -> event( TEACHER_BEGIN );
//
//    /* Retrive error layer by id */
//    auto errorLayer = localNet -> getLayers() -> getById( idErrorLayer );
//
//    if( errorLayer != NULL )
//    {
//        auto error = errorLayer -> getValue();
//
//        getLog()
//        -> trace( "Compare" )
//        -> prm( "error", error )
//        -> prm( "error limit", errorLimit )
//        ;
//
//        /* Check error limit */
//        if( error <= errorLimit )
//        {
//            /* Check new batch */
//            getLog() -> begin( "New batch" );
//
//            auto item = batches -> getRnd();
//            if( item != NULL && item -> isObject() )
//            {
//                /* Batch precessing */
//                auto batch = item -> getObject();
//                batch -> loop
//                (
//                    [ this ]
//                    ( Param* aParam )
//                    {
//                        auto obj = aParam -> getObject();
//                        if( obj != NULL )
//                        {
//                            auto command = obj -> getString( "cmd" );
//                            getLog()
//                            -> begin( "Command" )
//                            -> prm( "cmd", command );
//                            ParamListLog::dump( getLog(), obj, "Arguments" );
//                            switch( stringToTeacherTask( command ))
//                            {
//                                case TEACHER_CMD_VALUE_TO_LAYER:
//                                    cmdValueToLayer( obj );
//                                break;
//                                case TEACHER_CMD_IMAGE_TO_LAYER:
//                                    cmdImageToLayer( obj );
//                                break;
//                                case TEACHER_CMD_FOLDER_TO_LAYER:
//                                    cmdFolderToLayer( obj );
//                                break;
//                                case TEACHER_CMD_GUID_TO_LAYER:
//                                break;
//                                case TEACHER_CMD_HID_TO_LAYER:
//                                break;
//                                default:
//                                    getLog() -> warning( "Unknown command" );
//                                break;
//                            }
//                            getLog() -> end();
//                        }
//                        return false;
//                    }
//                );
//                localNet -> event( TEACHER_END );
//            }
//            else
//            {
//                getLog() -> warning( "Batch is not a object" );
//            }
//            getLog() -> end();
//        }
//        else
//        {
//            getLog() -> trace( "Hight error rate" );
//        }
//    }
//    else
//    {
//        getLog()
//        -> warning( "Error layer not found" )
//        -> prm( "id", idErrorLayer );
//    }
//    getLog() -> end();
}
