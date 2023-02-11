#include "net.h"



/*
    Constructor
*/
Net::Net
(
    Log* a    /* Log object */
)
{
    log = a;
    log -> trace( "Create net" );

    layers = new LayerList();
}




/*
    Constructor
*/
Net::~Net()
{
    clear();

    delete( layers );

    log -> trace( "Destroy net" );
}



/*
    Static constructor
*/
Net* Net::create
(
    Log* a
)
{
    return new Net( a );
}



/*
    Destructor
*/
void Net::destroy()
{
    delete ( this );
}



/*
    Return layer by Id
*/
Layer* Net::getLayerById
(
    string a /* Id of layer */
)
{
    Layer* result = NULL;
    int c = layers -> getCount();
    for( int i=0; i < c && result == NULL; i++ )
    {
        Layer* iLayer = layers -> getByIndex( i );
        if( iLayer -> getId() == a )
        {
            result = iLayer;
        }
    }
    return result;
}



/*
    Create new layer
*/
Layer* Net::createLayer
(
    string a /* Name of layer */
)
{
    Layer* result = Layer::create( *log ) -> setName( a );
    layers -> push( result );
    return result;
}



/*
    Delete layer by Id
*/
Net* Net::deleteLayer
(
    string a /* Id of layer */
)
{
    int layerIndex = layers -> getIndexById( a );
    Layer* layerRemoved = ( Layer* ) layers -> remove( layerIndex );
    layerRemoved -> destroy();

    return this;
}



/*
    Clear all layers
*/
Net* Net::clear()
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> destroy();
    }
    layers -> resize( 0 );
    return this;
}



/*
    Calculate all enabled layers
*/
Net* Net::calc
(
    Scene* aScene
)
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> calc( aScene );
    }
    return this;
}



/*
    Calculate all enabled layers
*/
Net* Net::draw
(
    Scene* aScene,
    bool calcScreenPos
)
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> draw( aScene, calcScreenPos );
    }
    return this;
}






/*
    Return log object
*/
Log* Net::getLog()
{
    return log;
}



/*
    Return list of neurons in screen rect
*/
Net* Net::getNeuronsByScreenRect
(
    NeuronList* aList,
    Point3d& aTopLeft,      /* Top left point */
    Point3d& aBottomRight   /* Bottom right point */
)
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> getNeuronsByScreenRect( aList, aTopLeft, aBottomRight );
    }
    return this;
}
