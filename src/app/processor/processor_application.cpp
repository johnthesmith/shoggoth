/* System libraries */
#include <iostream>

/* Application libraryes */
#include "processor_application.h"
#include "processor_payload.h"



using namespace std;


/*
    Processor constructor
*/
ProcessorApplication::ProcessorApplication
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
:ShoggothApplication
(
    aCount,
    aList
)
{
    getLog() -> trace( "Processor constructor" );
}



/*
    Create and return Processor application
*/
ProcessorApplication* ProcessorApplication::create
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
{
    return new ProcessorApplication( aCount, aList );
}



/*
    Run application
*/
ProcessorApplication* ProcessorApplication::run()
{
    getLog() -> begin( "Start the Shoggoth Processor application" );

    prepareConfiguration();

    ProcessorPayload::create( this,  getNetId(), getNetVersion() )
    -> resume()
    -> loop()
    -> destroy();

    getLog() -> end( "Application stop" );

    return this;
}

