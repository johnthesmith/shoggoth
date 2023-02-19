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
Neuron* Neuron::setLearningValue
(
    const double a
)
{
    createExtention();
    extention -> learningValue = a;
    return this;
}



/*
    Return a waiting value for neuron in learning mode
*/
double Neuron::getLearningValue()
{
    return extention == NULL ? 0.0 : extention -> learningValue;
}



/*
    Calculate neuron
*/
Neuron* Neuron::calcValue
(
    bool aLoopParity /* Loop parity */
)
{
    double summ = 0;
    bool stop = false;
    switch( getLayer() -> getLayerType())
    {
        case LT_RECEPTOR:
            /* It is Preceptron */
            /* Neuron is calculated */
            setLoopParityValue( aLoopParity );
        break;
        case LT_RESULT:
        case LT_CORTEX:
            /* Neuron has a binds and it is not a Receptor */
            parentBinds -> loop
            (
                [ &summ, &aLoopParity, &stop ]( Bind* bind ) -> bool
                {
                    /* Get a nauron */
                    Neuron* iNeuron = bind -> getParent();
                    /* Calculate summ */
                    summ += iNeuron -> getValue() * bind -> getWeight();
                    /* Check parent parity and compare with current loop parity */
                    stop = iNeuron -> getLoopParityValue() != aLoopParity;
                    return stop;
                }
            );
            if( !stop )
            {
                /* Neuron is calculated */
                setValue( FUNC_SIGMOID( summ, layer -> getSensivity() ));
                setLoopParityValue( aLoopParity );
            }
        break;
    }
    return this;
}



Neuron* Neuron::calcError
(
    bool& aLoopParity
)
{
    double summ = 0.0;
    bool stop = false;  /* Stop calculating */

    switch( getLayer() -> getLayerType())
    {
//            setError( 0.0 );
//            setLoopParityError( aLoopParity );
//        break;
        case LT_RECEPTOR:
        case LT_CORTEX:
            /* This neuron is not result and must take the error from parents */
            childrenBinds -> loop
            (
                [ &summ, &aLoopParity, &stop ]( Bind* bind ) -> bool
                {
                    /* Get a nauron */
                    Neuron* iNeuron = bind -> getChild();
                    /* Calculate summ */
                    summ += iNeuron -> getError() * bind -> getWeight();
                    /* Check parent parity and compare with current loop parity */
                    stop = iNeuron -> getLoopParityError() != aLoopParity;
                    return false;
                }
            );
            if( !stop )
            {
                /* Neuron error is calculated */
                setError( FUNC_SIGMOID_DERIVATIVE( summ, layer -> getSensivity() ));
                setLoopParityError( aLoopParity );
            }
        break;
        case LT_RESULT:
            /* This neuron have a waiting result */
            setError( getLearningValue() - value );
            setLoopParityError( aLoopParity );
        break;
    }
    return this;
}



Neuron* Neuron::learning()
{
    double aLearningRate = 1.0e-4;

    parentBinds -> loop
    (
        [ this, &aLearningRate ]( Bind* bind ) -> bool
        {
            /* Get a neuron */
            Neuron* iNeuron = bind -> getParent();
            /* Calculate summ */
            bind -> setWeight( bind -> getWeight() + /*iNeuron -> getValue() **/ getError() * aLearningRate );
            return false;
        }
    );
    return this;
}



bool Neuron::getLoopParityError()
{
    return loopParity & 0x00000010;
}



Neuron* Neuron::setLoopParityError
(
    bool a
)
{
    loopParity = a ? loopParity | 0x00000010 : loopParity & 0x11111101;
    return this;
}



bool Neuron::getLoopParityValue()
{
    return loopParity & 0x00000001;
}



Neuron* Neuron::setLoopParityValue
(
    bool a
)
{
    loopParity = a ? loopParity | 0x00000001 : loopParity & 0x11111110;
    return this;
}



