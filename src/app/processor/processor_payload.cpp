/*
    Shoggoth processor payload
*/

#include <iostream>

/* Core libraryes */
#include "../../../../../lib/core/mon.h"
#include "../../../../../lib/core/moment.h"

/* Local libraries */
#include "processor_payload.h"



using namespace std;



/*
    Constructor
*/
ProcessorPayload::ProcessorPayload
(
    ShoggothApplication* aApp,
    std::string aId
)
/* Call parent constructor */
: PayloadEngine( (Application*) aApp, aId )
{
    aApp -> getLog() -> trace( "Processor creating" );
    net = aApp -> getNet();

    /* Create Processor monitor */
    mon = Mon::create( net -> getMonPath( "processor_payload.json" ))
    -> setString( Path{ "start", "source" }, "Processor payload" )
    -> startTimer( Path{ "start", "moment" });

    /* Create the limb */
    limb = LimbProcessor::create( ( Payload* )this, net );
}



/*
    Destructor
*/
ProcessorPayload::~ProcessorPayload()
{
    waitStop();

    /* Destroy Processor monitor */
    mon -> destroy();

    /* Destroy limb */
    limb -> destroy();

    /* Log report */
    getLog() -> trace( "Processor destroyd" );
}



/*
    Destructor
*/
void ProcessorPayload::destroy()
{
    delete this;
}



ShoggothApplication* ProcessorPayload::getApplication()
{
    return ( ShoggothApplication* ) PayloadEngine::getApplication();
}




LimbProcessor* ProcessorPayload::getLimb()
{
    return limb;
}




/******************************************************************************
    Events
*/



/*
    Processor main loop event
    TODO Its need to analize. We must use onEngineLoop !!!
*/
void ProcessorPayload::onLoop()
{
    limb -> calc() -> resultTo( this );

    mon
    -> now( Path{ "current", "now" } )
    -> startTimer( Path{ "current", "moment" } )
    -> interval
    (
        Path{ "current", "uptime" },
        Path{ "current", "moment" },
        Path{ "start", "moment" }
    )
    -> addInt( Path{ "current", "loop" } )
    -> setInt( Path{ "config", "loopTimeoutMcs" }, getLoopTimeoutMcs() )
    -> intervalScale
    (
        Path{ "current", "uptimeSec" },
        Path{ "current", "moment" },
        Path{ "start", "moment" },
        SECOND
    )
    -> div
    (
        Path{ "current", "loopAvg" },
        Path{ "current", "loop" }, Path{ "current", "uptimeSec" }
    )
    -> dumpResult( Path{ "result" }, this )
    -> flush()
    ;
}



/*
    Processor resume action
    Main app thread
*/
void ProcessorPayload::onStartBefore()
{
    getLog() -> trace( "Processor starting" );
}



/*
    Server pause action
*/
void ProcessorPayload::onStopBefore()
{
    getLog() -> begin( "Processor stoping" ) -> lineEnd();
    getLog() -> end() -> lineEnd();;
}



/*
    Processor pause action
*/
void ProcessorPayload::onStopAfter()
{
    getLog() -> trace( "Processor stoped" ) -> lineEnd();
}




