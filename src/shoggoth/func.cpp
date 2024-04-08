#include "cmath"

#include "../../../../lib/core/rnd.h"

#include "func.h"

using namespace std;


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

function
<
    double  /* Result */
    (
        double, /* Argiment [-oo; +oo] */
        double  /* Sensiviti [ 0; +oo]*/
    )
>
FUNC_SIGMOID =
[]( double x, double sensivity ) -> double
{
    return 1.0 / ( 1.0 + pow( M_E, ( -x + 0.5 ) * sensivity ) );
};



/*
    Linear sigmoid
       ^ y
       | +1.0
     - - - - - - - - - - -*****
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
function
<
    double  /* Result */
    (
        double, /* Argiment [-oo; +oo] */
        double  /* Sensiviti [ 0; +oo]*/
    )
>
FUNC_SIGMOID_LINE_ZERO_PLUS =
[]( double x, double s ) -> double
{
    return x < 0.0 ? 0.0 : ( x > s ? 1.0 : x / s );
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
function
<
    double  /* Result */
    (
        double /* Argiment [-oo; +oo] */
    )
>
FUNC_RELU =
[]( double x ) -> double
{
    return x < 0.0 ? 0.0 : x;
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

function
<
    double
    (
        double, /* Argiment [-oo; +oo] */
        double  /* Sensiviti [ 0; +oo]*/
    )
>
FUNC_SIGMOID_DERIVATIVE =
[]( double x, double sensivity ) -> double
{
    auto sigmoida = FUNC_SIGMOID( x, sensivity );
    return sigmoida * ( 1 - sigmoida );
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
