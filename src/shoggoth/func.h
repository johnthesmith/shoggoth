#pragma once

#include <functional>   /* for lyambda */

using namespace std;

extern function <double ( double, double )> FUNC_SIGMOID;
extern function <double ( double, double )> FUNC_SIGMOID_PLUS_MINUS;
extern function <double ( double, double )> FUNC_SIGMOID_DERIVATIVE;
