/* System libraries */
#include <iostream>

/* Application libraryes */
#include "evolution_application.h"
#include "evolution_payload.h"



using namespace std;



EvolutionApplication::EvolutionApplication
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
    getLog() -> trace( "Evolution constructor" );
}



/*
    Creator
*/
EvolutionApplication* EvolutionApplication::create
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
{
    return new EvolutionApplication( aCount, aList );
}



/*
    Run application
*/
EvolutionApplication* EvolutionApplication::run()
{
    getLog() -> begin( "Start he Shoggoth evolution application" );

    prepareConfiguration();

    EvolutionPayload::create( this,  getNetId(), getNetVersion() )
    -> start()
    -> destroy();

    getLog() -> end( "Application stop" );

    return this;
}

