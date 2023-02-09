#include "neuron.h"
#include "layer.h"


using namespace std;

/*
            ---->----
       [w] /         \
  [p]( P )[c]    [p]( C )[c]
        \        /
         ----<---
*/



Neuron::Neuron
(
    Layer* aLayer,  /* Layer object */
    int aLayerIndex /* Index in layer */
)
{
    layer = aLayer;
    layerIndex = aLayerIndex;
    parentBinds = new BindList();
    childrenBinds = new BindList();
}



Neuron::~Neuron()
{
    delete( childrenBinds );
    delete( parentBinds );
}



/*
    Return null neuron
*/
bool Neuron::isNull()
{
    return false;
}



/******************************************************************************
    Virtuals
*/



/*
    Return Name of neuron
*/
string Neuron::getName()
{
    return NULL;
}



/*
    Draw neuron
*/
Neuron& Neuron::draw()
{
    return *this;
}




/******************************************************************************
    Setters and getters
*/


Layer& Neuron::getLayer()
{
    return *layer;
}



int Neuron::getLayerIndex()
{
    return layerIndex;
}



Neuron& Neuron::setValue
(
    double a
)
{
    value = a;
    return *this;
}



double Neuron::getValue()
{
    return value;
}



Point3d& Neuron::getWorldPoint()
{
    return
    layer -> points -> getCount() > layerIndex
    ? layer -> points -> items[ layerIndex ]
    : POINT_3D_0;
}
