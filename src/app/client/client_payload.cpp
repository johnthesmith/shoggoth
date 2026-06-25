/*
    Client payload
*/

/* Vanilla libraris */
#include <iostream>

/* Shoggoth libraries */
#include "../../../../../lib/core/str.h"
#include "../../../../../lib/json/param_list.h"

/* User libraries */
#include "client_payload.h"

#include "../../shoggoth/shoggoth_rpc_client.h"

using namespace std;




/******************************************************************************
    Payload events
*/


//ПРоверить что клиент отрпавляет запрос по слоям и включает в него данные по валуесам



/*
    Main loop event
*/
void ClientPayload::onEngineLoop( bool )
{
    getLog() -> begin( "Client processing" );

    getMon()
    -> setString( Path{ "net", "id" }, net -> getId() )
    -> setString( Path{ "net", "version" }, net -> getVersion() )
    -> setInt( Path{ "net", "number" }, net -> getLearningTick() )
    ;

    /* Check server net config */
    auto netConfig = ParamList::create();
    auto connection = getConfig() -> getString( Path{ "connection" });

    ShoggothRpcClient::create( getApplication(), connection )
    -> readNet( netConfig )
    -> destroy();
 
    /* Read net config from server */
    net -> lock();
    net -> setOk();

    if
    (
        net -> isConfigUpdate( netConfig ) ||
        net -> isVersionChanged()
    )
    {
        getLog()
        -> begin( "Net config updated" )
        -> prm( "File", getApplication() -> getConfigFileName() )
        -> lineEnd();

        net -> applyConfig( getApplication() -> getConfig(), netConfig );
        getLog() -> end();
    }
    net -> unlock();
    netConfig -> destroy();

    /* Execute synchronize */
    ShoggothRpcClient::create( getApplication(), connection )
    -> netSyncLayers( oldValuesHashes, mon )
    -> resultTo( this )
    -> destroy();

    /* Dump layers hash in to mon */
    for( const auto& pair : oldValuesHashes )
    {
        getMon() -> setUInt( Path{ "hash", pair.first }, pair.second );
    }

    getMon()
    -> setString( Path{ "result", "last" }, getCode() )
    -> addInt( Path{ "result", "codes", getCode() })
    -> flush();


    getLog() -> end();
}

