#include <iostream>
#include <thread>   /* For Processor thread */

/* Core libraryes */
#include "../../../../../lib/core/mon.h"

/* Local libraries */
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
: PayloadEngine( aNet -> getApplication(), "processor" )
{
    net = aNet;
    net -> getApplication() -> getLog() -> trace( "Create Processor" );

    /* Create Processor monitor */
    mon = Mon::create( aNet -> getMonPath( "brain_processor.json" ))
    -> setString( Path{ "start", "source" }, "Processor payload" )
    -> startTimer( Path{ "start", "moment" });

    /* Create the limb */
    limb = LimbProcessor::create( net );
}



/*
    Destructor
*/
Processor::~Processor()
{
    stop();
    waitStop();

    /* Destroy Processor monitor */
    mon -> destroy();

    /* Destroy limb */
    limb -> destroy();

    /* Log report */
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



BrainApplication* Processor::getApplication()
{
    return ( BrainApplication* ) PayloadEngine::getApplication();
}




LimbProcessor* Processor::getLimb()
{
    return limb;
}




/******************************************************************************
    Events
*/



/*
    Processor main loop event
*/
void Processor::onLoop()
{
    limb -> calc();

    mon
    -> now( Path{ "current", "now" } )
    -> startTimer( Path{ "current", "moment" } )
    -> interval( Path{ "current", "uptime" }, Path{ "current", "moment" }, Path{ "start", "moment" })
    -> addInt( Path{ "current", "loop" } )
    -> setInt( Path{ "config", "loopTimeoutMcs" }, getLoopTimeoutMcs() )
    -> dumpResult( Path{ "result" }, this )
    -> flush()
    ;
}



/*
    Processor resume action
    Main app thread
*/
void Processor::onStartBefore()
{
    getLog() -> trace( "Processor starting" );
}



/*
    Processor pause action
*/
void Processor::onStopAfter()
{
    getLog() -> trace( "Processor stoped" ) -> lineEnd();
}
