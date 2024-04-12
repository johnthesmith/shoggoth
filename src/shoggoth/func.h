#pragma once

#include <functional>   /* for lyambda */
#include <string>


using namespace std;

/* Math functionmodificator type defined */
typedef function <double ( double )> NeuronFunc;

/* Neural net functions */
extern NeuronFunc FUNC_LINE;
extern NeuronFunc FUNC_LINE_BACK;
extern NeuronFunc FUNC_SIGMOID;
extern NeuronFunc FUNC_SIGMOID_BACK;
extern NeuronFunc FUNC_SIGMOID_DERIVATIVE;
extern NeuronFunc FUNC_RELU;
extern NeuronFunc FUNC_RELU_BACK;
extern NeuronFunc FUNC_RELU_DERIVATIVE;
extern NeuronFunc FUNC_TG;
extern NeuronFunc FUNC_TG_BACK;

extern function <double ( double, double )> FUNC_SIGMOID_LINE_MINUS_PLUS;
extern function <double ( double, double )> FUNC_LINE_ABS_MINUS_PLUS;
extern function <double ( double, double )> FUNC_V_LINE;
extern function <double ( double, double )> FUNC_SIGMOID_PLUS_MINUS;
extern function <double ( double, double, double )> FUNC_WEIGHT;
extern function <double ( double, double )> FUNC_ERROR;

/*
    Function names
    "LINE"
    "LINE_BACK"
    "SIGMOID"
    "SIGMOID_BACK"
    "SIGMOID_DERIVATIVE"
    "RELU"
    "RELU_BACK"
    "RELU_DERIVATIVE"
*/

void strToNeuronFunc
(
    string,
    NeuronFunc*&,
    NeuronFunc*&
);



string neuronFuncToStr
(
    NeuronFunc*
);
