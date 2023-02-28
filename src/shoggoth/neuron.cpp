#include "../lib/rnd.h"
#include "../lib/math.h"

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
    value = FUNC_SIGMOID_LINE_ZERO_PLUS( a, 1.0 );
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
    Calculate neuron
*/
Neuron* Neuron::calcValue
(
    bool aLoopParity /* Loop parity */
)
{
    double summValue = 0;
    double summCommand = 0;
    double summSample = 0;
    int countSample = 0;
    int countValue = 0;

    bool stop = false;

    /* Neuron has a binds and it is not a Receptor */
    parentBinds -> loop
    (
        [
            this,
            &summValue,
            &summSample,
            &summCommand,
            &aLoopParity,
            &stop,
            &countSample,
            &countValue
        ]
        ( Bind* bind ) -> bool
        {
            /* Get a nauron */
            Neuron* iNeuron = bind -> getParent();

            /* Calculate summ */
            switch( bind -> getType())
            {
                case BT_VALUE:
                    summValue += iNeuron -> getValue() * bind -> getWeight();
                    countValue ++;
                break;
                case BT_COMMAND:
                    summCommand += iNeuron -> getValue() * bind -> getWeight();
                break;
                case BT_SAMPLE:
                    summSample += iNeuron -> getValue() * bind -> getWeight();
                    countSample ++;
                break;
            }

            /* Check parent parity and compare with current loop parity */
            stop = iNeuron -> getLoopParityValue() != aLoopParity;
            return stop;
        }
    );

    if( !stop )
    {
        if( countValue > 0 )
        {
            setValue( FUNC_SIGMOID_LINE_ZERO_PLUS( summValue, 1.0 ));
        }
        if( countSample > 0 )
        {
            setError
            (
                FUNC_SIGMOID_LINE_MINUS_PLUS
                (
                    ( summSample - getValue() ) * ( summCommand > EPSILON_D ? 1 : 0 ),
                    1.0
                )
            );
        }
        /* Neuron is calculated */
        setLoopParityValue( aLoopParity );
    }

    return this;
}



Neuron* Neuron::calcError
(
    bool& aLoopParity
)
{
    double summValue = 0.0;
    int countValue = 0;
    bool stop = false;  /* Stop calculating */

    /* This neuron is not result and must take the error from parents */
    childrenBinds -> loop
    (
        [ &summValue, &countValue, &aLoopParity, &stop ]( Bind* bind ) -> bool
        {
            /* Get a nauron */
            Neuron* iNeuron = bind -> getChild();
            /* Calculate summ */
            switch( bind -> getType())
            {
                case BT_VALUE:
                    summValue += iNeuron -> getError() * bind -> getWeight();
                    countValue ++;
                break;
            }
            /* Check parent parity and compare with current loop parity */
            stop = iNeuron -> getLoopParityError() != aLoopParity;
            return false;
        }
    );

    if( !stop )
    {
        /* Neuron error is calculated */
        if( countValue > 0 )
        {
            setError( FUNC_SIGMOID_LINE_MINUS_PLUS( summValue, 1.0 ));
        }
        setLoopParityError( aLoopParity );
    }

    return this;
}



Neuron* Neuron::learning()
{
    double aLearningRate = 1.0e-2;

    parentBinds -> loop
    (
        [ this, &aLearningRate ]( Bind* bind ) -> bool
        {
            /* Get a neuron */
            Neuron* parentNeuron = bind -> getParent();

            if( bind -> getType() == BT_VALUE )
            {
                /* Calculate delta */
                auto wv = bind -> getWeight() * parentNeuron -> getValue();
                auto deltaWeight = error * ( 0.01 + 0.99 * abs( wv ) );
                bind -> setWeight
                (
                    FUNC_SIGMOID_LINE_MINUS_PLUS( bind -> getWeight() + deltaWeight * 0.001, 1.0 )
                );
            }

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



