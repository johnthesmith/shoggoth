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



ShoggothApplication* ShoggothApplication::collectLayersUsing()
{
    /* Lock */
    std::lock_guard<std::mutex> lock( mLayersMutex );

    layersUsing.clear();

    auto payloads = getConfig() -> getObject
    (
        Path
        {
            "engine",
            "payloads"
        }
    );

    if( payloads != nullptr )
    {
        payloads -> loop
        (
            [ this ]
            ( Param* item )
            {
                if( item -> isObject())
                {
                    auto layers = item -> getObject() -> getObject
                    (
                        Path{ "config", "layers" }
                    );
                    if( layers != nullptr )
                    {
                        layers -> loop
                        (
                            [ this ]
                            ( Param* list )
                            {
                                auto layersId = list -> getObject();
                                if( layersId != nullptr )
                                {
                                    auto key =list -> getName();
                                    layersId -> loop
                                    (
                                        [ &key, this ]
                                        ( Param* item )
                                        {
                                            auto val = item -> getString();
                                            layersUsing[ val ].insert( key );
                                            return false;
                                        }
                                    );
                                }
                                return false;
                            }
                        );
                    }
                }
                return false;
            }
        );
    }
    return this;
}



std::vector<std::string> ShoggothApplication::layersByOperation
(
    /* Type operation */
    std::string aOperation
)
{
    std::lock_guard<std::mutex> lock(mLayersMutex);

    std::vector<std::string> result;

    for( const auto& [ layer, operations ] : layersUsing )
    {
        if( operations.count( aOperation ))
        {
            result.push_back( layer );
        }
    }

    return result;
}



/*
    Return true if layer must be loaded
*/
bool ShoggothApplication::layerIsUsing
(
    /* Layer id */
    std::string aId,
    /* Type of operation */
    std::string aOperation
)
{
    std::lock_guard<std::mutex> lock(mLayersMutex);

    auto it = layersUsing.find(aId);

    bool result =
    ( it != layersUsing.end()) &&
    ( aOperation.empty() || it -> second.count( aOperation ))
    ;

    return result;
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

