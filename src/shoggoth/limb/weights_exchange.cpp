#include "weights_exchange.h"
#include "../../../../../lib/core/rnd.h"



/*
    Constructor
*/
WeightsExchange::WeightsExchange
()
{
    auto rnd = Rnd::create();
    clientId = rnd -> randomize() -> getUuid();
    rnd -> destroy();
}



/*
    Destructor
*/
WeightsExchange::~WeightsExchange()
{
}



/*
    Create of the object
*/
WeightsExchange* WeightsExchange::create()
{
    return new WeightsExchange();
}



/*
    Destroy of the object
*/
void WeightsExchange::destroy()
{
    delete this;
}



/*
    Find request by arguments
*/
Param* WeightsExchange::find
(
    string  aLayerId,    /* Layer id */
    int     aNeuron,     /* neuron */
    string  aClientId
)
{
    Param* result = NULL;

    if( aClientId == "" )
    {
        aClientId = clientId;
    }

    loop
    (
        [ &aLayerId, &aNeuron, &aClientId, &result ]
        ( Param* aParam )
        {
            if( aParam -> isObject())
            {
                auto object = aParam -> getObject();

                if
                (
                    object -> getString( "layerId" ) == aLayerId &&
                    object -> getInt( "neuronIndex" ) == aNeuron &&
                    object -> getString( "clientId" ) == aClientId
                )
                {
                    result = aParam;
                }
            }

            return result != NULL;
        }
    );
    return result;
}



/*
    Remove all records by client id
*/
WeightsExchange* WeightsExchange::purgeByClientId
(
    string  aClientId
)
{
    if( aClientId == "" )
    {
        aClientId = clientId;
    }

    purge
    (
        [ &aClientId]
        ( Param* object )
        {
            return object -> getObject() -> getString( "clientId" ) == aClientId;
        }
    );

    return this;
}



/**********************************************************************
    Client and server methods
*/
WeightsExchange* WeightsExchange::add
(
    string  aLayerId,    /* Layer id */
    int     aNeuron,     /* neuron */
    string  aClientId
)
{
    if( aClientId == "" )
    {
        aClientId = clientId;
    }

    auto param = find( aLayerId, aNeuron, aClientId );

    if( param == NULL  )
    {
        pushObject
        (
            ParamList::create()
            -> setString( "layerId", aLayerId )
            -> setInt( "neuronIndex", aNeuron )
            -> setString( "clientId", aClientId )
        );

        clear = false;
    }
    return this;
}



bool WeightsExchange::needRequest()
{
    return clear == false || getCount() > 0;
}



WeightsExchange* WeightsExchange::prepareRequest
(
    ParamList* aRequest
)
{
    /* WARNING !!! it will be destropy with aRequest */
    aRequest -> setString( "clientId", clientId );
    auto neurons = aRequest -> addObject( "neurons" );

    loop
    (
        [ &neurons ]
        ( Param* item )
        {
            if( item -> isObject() )
            {
                neurons
                -> addObject()
                -> setString( "layerId", item -> getObject() -> getString( "layerId" ))
                -> setInt( "neuronIndex", item -> getObject() -> getInt( "neuronIndex" ))
                ;
            }
            return false;
        }
    );

    return this;
}



WeightsExchange* WeightsExchange::synchNeuronsByClient
(
    string aClientId,
    ParamList* aNeurons
)
{
    /* Delete records from this for aClientId and is not exists in aNeurons */
    purge
    (
        [ &aNeurons, &aClientId ]
        ( Param* iParam )
        {
            return
            /* ... check clientId ... */
            iParam -> getObject() -> getString( "clientId" ) == aClientId &&
            /* ... not found ... */
            aNeurons -> findFirst
            (
                [ &iParam ]
                ( Param* jParam )
                {
                    return iParam -> getObject() -> isEqual
                    (
                        jParam -> getObject(),
                        { "layerId", "neuronIndex" }
                    );
                }
            ) == NULL;
        }
    );

    /*
        Copy values from Arguments paramlist in to this (WeightsExchange)
    */
    aNeurons -> loop
    (
        [ this, &aClientId ]
        ( Param* iParam )
        {
            if
            (
                iParam -> isObject() &&
                findFirst
                (
                    [ &iParam, &aClientId ]
                    ( Param* jParam )
                    {
                        return
                        jParam -> isObject() &&
                        jParam -> getObject() -> getString( "clientId" ) == aClientId &&
                        iParam -> getObject() -> isEqual
                        (
                            jParam -> getObject(),
                            { "layerId", "neuronIndex" }
                        );
                    }
                ) == NULL
            )
            {
                addObject()
                -> copyFrom( iParam -> getObject() )
                -> setString( "clientId", aClientId );
            }
            return false;
        }
    );
    return this;
}



/*
    Client method
*/
WeightsExchange* WeightsExchange::readAnswer
(
    ParamList* aAnswer  /* Readed answer */
)
{
    auto answerNeurons = aAnswer -> getObject( "neurons" );
    if( answerNeurons != NULL )
    {
        answerNeurons -> loop
        (
            [ this ]
            ( Param* param )
            {
                if( param -> isObject() )
                {
                    auto params = param -> getObject();
                    auto layerId = params -> getString( "layerId" );
                    auto neuronIndex = params -> getInt( "neuronIndex" );
                    auto item = find( layerId, neuronIndex );
                    if( item != NULL )
                    {
                        item -> getObject() -> addObject( "children" ) -> copyFrom( params -> getObject( "children" ));
                        item -> getObject() -> addObject( "parents" ) -> copyFrom( params -> getObject( "parents" ));
                    }
                }
                return false;
            }
        );
    }
    return this;
}



/*
    Server method
*/
WeightsExchange* WeightsExchange::prepareAnswer
(
    string aClientId,     /* Requested by client id */
    ParamList* aNeurons  /* List of neurons from clients request */
)
{
    aNeurons -> loop
    (
        [ this, &aClientId ]
        ( Param* requestNeuron )
        {
            if( requestNeuron -> isObject() )
            {
                auto requestNeuronDetails = requestNeuron -> getObject();
                auto netNeuron = find
                (
                    requestNeuronDetails -> getString( "layerId" ),
                    requestNeuronDetails -> getInt( "neuronIndex" ),
                    aClientId
                );
                if( netNeuron != NULL )
                {
                    auto n = netNeuron -> getObject();

                    requestNeuronDetails
                    -> addObject( "parents" )
                    -> copyFrom( n -> getObject( "parents" ) );

                    requestNeuronDetails
                    -> addObject( "children" )
                    -> copyFrom( n -> getObject( "children" ) );
                }
            }
            return false;
        }
    );
    return this;
}



/*
    Set clear flag to true
*/
WeightsExchange* WeightsExchange::checkClearSignal()
{
    clear = getCount() == 0;
    return this;
}
