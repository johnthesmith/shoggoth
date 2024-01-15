#include <iostream>
#include <unistd.h>

/* File read */

#include <streambuf>
#include <sstream>
#include <fstream>

/* for config update control */
#include <sys/types.h>
#include <sys/stat.h>

/* Local libraries */
#include "../../../../lib/graph/log_points.h"
#include "../../../../lib/core/moment.h"
#include "../../../../lib/core/utils.h"

/* User libraries */
#include "loop.h"



using namespace std;



/*
    Constructor
*/
Loop::Loop
(
    ShoggothApplication* a
)
: Payload( a ) /* Call parent constructor */
{
    net = Net::create( a, a -> getSockManager());
}



/*
    Destructor
*/
Loop::~Loop()
{
    if( teacher != NULL)    teacher -> destroy();
    if( processor != NULL)  processor -> destroy();
    if( server != NULL)     server -> destroy();
    if( ui != NULL)         ui -> destroy();
    net -> destroy();
}



/*
    Creator
*/
Loop* Loop::create
(
    ShoggothApplication* a
)
{
    return new Loop( a );
}



/*
    Destructor
*/
void Loop::destroy()
{
    delete this;
}



/*
    Return application object
*/
ShoggothApplication* Loop::getApplication()
{
    return ( ShoggothApplication* ) Payload::getApplication();
}



Loop* Loop::processorControl()
{
    auto taskProc =
    getApplication()
    -> getConfig()
    -> getObject
    (
        Path { "tasks",  taskToString( TASK_PROC )}
    );

    if( taskProc != NULL && taskProc -> getBool( "enabled" ))
    {
        if( processor == NULL )
        {
            /* Create server and processor payloads */
            server      = Server::create( net );
            processor   = Processor::create( net );

            /* Run server and processor thread */
            server -> setId( "server_thread" ) -> loop( true );
            processor -> setId( "processor_thread" ) -> loop( true );
        }

        /* Apply config */
        processor
        -> getLimb()
        -> setLearningSpeed
        (
            taskProc -> getDouble
            (
                "learningSpeed",
                processor -> getLimb() -> getLearningSpeed()
            )
        )
        -> setWakeupWeight
        (
            taskProc
            -> getDouble( "wakeupWeight", processor -> getLimb() -> getWakeupWeight() )
        )
        -> setErrorNormalize
        (
            taskProc
            -> getDouble( "errorNormalize", processor -> getLimb() -> getErrorNormalize() )
        )
        -> setCalcDebug
        (
            taskProc
            -> getBool( "debug", processor -> getLimb() -> getCalcDebug() )
        );


        server -> setLoopTimeoutMcs( 1000000 );
        server -> resume();

        processor
        -> setLoopTimeoutMcs
        (
            taskProc
            -> getDouble( "loopSleepMcs", processor -> getLoopTimeoutMcs() )
        );
        processor -> resume();
    }
    else
    {
        if( processor )
        {
            processor -> destroy();
            processor = NULL;

            server -> destroy();
            server = NULL;
        }
    }
    return this;
}



Loop* Loop::uiControl()
{
    auto cfg =
    getApplication()
    -> getConfig()
    -> getObject( Path{ "tasks", taskToString( TASK_UI )} );

    if( cfg != NULL && cfg -> getBool( "enabled" ))
    {
        if( ui == NULL )
        {
            ui = Ui::create( net );
            ui -> setId( "ui_thread" ) -> loop( true );
        }

        ui
        -> setLoopTimeoutMcs( cfg -> getDouble( "loopSleepMcs", 100 ))
        -> resume();
    }
    else
    {
        if( ui != NULL )
        {
            ui -> destroy();
            /* Reset UI and Scene */
            ui = NULL;
        }
    }
    return this;
}



Loop* Loop::teacherControl()
{
    auto cfg =
    getApplication()
    -> getConfig()
    -> getObject( Path{ "tasks", taskToString( TASK_TEACHER )} );

    if( cfg != NULL && cfg -> getBool( "enabled" ))
    {
        if( teacher == NULL )
        {
            teacher = Teacher::create(( Net* ) net );
            teacher -> setId( "teacher_thread" ) -> loop( true );
        }

        /* Read batches list */
        teacher -> getBatches() -> copyFrom( cfg -> getObject( Path{ "batches" }));
        /* Read layer with errors */
        teacher -> setIdErrorLayer( cfg -> getString( "idErrorLayer" ));
        /* Read error limit */
        teacher -> setErrorLimit( cfg -> getDouble( "errorLimit" ));

        teacher
        -> setLoopTimeoutMcs
        (
            cfg
            -> getDouble( "loopSleepMcs", teacher -> getLoopTimeoutMcs() )
        )
        -> resume();
    }
    else
    {
        if( teacher )
        {
            teacher -> destroy();
            teacher = NULL;
        }
    }

    return this;
}



/******************************************************************************
    Payload events
*/



/*
    Main loop event
*/
void Loop::onLoop()
{
    net -> event( LOOP_BEGIN );

    /* Set ok */
    getApplication() -> checkConfigUpdate();
    if
    (
        getApplication() -> getConfigUpdated() ||
        ! getApplication() -> getConfig() -> isOk()
    )
    {
        getLog()
        -> begin( "Config updated" )
        -> prm( "File", getApplication() -> getConfigFileName() )
        -> lineEnd();

        if( getApplication() -> getConfig() -> isOk())
        {
            this -> setOk();
            /* Set pause for main thread */
            setLoopTimeoutMcs
            (
                getApplication()
                -> getConfig()
                -> getDouble( "loopSleepMcs", getLoopTimeoutMcs() )
            );

            /* Paused processes */
            if( processor != NULL ) processor -> pause();
            if( server != NULL )    server -> pause();
            if( teacher != NULL )   teacher -> pause();
            if( ui != NULL )        ui -> pause();

            /* Process pause waiting */
            if( processor != NULL ) processor -> waitPause();
            if( server != NULL )    server -> waitPause();
            if( teacher != NULL )   teacher -> waitPause();
            if( ui != NULL )        ui -> waitPause();

            /*
                TODO - net must reload from IO each time or timeout
                Config apply
            */
            net -> readNet();

            /* Reinit process */
            processorControl();
            teacherControl();
            uiControl();
        }
        else
        {
            /* Config error */
            getLog()
            -> warning( "Config error" )
            -> prm
            (
                "message",
                getApplication()
                -> getConfig()
                -> getMessage()
            );

            getApplication()
            -> getConfig()
            -> resultTo( this )
            -> setOk();
        }

        getLog() -> end();

        net -> event( LOOP_END );
    }



//            aTreminated = scene -> windowClosed();
}
