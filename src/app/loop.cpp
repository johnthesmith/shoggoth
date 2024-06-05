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
        -> setMinWeight
        (
            taskProc
            -> getDouble( "minWeight", processor -> getLimb() -> getMinWeight() )
        )
        -> setMaxWeight
        (
            taskProc
            -> getDouble
            (
                "maxWeight",
                processor -> getLimb() -> getMaxWeight()
            )
        )
        -> setMaxError
        (
            taskProc
            -> getDouble
            (
                "maxError",
                processor -> getLimb() -> getMaxError()
            )
        )
        -> setTickWrite
        (
            taskProc
            -> getInt
            (
                "tickWrite",
                processor -> getLimb() -> getTickWrite()
            )
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

        /* Read batches list and other config */
        teacher -> getBatches() -> copyFrom( cfg -> getObject( Path{ "batches" }));
        teacher -> setIdErrorLayer( cfg -> getString( "idErrorLayer" ));
        teacher -> setMode( cfg -> getString( "mode" ));

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


    if
    (
        net -> isConfigUpdate( netConfig ) ||
        net -> isVersionChanged() ||
        getApplication() -> getConfigUpdated()
    )
    {
        getLog()
        -> begin( "Config updated" )
        -> prm( "File", getApplication() -> getConfigFileName() )
        -> dump( netConfig, "Net config" )
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

            /* Apply config */
            net -> applyNet( netConfig );

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
