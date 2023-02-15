#include "../rnd.h"

#include "neuron.h"
#include "layer.h"
#include "func.h"
#include "../math.h"

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


Layer* Neuron::getLayer()
{
    return layer;
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



Neuron& Neuron::setError
(
    const double a,
    bool& change
)
{
    change = abs( error - a ) > EPSILON_D;
    error = a;
    return *this;
}



double Neuron::getError()
{
    return error;
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



/*
    Set waiting value for neuron in learning mode
*/
Neuron* Neuron::setWaitingValue
(
    const double a
)
{
    createExtention();
    extention -> waitingValue = a;
    return this;
}



/*
    Return a waiting value for neuron in learning mode
*/
double Neuron::getWaitingValue()
{
    return extention == NULL ? 0.0 : extention -> waitingValue;
}



Neuron* Neuron::calc()
{
    if( parentBinds -> getCount() != 0 )
    {
        /* Layer is not preceptorn */
        double summ = 0;

        parentBinds -> loop
        (
            [ &summ ]( Bind* bind ) -> bool
            {
                summ += bind -> getParent() -> getValue() * bind -> getWeight();
                return false;
            }
        );
        setValue( FUNC_SIGMOID( summ, layer -> getSensivity() ));
    }
    return this;
}



Neuron* Neuron::calcError
(
    bool& aChange
)
{
    double summ = 0;

    if( abs( getWaitingValue()) < EPSILON_D  )
    {
        /* This neuron is not reult and must take the error from parents */
        parentBinds -> loop
        (
            [ &summ ]( Bind* bind ) -> bool
            {
                summ += bind -> getChild() -> getError() * bind -> getWeight();
                return false;
            }
        );
    }
    else
    {
        /* This neuron have a waiting result */
        summ = getWaitingValue() - value;
    }
    setError( FUNC_SIGMOID_DERIVATIVE( summ, layer -> getSensivity() ), aChange );

    return this;
}



