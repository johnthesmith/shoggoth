#include <iostream>

/* User libraries */
#include "loop.h"
#include "../../../../lib/core/moment.h"



using namespace std;


/*
    Constructor
*/
Loop::Loop
(
    ShoggothApplication* a,
    string aNetId,
    string aNetVersion
)
: Payload( a ) /* Call parent constructor */
{
    net = Net::create( a, a -> getSockManager(), aNetId, aNetVersion );
}



/*
    Destructor
*/
Loop::~Loop()
{
//    if( teacher != NULL)    teacher -> destroy();
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
    ShoggothApplication* a,
    string aNetId,
    string aNetVersion
)
{
    return new Loop( a, aNetId, aNetVersion );
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
            server -> setId( net -> getLogPath( "server_thread" )) -> loop( true );
            processor -> setId( net -> getLogPath( "processor_thread" )) -> loop( true );
        }

        /* Apply config for processor */
        processor
        -> getLimb()
        -> setLearningSpeed( taskProc -> getDouble( "learningSpeed", 0.001 ))
        -> setMinWeight( taskProc -> getDouble( "minWeight", 1.0e-5 ))
        -> setMaxWeight( taskProc -> getDouble( "maxWeight", 1.0e5 ))
        -> setMaxError( taskProc -> getDouble( "maxError", 0.01 ))
        -> setTickWrite( taskProc -> getInt( "tickWrite", 0 ))
        -> setTickChart( taskProc -> getInt( "tickChart", 0 ))
        -> setDumpConf( taskProc -> getObject( "dump" ))
//        -> setLoopTimeoutMcs( taskProc -> getInt( "loopSleepMcs", 1000000 ))
        ;

        /* Apply config for server */
        server -> setLoopTimeoutMcs( 1000000 );

        server -> resume();
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



//Loop* Loop::teacherControl()
//{
//    auto cfg =
//    getApplication()
//    -> getConfig()
//    -> getObject( Path{ "tasks", taskToString( TASK_TEACHER )} );
//
//    if( cfg != NULL && cfg -> getBool( "enabled" ))
//    {
//        if( teacher == NULL )
//        {
//            teacher = Teacher::create(( Net* ) net );
//            teacher -> setId( net -> getLogPath( "teacher_thread" )) -> loop( true );
//        }
//
//        /* Read batches list and other config */
//        teacher -> getBatches() -> clear() -> copyFrom( cfg -> getObject( Path{ "batches" }));
//        teacher -> setIdErrorLayer( cfg -> getString( "idErrorLayer" ));
//        teacher -> setMode( cfg -> getString( "mode" ));
//
//        teacher
//        -> setLoopTimeoutMcs
//        (
//            cfg
//            -> getDouble( "loopSleepMcs", teacher -> getLoopTimeoutMcs() )
//        )
//        -> resume();
//    }
//    else
//    {
//        if( teacher )
//        {
//            teacher -> destroy();
//            teacher = NULL;
//        }
//    }
//
//    return this;
//}



/******************************************************************************
    Payload events
*/



/*
    Main loop event
*/
void Loop::onLoop()
{
    getLog() -> trapOn() -> begin( "Loop" );

    getMon() -> startTimer( Path{ "loop", "moment" });

    /* Check local application config */
    getApplication() -> checkConfigUpdate();

    /* Check server net config */
    auto netConfig = ParamList::create();
    /* Read net config from server */
    net -> readNet( netConfig );

    /* Monitoring */
    getMon()
    -> setInt
    (
        Path{ "loop", "netConfigUpdate" },
        netConfig -> getInt( "lastUpdate" ) * SECOND
    )
    -> setInt
    (
        Path{ "loop", "lastNetConfig" },
        net -> getLastUpdate() * SECOND
    )
    -> interval
    (
        Path{ "loop", "netConfigUpdateStr" },
        Path{ "loop", "moment" },
        Path{ "loop", "netConfigUpdate" }
    )
    -> interval
    (
        Path{ "loop", "lastNetConfigStr" },
        Path{ "loop", "moment" },
        Path{ "loop", "lastNetConfig" }
    );

    auto netConfigUpdated = net -> isConfigUpdate( netConfig );
    auto netVersionChanged = net -> isVersionChanged();
    auto appConfigUpdated = getApplication() -> getConfigUpdated();

    if( netConfigUpdated || netVersionChanged || appConfigUpdated )
    {
        getLog()
        -> begin( "Restarting" )
        -> info( "by reason" )
        -> prm( "Net updated", netConfigUpdated )
        -> prm( "Net version changed", netVersionChanged )
        -> prm( "App config updated", appConfigUpdated)
        -> prm( "File", getApplication() -> getConfigFileName() )
        -> lineEnd();

        /* Dump net config if changed */
        if( netConfigUpdated )
        {
            getLog() -> dump( netConfig, "Net config" );
        }


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

            getLog() -> begin( "Threads stoping" );

            /* Paused processes */
            if( processor != NULL ) processor -> pause();
            if( server != NULL )    server -> pause();
//            if( teacher != NULL )   teacher -> pause();
            if( ui != NULL )        ui -> pause();

            getLog() -> begin( "Threads waiting begin" );

            /* Process pause waiting */
            if( processor != NULL ) processor -> waitPause();
            if( server != NULL )    server -> waitPause();
//            if( teacher != NULL )   teacher -> waitPause();
            if( ui != NULL )        ui -> waitPause();

            getLog() -> end( "" );
            getLog() -> end( "Threads stoped" );

            /* Apply config */
            net -> applyNet( netConfig );

            /* Reinit process */
            processorControl();
//            teacherControl();
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
    }

    net -> syncWithServer();

    netConfig -> destroy();

    if( getApplication() -> getCli() -> getString( "config" ) == "" )
    {
        getLog() -> error( "Need the --config cli argument." );
        terminate();
    }


    getMon() -> flush();
    getLog()
    -> end( getCode() )
    -> trapOff();
}
