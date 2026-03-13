#include "limb_teacher.h"



/*
    Constructor
*/
LimbTeacher::LimbTeacher
(
    /* Payload */
    Payload* aPayload,
    /* Net limb object*/
    Net* aNet
)
:Limb
(
    aNet -> getLogManager(),
    /* Empty version */
    ""
)
{
    net = aNet;
    payload = aPayload;
}



/*
    Destructor
*/
LimbTeacher::~LimbTeacher()
{
}




/*
    Create a new layer for this limb and copy parameters from source layer.
    This method is an overridden method of the Limb class.
*/
Layer* LimbTeacher::copyLayerFrom
(
    Layer* aFromLayer
)
{
    auto config = net -> getConfig();

    auto result = LayerTeacher::create( this, aFromLayer -> getId() );

    auto configLayer = config -> getObject( Path { "layers", aFromLayer -> getId() } );
    if( configLayer != NULL )
    {
        result -> setSize( configLayer );
    }
    else
    {
        getLog() -> error( "NetAndConfigIsNotConsistents" ) -> lineEnd();
    }

    return ( Layer* )result;
}



/*
    Return layer by id or NULL for not exists
*/
LayerTeacher* LimbTeacher::getLayerById
(
    string aId
)
{
    return (LayerTeacher*) ( getLayerList() -> getById( aId ));
}



/*
    Return net object
*/
Net* LimbTeacher::getNet()
{
    return net;
}
