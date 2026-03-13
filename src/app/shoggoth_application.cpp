/* Application libraryes */
#include "shoggoth_application.h"
#include "../shoggoth/net.h"


/*
    Constructor of the Shogoth application
*/
ShoggothApplication::ShoggothApplication
(
    /* cli argumends count */
    int aCount,
    /* cli arguments */
    char** aList
)
:Application
(
    aCount,
    aList
)
{
    getLog() -> begin( "Shoggoth application start" );

    /* Create sock manager */
    sockManager = SockManager::create();

    /* Create main net */
    net = Net::create
    (
        this,
        sockManager,
        "",
        getNetVersion(),
        TASK_PROC
    );

    /* Registarte signal */
    registerSignal( SIGINT );
    registerSignal( SIGTERM );
    registerSignal( SIGSEGV );
    registerSignal( SIGQUIT );
}



/*
    Destructor of the Shogoth application
*/
ShoggothApplication::~ShoggothApplication()
{
    /* Destroy net */
    net -> destroy();

    /* DEstroy sock manager */
    sockManager -> destroy();

    getLog() -> end( "Shoggoth stop" );
}



/*
    End of thread
*/
ShoggothApplication* ShoggothApplication::onThreadAfter()
{
    getSockManager() -> closeHandlesByThread( "" );
    return this;
}

