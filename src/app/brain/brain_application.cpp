/* System libraries */
#include <iostream>

/* Application libraryes */
#include "brain_application.h"
#include "brain_payload.h"




/*
    Brain constructor
*/
BrainApplication::BrainApplication
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
    getLog() -> trace( "Brain constructor" );
}



/*
    Create and return Brain application
*/
BrainApplication* BrainApplication::create
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
{
    return new BrainApplication( aCount, aList );
}



/*
    Run application
*/
BrainApplication* BrainApplication::run()
{
    getLog() -> begin( "Start the Shoggoth Brain application" );

    prepareConfiguration();

    BrainPayload::create( this,  getNetId(), getNetVersion() )
    -> start()
    -> destroy();

    getLog() -> end( "Application stop" );

    return this;
}

