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
#include "processor.h"



using namespace std;



/*
    Constructor
*/
Processor::Processor
(
    ShoggothApplication* a
)
: Payload( a ) /* Call parent constructor */
{
    net = Net::create( a );

    getLog()
    -> trace( "Config source" )
    -> prm( "file", getApplication() -> getConfigFileName() );
}



/*
    Destructor
*/
Processor::~Processor()
{
    net -> destroy();
}



/*
    Creator
*/
Processor* Processor::create
(
    ShoggothApplication* a
)
{
    return new Processor( a );
}



/*
    Destructor
*/
void Processor::destroy()
{
    delete this;
}



ShoggothApplication* Processor::getApplication()
{
    return ( ShoggothApplication* ) Payload::getApplication();
}



/******************************************************************************
    Methods
*/



/*
    Activate payload
*/
void Processor::onActivate()
{
}



/*
    Main calc method
*/
void Processor::onLoop
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
                net -> applyConfig(  getApplication() -> getConfig() );
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

    net -> calc();
}
