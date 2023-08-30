#include "sync.h"

/*
    Constructor
*/
Sync::Sync
(
    Log*        aLog       /* Log object*/
)
{
    /* Set properties */
    log         = aLog;
}



/*
    Destructor
*/
Sync::~Sync()
{
}



/*
    Create
*/
Sync* Sync::create
(
    Log*        aLog       /* Log object*/
)
{
    return new Sync( aLog );
}



/*
    Destroy
*/
void Sync::destroy()
{
    delete this;
}



/*
    Return log object
*/
Log* Sync::getLog()
{
    return log;
}



/*
    Fill object from net config
*/
Sync* Sync::fill
(
    ParamList*  a /* Source ParamList from Net.net.layers */
)
{
    clear();
    if( a == NULL )
    {
        setCode( "LayersNotFound" );
    }
    else
    {
        a -> loop
        (
            [ this ]
            ( Param* item )
            {
                auto layer = item -> getObject();
                if( layer != NULL )
                {
                    auto clients = layer -> getObject( "processors" );
                    clients -> loop
                    (
                        [ this, &item ]
                        ( Param* aProcessor )
                        {
                            /* Create hash pair */
                            getByClientAndLayer
                            (
                                aProcessor -> getString(),
                                item -> getName()
                            );
                            return false;
                        }
                    );
                }
                return false;
            }
        );
    }
    return this;
}



/*
    Return sync by client
*/
Sync* Sync::set
(
    ParamList*  a /* Source ParamList */
)
{
    clear();
    copyFrom( a );
    return this;
}




/*
    Return sync by client
*/
Sync* Sync::getByClient
(
    string      aIdClient,       /* Client ID */
    ParamList*  aDest            /* Destination ParamList */
)
{
    /* Clear the paramlist result */
    aDest -> clear();

    loop
    (
        [ &aIdClient, &aDest ]
        (
            Param* aObject
        )
        {
            auto item = aObject -> getObject();
            if( item != NULL && item -> getString( "client" ) == aIdClient )
            {
                aDest -> pushObject
                (
                    ParamList::create()
                    -> copyFrom( item )
                );
            }
            return false;
        }
    );

    return this;
}




/*
    return true if net is complete for forward and backward flags
*/
bool Sync::isComplete
(
    string aId
)
{
    auto result = getCount() != 0;

    loop
    (
        [ &result, &aId ]
        (
            Param* aObject
        )
        {
            auto item = aObject -> getObject();
            if
            (
                item != NULL &&
                ( aId == ""  || item -> getString( "layer" ) == aId )
            )
            {
                result = result &&
                item -> getBool( "forward" ) &&
                item -> getBool( "backward" );
            }
            return false;
        }
    );

    return result;
}



/*
    return true if net is complete for forward and backward flags
*/
bool Sync::isForward
(
    string aId
)
{
    auto result = getCount() != 0;

    loop
    (
        [ &result, &aId ]
        (
            Param* aObject
        )
        {
            auto item = aObject -> getObject();
            if
            (
                item != NULL &&
                ( aId == ""  || item -> getString( "layer" ) == aId )
            )
            {
                result = result &&
                item -> getBool( "forward" );
            }
            return false;
        }
    );

    return result;
}



/*
    return true if layer or all layers is backward true
*/
bool Sync::isBackward
(
    string aId
)
{
    auto result = getCount() != 0;

    loop
    (
        [ &result, &aId ]
        (
            Param* aObject
        )
        {
            auto item = aObject -> getObject();
            if
            (
                item != NULL &&
                ( aId == ""  || item -> getString( "layer" ) == aId )
            )
            {
                result = result &&
                item -> getBool( "backward" );
            }
            return false;
        }
    );

    return result;
}



/*
    Set forward flag
*/
Sync* Sync::setForward
(
    string aIdClient,
    string aIdLayer
)
{
    getByClientAndLayer( aIdClient, aIdLayer )
    -> setBool( "forward", true );
    return this;
}



/*
    Return forward flag
*/
bool Sync::getForward
(
    string aIdClient,
    string aIdLayer
)
{
    return
    getByClientAndLayer( aIdClient, aIdLayer )
    -> getBool( "forward", true );
}



/*
    Set backward flag
*/
Sync* Sync::setBackward
(
    string aIdClient,
    string aIdLayer
)
{
    getByClientAndLayer( aIdClient, aIdLayer )
    -> setBool( "backward", true );
    return this;
}



/*
    Return backward flag
*/
bool Sync::getBackward
(
    string aIdClient,
    string aIdLayer
)
{
    return
    getByClientAndLayer( aIdClient, aIdLayer )
    -> getBool( "backward", true );
}



/*
    Private declaraion
*/


/*
    Return paramlist by layer and client or create new element
*/
ParamList* Sync::getByClientAndLayer
(
    string aIdClient,
    string aIdLayer
)
{
    ParamList* result = NULL;

    loop
    (
        [ &result, &aIdClient, &aIdLayer ]
        (
            Param* aObject
        )
        {
            auto item = aObject -> getObject();
            if
            (
                item != NULL &&
                item -> getString( "client" ) == aIdClient &&
                item -> getString( "layer" ) == aIdLayer
            )
            {
                result = item;
                return true;
            }
            return false;
        }
    );

    /* Create new pair if not exists */
    if( result == NULL )
    {
        result = ParamList::create()
        -> setString( "client", aIdClient)
        -> setString( "layer", aIdLayer)
        -> setBool( "forward", false )
        -> setBool( "backward", false );

        pushObject( result );
    }

    return result;
}



/*
    Return all clients id
*/
Sync* Sync::collectClientsAndLayers
(
    ParamList* aClients,
    ParamList* aLayers
)
{
    loop
    (
        [ &aClients, &aLayers ]( Param* aObject )
        {
            auto item = aObject -> getObject();
            if( item != NULL )
            {
                aClients -> setBool( item -> getString( "client" ), true );
                aLayers -> setBool( item -> getString( "layer" ), true );
            }
            return false;
        }
    );
    return this;
}



Sync* Sync::toLog
(
    int aLayerIndex
)
{
    getLog() -> begin( "Sync dump" );

    /* Get clients */
    auto clients = ParamList::create();
    auto layers = ParamList::create();
    collectClientsAndLayers( clients, layers );

    /* Dump header */
    getLog()
    -> trace()
    -> setWidth( 4 )
    -> text( "Cur" )
    -> setWidth( 20 )
    -> text( "Layer" )
    -> setWidth( 20 );

    int v = clients -> getCount();
    for( int j = 0; j < v; j++ )
    {
        auto client = clients -> getByIndex( j );
        getLog()
        -> text( client -> getName() );
    }
    getLog() -> setWidth( 0 );


    int c = layers -> getCount();
    for( int i=0; i<c; i++ )
    {
        auto layer = layers -> getByIndex( i );

        getLog()
        -> trace()
        -> setWidth( 4 )
        -> text( i == aLayerIndex ? ">" : " " )
        -> setWidth( 20 )
        -> text( layer -> getName() )
        -> setWidth( 20 );

        for( int j=0; j<v; j++ )
        {
            auto client = clients -> getByIndex( j );

            string f = getForward( client -> getName(), layer -> getName() ) ? "F" : "-";
            string b = getBackward( client -> getName(), layer -> getName() ) ? "B" : "-";

            getLog() -> text( f + " " + b);
        }
    }

    clients -> destroy();
    layers -> destroy();

    getLog() -> setWidth( 0 ) -> end();

    return this;
}
