#include "iostream"
#include "cmath"

#include "../../../../lib/core/rnd.h"

#include "func.h"

using namespace std;




/*
    Linear function
*/

NeuronFunc FUNC_LINE =
[]( double x ) -> double
{
    return x;
};



/*
    Linear learning
*/

NeuronFunc FUNC_LINE_BACK =
[]( double x ) -> double
{
    return 1;
};





/*
    Create sigmoid modifer
    https://ru.wikipedia.org/wiki/%D0%A1%D0%B8%D0%B3%D0%BC%D0%BE%D0%B8%D0%B4%D0%B0
    |
    |           Power = 10
    |
    |               ^
    |               |       ****
    |               |   ****
    |               | **
    |               |*
    |               *  0.5
    |              *|
    |            ** |
    |        ****   |
    |   *****       |
    |  -1-----------0-----------1
*/

NeuronFunc FUNC_SIGMOID =
[]( double x ) -> double
{
    return 1.0 / ( 1.0 + pow( M_E, ( -x )));
};



/*
    Create sigmoid derivative
    https://ru.wikipedia.org/wiki/%D0%A1%D0%B8%D0%B3%D0%BC%D0%BE%D0%B8%D0%B4%D0%B0
    https://kawahara.ca/how-to-compute-the-derivative-of-a-sigmoid-function-fully-worked-example/
    |                y
    |                ^
    |                |
    |                |
    |                |
    |                |
    |               *** 0.25
    |             ** | ***
    |        *****   |    ***
    |   -1-----------0-----------1
*/

NeuronFunc FUNC_SIGMOID_BACK =
[]( double x ) -> double
{
    return x * ( 1 - x );
};



/*
    Create sigmoid derivative
    https://ru.wikipedia.org/wiki/%D0%A1%D0%B8%D0%B3%D0%BC%D0%BE%D0%B8%D0%B4%D0%B0
    https://kawahara.ca/how-to-compute-the-derivative-of-a-sigmoid-function-fully-worked-example/
    |                y
    |                ^
    |                |
    |                |
    |                |
    |                |
    |               *** 0.25
    |             ** | ***
    |        *****   |    ***
    |   -1-----------0-----------1
*/

NeuronFunc FUNC_SIGMOID_DERIVATIVE =
[]( double x ) -> double
{
    auto sigmoida = FUNC_SIGMOID( x );
    return sigmoida * ( 1 - sigmoida );
};




/*
    ReLU
       ^ y                    **
       | +1.0               **
     - - - - - - - - - - -**
       |                **:
       |              **  :
       |            **    :
       |          **      :
       |        **  0.5   :
       |      **          :
       |    **            :
       |  **              :
       |**                :
    ****------------------o-> x
        0                +s
*/
NeuronFunc FUNC_RELU =
[]( double x ) -> double
{
    return x < 0.0 ? 0.0 : x;
};


/*
    ReLU back
*/
NeuronFunc FUNC_RELU_BACK =
[]( double x ) -> double
{
    return x < 0.0 ? 0.0 : 1;
};



NeuronFunc FUNC_RELU_DERIVATIVE =
[]( double x ) -> double
{
    return x < 0.0 ? 0.0 : 1;
};




/*
    Sigmoid modifer from -sensivity to +sensifity on line spline r=x

                 ^ y
                 | +1.0
     - -:- - - - o - - - -*****
        :        |      **:
        :        |    **  :
        :        |  **    :
        :        |**      :
    ****---------0----------> x
        :       **        :
        :     ** |        :
        :   **   |        :
        : **     |        :
        **       |        :
    **** - - - - o - - - - - -
        -s       |        +s
*/

function
<
    double  /* Result */
    (
        double, /* Argiment [-oo; +oo] */
        double  /* Sensiviti [ 0; +oo]*/
    )
>
FUNC_SIGMOID_LINE_MINUS_PLUS =
[]( double x, double s ) -> double
{
    return x < -s ? -1.0 : ( x > s ? 1.0 :  x / s );
};



/*
    V modifer from -sensivity to +sensifity on line spline

                 ^ y
                 | +1.0
     ****- - - - o - - - -*****
        :*       |       *:
        : *      |      * :
        :  *     |     *  :
        :   *    |    *   :
        :    *   |   *    :
        :     *  |  *     :
        :      * | *      :
        :       *|*       :
    -------------0-------------> x
        -s                +s
*/

