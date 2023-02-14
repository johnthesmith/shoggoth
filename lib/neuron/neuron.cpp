#include "../rnd.h"

#include "neuron.h"
#include "layer.h"
#include "func.h"

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
    Layer* aLayer   /* Layer object */
)
{
    layer = aLayer;
    parentBinds = new BindList();
    childrenBinds = new BindList();
}



Neuron::~Neuron()
{
    if( extention != NULL )
    {
        destroyExtention();
    }
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
    return extention == NULL ? POINT_3D_0 : extention -> world;
}



Point3d& Neuron::getScreenPoint()
{
    return extention == NULL ? POINT_3D_0 : extention -> screen;
}



/*
    Create extention for neuron
*/
Neuron* Neuron::createExtention()
{
    if( extention == NULL )
    {
        extention = new NeuronExtention();
    }
    return this;
}



/*
    Destroy extention for neuron
*/
Neuron* Neuron::destroyExtention()
{
    delete extention;
    extention = NULL;
    return this;
}



/*
    Set world point
*/
Neuron* Neuron::setWorldPoint
(
    Point3d& a
)
{
    createExtention();
    extention -> world = a;
    return this;
}




/*
    Set screen point
*/
Neuron* Neuron::setScreenPoint
(
    Point3d& a
)
{
    createExtention();
    extention -> screen = a;
    return this;
}



Neuron* Neuron::calc()
{
    if( parentBinds -> getCount() != 0 )
    {
        double summ = 0;

        parentBinds -> loop
        (
            [ &summ ]( Bind* bind ) -> bool
            {
                summ += bind -> getParent() -> getValue() * bind -> getWeight();
                return false;
            }
        );
        setValue( FUNC_SIGMOID( summ ));
    }
    return this;
}



