/* System libraries */
#include <iostream>

/* Application libraryes */
#include "debugger_application.h"
#include "debugger_payload.h"



using namespace std;


/*
    debugger constructor
*/
DebuggerApplication::DebuggerApplication
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
    getLog() -> trace( "Debugger constructor" );

    terminal = Terminal::create( getLog() );

    registerSignal(SIGINT);
    registerSignal(SIGTERM);
    registerSignal(SIGSEGV);
    registerSignal(SIGQUIT);
}



DebuggerApplication::~DebuggerApplication()
{
    terminal -> destroy();
}



/*
    Create and return debugger application
*/
DebuggerApplication* DebuggerApplication::create
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
{
    return new DebuggerApplication( aCount, aList );
}




/*
    Run application
*/
DebuggerApplication* DebuggerApplication::run()
{
    getLog() -> begin( "Start the Shoggoth Debugger application" );

    payload = DebuggerPayload::create( this, "debugger", "", getNetVersion() );
    payload -> setLoopTimeoutMcs(0)
    -> start()
    -> destroy();

    getLog() -> end( "Application stop" );

    return this;
}



/*
    on signale event handler
*/
bool DebuggerApplication::onSignal
(
    int aSignal
)
{
    payload -> stop();
    return true;
}



Terminal* DebuggerApplication::getTerminal()
{
    return terminal;
}
