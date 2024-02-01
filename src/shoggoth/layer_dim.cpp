#include "layer_dim.h"


/*
    Constructor
*/
LayerDim::LayerDim
(
    Limb* aLimb,
    string aId
)
:Layer( aLimb, aId )
{
}



/*
    Set dimations size
*/
LayerDim* LayerDim::setSize
(
    const Point3i& a
)
{
    setCount( a.x * a.y * a.z );
    size = a;

    return this;
}




/*
    Set dimentions size from ParamList object
*/
LayerDim* LayerDim::setSize
(
    ParamList* a
)
{
    auto jsonSize = a -> getObject( "size" );
    if( jsonSize != NULL )
    {
        setSize
        (
            Point3i
            (
                jsonSize -> getInt( 0 ),
                jsonSize -> getInt( 1 ),
                jsonSize -> getInt( 2 )
            )
        );
    }
    return this;
}



/*
    Return 3d size of layer
*/
Point3i LayerDim::getSize()
{
    return size;
}



/*
    Return linear index of neuron by position
*/
int LayerDim::indexByPos
(
    const Point3i& a
)
{
    auto s = getSize();
    return a.x + a.y * s.x + a.z * s.x * s.y;
}