function
<
    double  /* Result */
    (
        double, /* Argiment [-oo; +oo] */
        double  /* Sensiviti [ 0; +oo]*/
    )
>
FUNC_V_LINE =
[]( double x, double s ) -> double
{
    return abs( x < -s ? -1.0 : ( x > s ? 1.0 : x / s ));
};






/*
    Create sigmoid modifer
    https://ru.wikipedia.org/wiki/%D0%A1%D0%B8%D0%B3%D0%BC%D0%BE%D0%B8%D0%B4%D0%B0
    |
    |               ^ +1
    |   - - - - - - | - - - ****
    |               |   ****
    |               | **
    |               |*
    |  -1-----------0----------> 1
    |              *|
    |            ** |
    |        ****   | -1
    |   ***** - - - | - - - - -

*/
function
<
    double  /* Result */
    (
        double, /* Argiment [-oo; +oo] */
        double  /* Sensiviti [ 0; +oo]*/
    )
>
FUNC_SIGMOID_PLUS_MINUS =
[]( double x, double sensivity ) -> double
{
    return  2.0 / ( 1.0 + pow( M_E, ( -x ) * sensivity ) ) - 1;
};




/*
    Weight

                 ^ y
                 | +1.0
     - -:- - - - o - - - -***** +max
        :        |      **:
        :        |    **  :
        :        |  **    :     +min
        :        |  :     :
    ****---------0----------> x
        :        |  :     :
        :   **   |  :     :     -min
        : ** :   |  :     :
        **   :   |  :     :
    **** - - - - o - - - - -    -max
        -s   :   |  :      +s
             :      :
            singularity

*/

function
<
    double  /* Result */
    (
        double,     /* Argiment [-oo; +oo] */
        double,     /* Min [ 0; +oo] */
        double      /* Max [ 0; +oo] */
    )
>
FUNC_WEIGHT =
[]( double x, double min, double max ) -> double
{
    return x > max ? max :
    (
        x < -max ? -max :
        (
            x < min && x >= 0 ? - min :
            (
                x > -min && x < 0 ? +min : x
            )
        )
    );
};




/*
    Error calculation

                 ^ y
                 |
     - -:- - - - o - - - -***** +max
        :        |      **:
        :        |    **  :
        :        |  **    :
        :        |**      :
    ****---------0----------> x
        :     ** |        :
        :   **   |        :
        : **     |        :
        **       |        :
    **** - - - - o - - - - -    -max
        -s       |        +s
*/

function
<
    double  /* Result */
    (
        double,     /* Argiment [-inf; +inf] */
        double      /* Max      [ 0; +inf] */
    )
>
FUNC_ERROR =
[]( double x, double max ) -> double
{
    return x > max ? max :
    (
        x < -max ? -max : x
    );
};




void strToNeuronFunc
(
    string      aArgument,
    NeuronFunc* &aFront,
    NeuronFunc* &aBack
)
{
    if( aArgument == "LINE" )
    {
        aFront = &FUNC_LINE;
        aBack = &FUNC_LINE_BACK;
        return;
    }

    if( aArgument == "RELU" )
    {
        aFront = &FUNC_RELU;
        aBack = &FUNC_RELU_BACK;
        return;
    }

    if( aArgument == "SIGMOID" )
    {
        aFront = &FUNC_SIGMOID;
        aBack = &FUNC_SIGMOID_BACK;
        return;
    }

    /* Default */
    aFront = &FUNC_LINE;
    aBack = &FUNC_LINE_BACK;
}



string neuronFuncToStr
(
    NeuronFunc* a
)
{
    if( a == &FUNC_LINE )               return "LINE";
    if( a == &FUNC_LINE_BACK )          return "LINE_BACK";
    if( a == &FUNC_SIGMOID )            return "SIGMOID";
    if( a == &FUNC_SIGMOID_BACK )       return "SIGMOID_BACK";
    if( a == &FUNC_SIGMOID_DERIVATIVE ) return "SIGMOID_DERIVATIVE";
    if( a == &FUNC_RELU )               return "RELU";
    if( a == &FUNC_RELU_BACK )          return "RELU_BACK";
    if( a == &FUNC_RELU_DERIVATIVE )    return "RELU_DERIVATIVE";
    return "LINE";
}



