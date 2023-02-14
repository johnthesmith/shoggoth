#include "func.h"
#include "cmath"
#include "../rnd.h"

using namespace std;

function <double ( double, double )> FUNC_SIGMOID =
[]( double x, double sensivity ) -> double
{
    return 1.0 / ( 1.0 + pow( M_E, ( -x ) * sensivity ) );
};
