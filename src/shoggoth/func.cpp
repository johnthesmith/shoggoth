#include "cmath"

#include "../lib/rnd.h"

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
    Randomizer
*/
function
<
    double()
>
FUNC_RND =
[]() -> double
{
    return Rnd::get( 0.0, 1.0 );
};



