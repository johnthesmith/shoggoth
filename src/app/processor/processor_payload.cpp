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
    Return application object
*/
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
*/
void ProcessorPayload::onEngineLoop
(
    bool
)
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
        Path{ "current", "loop" },
        Path{ "current", "uptimeSec" }
    )
    -> addInt( Path{ "results", getCode() } )
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
