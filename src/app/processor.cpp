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
#include "../../../../lib/graph/log_points.h"
#include "../../../../lib/graph/draw_mode.h"
#include "../../../../lib/core/rnd.h"
#include "../../../../lib/core/hid.h"
#include "../../../../lib/core/moment.h"
#include "../../../../lib/core/utils.h"
#include "../../../../lib/json/json.h"

#include "../shoggoth/func.h"
#include "../shoggoth/shoggoth_consts.h"

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
    getLog() -> trace( "Create processor" );
    limb = LimbProcessor::create( aNet );
}





/*
    Destructor
*/
Processor::~Processor()
{
    limb -> destroy();
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
    limb -> calc();
}




/*
    Return limb
*/
LimbProcessor* Processor::getLimb()
{
    return limb;
}
