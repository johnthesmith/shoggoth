#pragma once

#include <functional>   /* for lyambda */

function <double (double)> FUNC_SIGMOID =
[]( double x ) -> double
{
    return 1.0 / ( 1.0 + pow( M_E, -x ) );
};


function <double (double)> FUNC_XX =
[]( double x ) -> double
{
    return x * x;
};
