#include "neuron_ui.h"
#include "layer_ui.h"



using namespace std;



/*
    Constructor
*/
NeuronUi::NeuronUi
(
    LayerUi* aLayer,
    int aIndex
)
{
    layer = aLayer;
    index = aIndex;
}




/*
    Create the Layer list object
*/
NeuronUi* NeuronUi::create
(
    LayerUi* aLayer,
    int aIndex
)
{
    return new NeuronUi( aLayer, aIndex );
}



/*
    Destroy the Layer list object
*/
void NeuronUi::destroy()
{
    delete this;
}



/*
    Return index
*/
int NeuronUi::getIndex()
{
    return index;
}



/*
    Return Layer
*/
LayerUi* NeuronUi::getLayer()
{
    return layer;
}



/*
    Return neuron screen position
*/
Point3d NeuronUi::getScreen()
{
    return layer -> getNeuronScreen( index );
}




/*
    Return neuron world position
*/
Point3d NeuronUi::getWorld()
{
    return layer -> getNeuronWorld( index );
}




/*
    Set neuron value
*/
NeuronUi* NeuronUi::setValue
(
    double aValue
)
{
    layer -> setNeuronValue( index, aValue );
    return this;
}



/*
    Return neuron value
*/
double NeuronUi::getValue()
{
    return layer -> getNeuronValue( index );
}



/*
    Set neuron error
*/
NeuronUi* NeuronUi::setError
(
    double aValue
)
{
    layer -> setNeuronError( index, aValue );
    return this;
}



/*
    Return neuron error
*/
double NeuronUi::getError()
{
    return layer -> getNeuronError( index );
}



/*
    Set neuron world position
*/
NeuronUi* NeuronUi::setWorld
(
    Point3d& aValue
)
{
    layer -> setNeuronWorld( index, aValue );
    return this;
}



/*
    Set neuron screen position
*/
NeuronUi* NeuronUi::setScreen
(
    Point3d& aValue
)
{
    layer -> setNeuronScreen( index, aValue );
    return this;
}
