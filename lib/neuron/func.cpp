#include "func.h"
#include "cmath"
#include "../rnd.h"

using namespace std;


/*
    Create sigmoid modifer from [-00; +00]
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

function <double ( double, double )> FUNC_SIGMOID =
[]( double x, double sensivity ) -> double
{
    return 1.0 / ( 1.0 + pow( M_E, ( -x ) * sensivity ) );
};

/*
    Create sigmoid modifer from [-00; +00]
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

function <double ( double, double )> FUNC_SIGMOID_DERIVATIVE =
[]( double x, double sensivity ) -> double
{
    return FUNC_SIGMOID( x, sensivity ) * ( 1 - FUNC_SIGMOID( x, sensivity ));
};


