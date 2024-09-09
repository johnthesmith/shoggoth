#pragma once

#include <functional>   /* for lyambda */
#include <string>

using namespace std;

/* Math functionmodificator type defined */
typedef function <double ( double )> NeuronFunc;

/* Neural net functions */

extern NeuronFunc FUNC_NULL;
extern NeuronFunc FUNC_LINE;
extern NeuronFunc FUNC_IDENTITY;
extern NeuronFunc FUNC_STEP;
extern NeuronFunc FUNC_RELU;

extern NeuronFunc FUNC_SIGMOID;
extern NeuronFunc FUNC_SIGMOID_BACK;
extern NeuronFunc FUNC_SIGMOID_DERIVATIVE;

extern NeuronFunc FUNC_TG;
extern NeuronFunc FUNC_TG_BACK;

extern function <double ( double, double )>         FUNC_SIGMOID_LINE_MINUS_PLUS;
extern function <double ( double, double )>         FUNC_LINE_ABS_MINUS_PLUS;
extern function <double ( double, double )>         FUNC_V_LINE;
extern function <double ( double, double )>         FUNC_SIGMOID_PLUS_MINUS;
extern function <double ( double, double, double )> FUNC_WEIGHT;
extern function <double ( double, double )>         FUNC_ERROR;



/*
    Convert string to function
*/
NeuronFunc* strToFunc
(
    string
);



string neuronFuncToStr
(
    NeuronFunc*
);
