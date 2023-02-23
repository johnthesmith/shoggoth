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
    Layer* result = Layer::create( *log ) -> setName( a ) -> setSensivity( sensivity );
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

    /* Calculate values */
    bool valueFinish = true;
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        if( layer -> getLoopParityValue() != loopParity )
        {
            layer -> calcValue( loopParity );
            valueFinish = valueFinish && layer -> getLoopParityValue() == loopParity;
        }
    }

    if( valueFinish )
    {
        /* Calculate errors */
        bool errorFinish = true;
        for( int i = c - 1; i >= 0; i-- )
        {
            Layer* layer = ( Layer* ) layers -> getByIndex( i );
            if( layer -> getLoopParityError() != loopParity )
            {
                layer -> calcError( loopParity );
                errorFinish = errorFinish && layer -> getLoopParityError() == loopParity;
            }
        }

        if( learningMode )
        {
            /* Learning */
            if( errorFinish )
            {
                for( int i = 0; i < c; i++ )
                {
                    Layer* layer = ( Layer* ) layers -> getByIndex( i );
                    layer -> learning();
                }
                loopParity = !loopParity;
            }
        }
        else
        {
            loopParity = !loopParity;
        }
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
    Return neuron by screen position
*/
Net* Net::getNeuronsByScreenPos
(
    NeuronList* aList,
    const Point3d& aPosition
)
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> getNeuronsByScreenPos( aList, aPosition );
    }
    return this;
}



Net* Net::setSelected
(
    Neuron* a
)
{
    selected = a;
    return this;
}



Neuron* Net::getSelected()
{
    return selected;
}




/*
    On mouse left click event
*/
Net* Net::setSelected
(
    Scene& aScene /* Scene object */
)
{
    auto neurons = NeuronList::create();
    getNeuronsByScreenPos( neurons, aScene.getMouseCurrentScreen() );

    if( neurons -> getCount() > 0 )
    {
        setSelected( neurons -> getByIndex( 0 ));
    }
    else
    {
        setSelected( NULL );
    }

    neurons -> destroy();

    return this;
}



Net* Net::switchShowBinds()
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> switchShowBinds();
    }
    return this;
}



Net* Net::switchShowLayer()
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> switchShowLayer();
    }
    return this;
}



/*
    Set learning mode
*/
Net* Net::setLearningMode
(
    bool a /* Value */
)
{
    learningMode = a;
    return this;
}



/*
    Get learning mode
*/
bool Net::getLearningMode()
{
    return learningMode;
}



/*
    Switch learning mode true/false
*/
Net* Net::switchLearningMode()
{
    learningMode = !learningMode;
    return this;
}


