#pragma once

#include <string>

using namespace std;

/* Math functionmodificator type defined */
typedef double (*NeuronFunc)( double );
typedef double (*NeuronFunc2)( double, double );
typedef double (*NeuronFunc3)( double, double, double );

/* Neural net functions */

extern NeuronFunc FUNC_NULL;
extern NeuronFunc FUNC_ZERO;
extern NeuronFunc FUNC_LINE;
extern NeuronFunc FUNC_IDENTITY;
extern NeuronFunc FUNC_STEP;
extern NeuronFunc FUNC_RELU;

extern NeuronFunc FUNC_SIGMOID;
extern NeuronFunc FUNC_SIGMOID_BACK;
extern NeuronFunc FUNC_SIGMOID_DERIVATIVE;

extern NeuronFunc FUNC_TG;
extern NeuronFunc FUNC_TG_BACK;

extern NeuronFunc2  FUNC_SIGMOID_LINE_MINUS_PLUS;
extern NeuronFunc2  FUNC_LINE_ABS_MINUS_PLUS;
extern NeuronFunc2  FUNC_V_LINE;
extern NeuronFunc2  FUNC_SIGMOID_PLUS_MINUS;
extern NeuronFunc2  FUNC_ERROR;

extern NeuronFunc3 FUNC_WEIGHT;



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
