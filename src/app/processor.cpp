#include <iostream>

/* File read */
#include <streambuf>
#include <sstream>
#include <fstream>
#include <thread>

/* for config update control */
#include <sys/types.h>
#include <sys/stat.h>


/* Local libraries */
#include "../graph/log_points.h"
#include "../graph/draw_mode.h"

#include "../shoggoth/neuron.h"
#include "../shoggoth/func.h"
#include "../shoggoth/shoggoth_consts.h"

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
    Net* aNet
)
/* Call parent constructor */
: Payload( aNet -> getApplication() )
{
    net = aNet;
    net -> getApplication() -> getLog() -> trace( "Create processor" );
}





/*
    Destructor
*/
Processor::~Processor()
{
    getLog() -> trace( "Processor destroyd" );
}



/*
    Creator
*/
Processor* Processor::create
(
    Net* a
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
    Run net calculateion
*/
void Processor::onLoop()
{
    net -> calc();
}
