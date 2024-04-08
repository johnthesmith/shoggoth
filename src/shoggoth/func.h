#pragma once

#include <functional>   /* for lyambda */

using namespace std;

extern function <double ( double, double )> FUNC_SIGMOID;
extern function <double ( double )> FUNC_RELU;
extern function <double ( double, double )> FUNC_SIGMOID_LINE_ZERO_PLUS;
extern function <double ( double, double )> FUNC_SIGMOID_LINE_MINUS_PLUS;
extern function <double ( double, double )> FUNC_LINE_ABS_MINUS_PLUS;
extern function <double ( double, double )> FUNC_V_LINE;

extern function <double ( double, double )> FUNC_SIGMOID_PLUS_MINUS;
extern function <double ( double, double )> FUNC_SIGMOID_DERIVATIVE;

extern function <double ( double, double, double )> FUNC_WEIGHT;
extern function <double ( double, double )> FUNC_ERROR;
