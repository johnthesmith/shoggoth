#include <iostream>

/* File read */
#include <streambuf>
#include <sstream>
#include <fstream>

/* for config update control */
#include <sys/types.h>
#include <sys/stat.h>


/* Local libraries */
#include "../graph/log_points.h"
#include "../graph/draw_mode.h"

#include "../shoggoth/neuron.h"
#include "../shoggoth/func.h"

#include "../lib/rnd.h"
#include "../lib/hid.h"
#include "../lib/moment.h"
#include "../lib/utils.h"
#include "../json/json.h"

#include "../math.h"

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
    net = NetGraph::create( a );
}



/*
    Destructor
*/
Loop::~Loop()
{
    net -> destroy();
    if( teacher != NULL) teacher -> destroy();
    if( processor != NULL) processor -> destroy();
    if( server != NULL) server -> destroy();
    if( ui != NULL) ui -> destroy();
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



ShoggothApplication* Loop::getApplication()
{
    return ( ShoggothApplication* ) Payload::getApplication();
}




Loop* Loop::processorControl()
{
    auto taskProc =
    getApplication()
    -> getConfig()
    -> getObject( Path { "tasks",  taskToString( TASK_PROC )} );

    if( taskProc != NULL && taskProc -> getBool( "enabled" ))
    {
        if( processor == NULL )
        {
            processor   = Processor::create( net );
            server      = Server::create( net );
            server -> setId( "server" );
            server -> run( true );

            /* Apply config */
            net -> setLearningSpeed
            (
                taskProc
                -> getDouble( "learningSpeed", net -> getLearningSpeed() )
            );
            net -> setWakeupWeight
            (
                taskProc
                -> getDouble( "wakeupWeight", net -> getWakeupWeight() )
            );
            net -> setErrorNormalize
            (
                taskProc
                -> getDouble( "errorNormalize", net -> getErrorNormalize() )
            );
            net -> setCalcDebug
            (
                taskProc
                -> getBool( "debug", net -> getCalcDebug() )
            );
        }
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
    if
    (
        getApplication()
        -> getConfig()
        -> getBool( Path { "tasks", taskToString( TASK_UI ), "enabled" } ))
    {
        if( ui == NULL )
        {
            ui = Ui::create( getApplication(), net );
            scene  = Scene::create( getLog() );
            scene
            -> getFont()
            -> setFontName
            (
                getApplication()
                -> getConfig()
                -> getString( Path{ "tasks", taskToString( TASK_UI ), "fontName" } )
            )
            -> setGliphSize
            (
                getApplication()
                -> getConfig()
                -> getInt( Path{ "tasks", taskToString( TASK_UI ), "gliphSize" }, 16 )
            )
            -> setCharSet
            (
                getApplication()
                -> getConfig()
                -> getString( Path{ "tasks", taskToString( TASK_UI ), "charSet" })
            );

            scene
            -> init()
            -> setPayload( ui );
        }
    }
    else
    {
        if( ui != NULL )
        {
            scene
            -> finit()
            -> destroy();
            ui -> destroy();
            /* Reset UI and Scene */
            ui = NULL;
            scene = NULL;
        }
    }
    return this;
}



Loop* Loop::teacherControl()
{
    auto cfg = getApplication()
    -> getConfig()
    -> getObject( Path{ "tasks", taskToString( TASK_TEACHER )} );

    if( cfg != NULL && cfg -> getBool( "enabled" ))
    {
        if( teacher == NULL )
        {
            teacher = Teacher::create( (Net*) net );
        }
        /* Read config */
        /* Read batches list */
        teacher -> getBatches() -> copyFrom( cfg -> getObject( Path{ "batches" }));

        /* Read layer with errors */
        teacher -> setIdErrorLayer( cfg -> getString( "idErrorLayer" ));
        /* Read error limit */
        teacher -> setErrorLimit( cfg -> getDouble( "errorLimit" ));
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
    Activate payload
*/
void Loop::onActivate()
{
}



/*
    Main calc method
*/
void Loop::onLoop
(
    bool&           aTreminated,
    bool&           aIdling,
    unsigned int&   sleep,
    bool&           aReconfig
)
{
    getLog() -> begin( "Loop" );

    /* Read config */
    auto nowMoment = now();
    if( lastConfigCheck + MILLISECOND * 100 < nowMoment )
    {
        getLog() -> begin( "Check config updated" );

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
            -> prm( "File", getApplication()
            -> getConfigFileName() );

            if( getApplication() -> getConfig() -> isOk())
            {
                this -> setOk();
                /* Config apply */
                net -> readNet();
                aReconfig = true;
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

        /* Set last moment */
        lastConfigCheck = nowMoment;
        getLog() -> end();
    }


    if( isOk() )
    {
        /* Begin of net loop */
        if( net -> isNextLoop() )
        {
            net -> event( LOOP_BEGIN );
        }

        /* Teacher */
        if( teacher != NULL )
        {
            teacher -> task();
        }

        /* Processor */
        if( processor )
        {
            processor -> run();
        }

        /* UI works*/
        if( ui != NULL )
        {
            scene -> calcEvent();
            scene -> drawEvent();
            aTreminated = scene -> windowClosed();
        }
    }

    getLog() -> end();
}
