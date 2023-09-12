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
    net = Net::create( a );
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
            getApplication() -> getConfig() -> setOk();
            getLog()
            -> begin( "Config updated" )
            -> prm( "File", getApplication()
            -> getConfigFileName() );

            if( getApplication() -> getConfig() -> isOk())
            {
                /* Config apply */
                net -> applyConfig();
                aReconfig = true;
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
            }

            getLog() -> end();
        }

        /* Set last moment */
        lastConfigCheck = nowMoment;
    }

    
}
