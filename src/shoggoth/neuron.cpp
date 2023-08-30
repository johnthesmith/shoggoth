#include "../lib/rnd.h"
#include "../lib/math.h"

#include "neuron.h"
#include "nerve.h"
#include "layer.h"
#include "func.h"
#include "net.h"

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
    Layer*  aLayer, /* Layer object */
    int     aIndex  /* Neurons indes in lmain layer */
)
{
    layer = aLayer;
    index = aIndex;
    layer -> getNeurons() -> setByIndex( aIndex, this );
}



Neuron::~Neuron()
{
    if( extention != NULL )
    {
        destroyExtention();
    }
}



/*
    Create the neuron
*/
Neuron* Neuron::create
(
    Layer* aLayer, /* Main neurons layer object */
    int    aIndex /* Neuron index in main layer */
)
{
    auto result = new Neuron( aLayer, aIndex );
    return result;
}



/*
    Self destroy the neuron
*/
void Neuron::destroy()
{
    delete this;
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
Neuron* Neuron::calcValue()
{
    double summValue        = 0;
    double summCommand      = 0;
    double summSample       = 0;
    int countSample         = 0;
    int countValue          = 0;

    bool stop = false;

    /* Neuron has a binds and it is not a Receptor */
    parentsLoop
    (
        [
            this,
            &summValue,
            &summSample,
            &summCommand,
            &stop,
            &countSample,
            &countValue
        ]
        ( Neuron* aParent, Nerve* aNerve, double aWeight ) -> bool
        {
            /* Calculate summ */
            switch( aNerve -> getBindType())
            {
                case BT_VALUE:
                    summValue += aParent -> getValue() * aWeight;
                    countValue ++;
                break;
                case BT_ERROR_TO_VALUE:
                    summValue += abs( aParent -> getError() * aWeight );
                    countValue++;
                break;
                case BT_COMMAND:
                    summCommand += aParent -> getValue() * aWeight;
                break;
                case BT_SAMPLE:
                    summSample += aParent -> getValue() * aWeight;
                    countSample ++;
                break;
            }
            return false;
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
                    ( summSample - getValue() ) * ( summCommand > EPSILON_D ? 1.0 : 0.0 ),
                    1.0
                )
            );
        }
    }

    return this;
}



/*
    w - вес связи между родительским нейроном C и дочерним P [ -1; +1 ]
    Vc - заряд дочернего нейрона [ 0; +1 ]
    Vp - заряд родительского нейрона [ 0; +1 ] (устанавливается предыдущим циклом)
    e - эпсилон - маленькое значение для роста нулевых связей
    Ec - ошибка дочернего нейрона [ -1; +1 ] (устанавливается предыдущим циклом)
    Ep - ошибка родительского нейрона [ -1; +1 ]
    s - скорость обучения = 0.001

    Vc = sig( sum( w * Vp ), [0; +1] )
        Сумма всех сигналов от родителей через связи.
        Сигмоида ограничивает результат в пределах от 0 до 1.

    w = sig( w + ( Vp * ( abc( w ) + e ) * Ec ) * s )
        При наличии ошибки Ес необходимо скоректировать поступающий на С сигнал
        Коррекция прозиводится изменением веса связи w.
        Для коррекции используется
            Vp - так как надо учитывать уровень синала родителя который влияет на Vc
            w - так как от связи зависит проходящий сигнал
                если связь < e то необходимо дать ей возможность минимально изменится
            Ec - надо учитывать ошибку ребенка. если шибки нет то и кореция не нужна.
*/



Neuron* Neuron::learning
(
    double aErrorNormalize,
    double aLearningSpeed,
    double aWakeupWeight
)
{
//    double summWeight   = 0.0;
//    double summError    = 0.0;
//    int countValue      = 0;
//    bool stop           = false;  /* Stop calculating */
//
//    /* This neuron is not result and must take the error from parents */
//    childrenBinds -> loop
//    (
//        [
//            this,
//            &summWeight,
//            &summError,
//            &countValue,
//            &aLoopParity,
//            &stop
//        ]
//        ( Bind* bind ) -> bool
//        {
//            /* Get a nauron */
//            Neuron* iNeuron = bind -> getChild();
//            /* Calculate summ */
//            switch( bind -> getType())
//            {
//                case BT_VALUE:
//                    summError += iNeuron -> getError() * bind -> getWeight();
//                    summWeight += abs( bind -> getWeight() );
//                    countValue ++;
//                break;
//            }
//            /* Check parent parity and compare with current loop parity */
//            stop = iNeuron -> getLoopParityError() != aLoopParity;
//            return stop;
//        }
//    );
//
//    if( !stop )
//    {
//        /* Neuron error is calculated */
//        if( countValue > 0 && summWeight > EPSILON_D )
//        {
//            setError
//            (
//                FUNC_SIGMOID_LINE_MINUS_PLUS
//                (
//                    summError / ( 1 + summWeight * aErrorNormalize ),
//                    1.0
//                )
//            );
//        }
//        setLoopParityError( aLoopParity );
//
//
//        /* Learning */
//        parentBinds -> loop
//        (
//            [ this, aLearningSpeed, aWakeupWeight ]( Bind* bind ) -> bool
//            {
//                /* Get a neuron */
//                Neuron* parentNeuron = bind -> getParent();
//
//                if( bind -> getType() == BT_VALUE )
//                {
//                    /*
//                        Calculate delta
//                    */
//                    double s = aLearningSpeed;  /* Learning speed */
//                    double e = aWakeupWeight;   /* epsilon for zero weight подъем нулевых связей */
//                    double w = abs( bind -> getWeight());
//                    double wv = ( w < e ? e : w ) * parentNeuron -> getValue();
//                    double deltaWeight = error * wv;
//
//                    bind -> setWeight
//                    (
//                        FUNC_SIGMOID_LINE_MINUS_PLUS
//                        (
//                            bind -> getWeight() + deltaWeight * s,
//                            1.0
//                        )
//                    );
//                }
//                return false;
//            }
//        );
//    }

    return this;
}



/*
    Loop for each parents of this neuron
*/
Neuron* Neuron::parentsLoop
(
    function <bool ( Neuron*, Nerve*, double )> callback
)
{
    /* Loop by nerves */
    layer -> getNet() -> getNerves() -> loop
    (
        [ this, &callback ]
        ( void* aNerve )
        {
            auto iNerve = ( Nerve* ) aNerve;
            if( iNerve -> getChild() == layer )
            {
                int from = 0;
                int to = 0;
                iNerve -> getWeightsRangeByChildIndex( index, from, to );
                /* Loop by weights */
                for( int i = from; i < to;  i++ )
                {
                    auto parent = iNerve -> getParentByWeightIndex( i );
                    callback( parent, iNerve, iNerve -> getWeight( i ) );
                }
            }
            return false;
        }
    );
    return this;
}

