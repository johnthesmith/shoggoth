#include <algorithm>

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

    /* Create new param list for config */
    netConfig = ParamList::create();

    /* Create main net */
    net = Net::create
    (
        this,
        "",
        getNetVersion()
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

    /* Destroy net config */
    netConfig -> destroy();

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



/*
    Return true value if layer contains action for current net task
*/
bool ShoggothApplication::checkActionValues
(
    std::string aPayloadId,
    std::string aLayerId,
    std::string aActionId
)
{
    auto actionLayers = getConfig() -> getObject
    (
        Path
        {
            "engine",
            "payloads",
            aPayloadId,
            "layers",
            aActionId
        }
    );
    return actionLayers != nullptr && actionLayers -> contains( aLayerId );
}





/*
    Generate event before main application loop in run method
*/
ShoggothApplication* ShoggothApplication::onBeforeLoop()
{
    netConfig -> lock();
    net -> lock();

    auto netConfigUpdate = netConfig -> getInt
    (
        Path{ "last-update" },
        0
    );

    if( netConfigUpdate != net -> getLastUpdate())
    {
        net -> applyConfig( getNetConfig(), getConfig() );
    }

    net -> unlock();
    netConfig -> unlock();

    return this;
}
