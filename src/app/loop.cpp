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



bool Loop::serverControll()
{
    bool result =
    getApplication()
    -> getConfig()
    -> getBool( Path { "server", "enabled" } );
    if( result )
    {
        if( server == NULL )
        {
            server = ShoggothRpcServer::create( getApplication() );
            server -> setPort
            (
                getApplication()
                -> getConfig()
                -> getInt( Path{ "server", "port" }, 11120 )
            );
        }
    }
    else
    {
        if( server != NULL )
        {
            server -> destroy();
            server = NULL;
        }
    }
    return result;
}



Loop* Loop::uiControll()
{
    if
    (
        getApplication()
        -> getConfig()
        -> getBool( Path { "ui", "enabled" } ))
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
                -> getString( Path{ "ui", "fontName" } )
            )
            -> setGliphSize
            (
                getApplication()
                -> getConfig()
                -> getInt( Path{ "ui", "gliphSize" }, 16 )
            )
            -> setCharSet
            (
                getApplication()
                -> getConfig()
                -> getString( Path{ "ui", "charSet" })
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
    /* Read config */
    auto nowMoment = now();
    if( lastConfigCheck + MILLISECOND * 100 < nowMoment )
    {
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

                if( !serverControll() )
                {
                    /* Config apply */
                    net -> applyConfig();
                    aReconfig = true;

                    serverControll();
                    uiControll();
                }
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
    }


    if( isOk() )
    {
        /* Server */
        if( server != NULL )
        {
            server -> up();
        }
        else
        {
            /* Processor */

            /* UI works*/
            if( ui != NULL )
            {
                scene -> calcEvent();
                scene -> drawEvent();
                aTreminated = scene -> windowClosed();
            }
        }
    }
}




Net* event
(
    Event aEvent
)
{
}
