#include "func.h"

function <double (double)> FUNC_SIGMOID =
[]( double x ) -> double
{
    return 1.0 / ( 1.0 + pow( M_E, -x ) );
};
