/*
    Memory payload
*/

/* Vanilla libraris */
#include <iostream>

/* Shoggoth libraries */
#include "../../../../../lib/core/str.h"
//#include "../../../../../lib/json/param_list.h"

/* User libraries */
#include "memory_payload.h"



using namespace std;




/******************************************************************************
    Payload events
*/


/*
    Main loop event
*/
void MemoryPayload::onEngineLoop( bool )
{
    getLog() -> begin( "Memory processing" );

    getMon()
    -> setString( Path{ "net", "id" }, net -> getId() )
    -> setString( Path{ "net", "version" }, net -> getVersion() )
    -> setUInt( Path{ "net", "tick" }, net -> getLearningTick() )
    ;

    auto header = readHeader();
    auto netTick = net -> getLearningTick();

    if( header.tick == netTick )
    {
        if( header.tick == 0 )
        {
            /* Generate new weights */
//            net -> fillWeights();
        }
        else
        {
            /* skip */
        }
    }
    else
    {
        if( header.tick < netTick )
        {
            /* Store net to disk*/
            dataFromNetToFile();
        }
        else
        {
           /* Read from disk to net */
           dataFromFileToNet();
        }
    }


    getMon()
    -> setString( Path{ "result", "last" }, getCode() )
    -> addInt( Path{ "result", "codes", getCode() })
    -> flush();

    getLog() -> end();
}

