#include "limb_teacher.h"
#include "../../../../../lib/core/payload.h"


/*
    Constructor
*/
LimbTeacher::LimbTeacher
(
    /* Payload */
    Payload* aPayload
)
:Limb
(
   aPayload -> getApplication() -> getLogManager(),
   aPayload -> getId(),
   ""
)
{
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
    auto config = payload -> getConfig();

    auto result = LayerTeacher::create( this, aFromLayer -> getId() );

    auto configLayer = config -> getObject( Path { "layers", aFromLayer -> getId() } );
    if( configLayer != nullptr )
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
