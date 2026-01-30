#include "func.h"

using namespace std;


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

NeuronFunc2 FUNC_SIGMOID_LINE_MINUS_PLUS =
[]( real x, real s ) -> real
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

NeuronFunc2 FUNC_V_LINE =
[]( real x, real s ) -> real
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
NeuronFunc2 FUNC_SIGMOID_PLUS_MINUS =
[]( real x, real sensivity ) -> real
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

NeuronFunc3 FUNC_WEIGHT =
[]( real x, real min, real max ) -> real
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
NeuronFunc2 FUNC_ERROR =
[]( real x, real max ) -> real
{
    return x > max ? max :
    (
        x < -max ? -max : x
    );
};

