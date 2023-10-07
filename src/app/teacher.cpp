#include <iostream>

/* User libraries */
#include "teacher.h"
#include "../lib/math.h"
#include "../lib/rnd.h"
#include "../json/param_list_log.h"
#include "../lib/utils.h"
#include "teacher_consts.h"

using namespace std;



/*
    Constructor
*/
Teacher::Teacher
(
    Net* aNet
)
: Payload( aNet -> getApplication() ) /* Call parent constructor */
{
    aNet -> getLog() -> trace( "Create teacher" );
    batches = ParamList::create();
    net = aNet;
}



/*
    Destructor
*/
Teacher::~Teacher()
{
    batches -> destroy();
    getLog() -> trace( "Destroy teacher" );
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
    Methods
*/

Teacher* Teacher::task()
{
    getLog() -> begin( "Check" );

    /* Retrive error layer by id */
    auto error = net -> getLayers() -> getById( idErrorLayer );

    if( error != NULL )
    {
        /* Check error limit */
        if( error -> getValue() <= errorLimit )
        {
            /* Check new batch */
            getLog() -> begin( "New batch" );
            auto item = batches -> getRnd();
            if( item != NULL && item -> isObject() )
            {
                /* Batch precessing */
                auto batch = item -> getObject();
                batch -> loop
                (
                    [ this ]
                    ( Param* aParam )
                    {
                        /* Layer processing */
                        auto layerId = aParam -> getName();
                        auto jobs = aParam -> getObject();
                        auto layer = net -> getLayers() -> getById( layerId );
                        getLog()
                        -> begin( "Layer" )
                        -> prm( "id", layerId );
                        if( layer != NULL )
                        {
                            auto job = jobs -> getRnd();
                            if( job != NULL && job -> isObject() )
                            {
                                buildValueBuffer( job -> getObject(), layer );
                            }
                            else
                            {
                                getLog()
                                -> warning( "Job is not a object" );
                            }
                        }
                        else
                        {
                            getLog()
                            -> warning( "Layer not found" )
                            -> prm( "id", layerId );
                        }
                        getLog()
                        -> end();
                        return false;
                    }
                );
                net -> event( TEACHING_END );
            }
            else
            {
                getLog()
                -> warning( "Batch is not a object" );
            }
            getLog() -> end();
        }
    }
    else
    {
        getLog()
        -> warning( "Error layer not found" )
        -> prm( "id", idErrorLayer );
    }
    getLog() -> end();


    return this;
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
    Create buffer for job
*/
Teacher* Teacher::buildValueBuffer
(
    ParamList* aJob,
    Layer* aLayer
)
{
    ParamListLog::dump( getLog(), aJob );

    auto cmdName    = aJob -> getString( "cmd" );
    auto cmdCode    = stringToTeacherTask( cmdName );

    switch( cmdCode )
    {
        case TEACHER_TASK_NOISE:
        {
            aLayer -> noiseValue
            (
                aJob -> getInt( "seed", 0 ),
                aJob -> getDouble( "min", 0.0 ),
                aJob -> getDouble( "max", 1.0 )
            );
            break;
        }
        case TEACHER_TASK_HID:
        {
            break;
        }
        case TEACHER_TASK_IMAGE:
        {
            aLayer -> imageToValue
            (
                aJob -> getString( "file" )
            );
            break;
        }
        default:
        {
            getLog()
            -> warning( "Unknown batch command" )
            -> prm( "Command", cmdName );
            break;
        }
    }
    return this;
}
