#include "../rnd.h"
#include "../math.h"

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
    const double a
)
{
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



/*
    Calculate neuron
*/
Neuron* Neuron::calcValue
(
    bool aLoopParity /* Loop parity */
)
{
    /* Checking if a neuron value needs to be counted */
    if( loopParity != aLoopParity )
    {
        if( parentBinds -> getCount() != 0 )
        {
            /* Neuron has a binds and it is not a Receptor */
            double summ = 0;
            bool stop = false;
            parentBinds -> loop
            (
                [ &summ, &aLoopParity, &stop ]( Bind* bind ) -> bool
                {
                    /* Get a nauron */
                    Neuron* iNeuron = bind -> getParent();
                    /* Calculate summ */
                    summ += iNeuron -> getValue() * bind -> getWeight();
                    /* Check parent parity and compare with current loop parity */
                    stop = iNeuron -> loopParity != aLoopParity;
                    return stop;
                }
            );
            if( !stop )
            {
                /* Neuron is calculated */
                setValue( FUNC_SIGMOID( summ, layer -> getSensivity() ));
                loopParity = aLoopParity;
            }
        }
        else
        {
            /* It is Preceptron */
            loopParity = aLoopParity;
        }
    }
    return this;
}



Neuron* Neuron::calcError
(
    bool& aLoopParity
)
{
    /* Checking if a neuron error needs to be counted */
    if( loopParity != aLoopParity )
    {
        double summ = 0;
        bool stop = false;  /* Stop calculating */
        if( abs( getWaitingValue()) < EPSILON_D  )
        {
            /* This neuron is not result and must take the error from parents */
            parentBinds -> loop
            (
                [ &summ, &aLoopParity, &stop ]( Bind* bind ) -> bool
                {
                    /* Get a nauron */
                    Neuron* iNeuron = bind -> getParent();
                    /* Calculate summ */
                    summ += iNeuron -> getError() * bind -> getWeight();
                    /* Check parent parity and compare with current loop parity */
                    stop = iNeuron -> loopParity != aLoopParity;
                    return false;
                }
            );
        }
        else
        {
            /* This neuron have a waiting result */
            summ = getWaitingValue() - value;
        }
        setError( FUNC_SIGMOID_DERIVATIVE( summ, layer -> getSensivity() ));
    }

    return this;
}



