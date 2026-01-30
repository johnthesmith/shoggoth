#pragma once

#include <string>
#include "cmath"
#include "../../../../lib/core/real.cpp"
#include "../../../../lib/core/math.h"


using namespace std;

/* Math functionmodificator type defined */
typedef real (*NeuronFunc)( real );
typedef real (*NeuronFunc2)( real, real );
typedef real (*NeuronFunc3)( real, real, real );

/* Neural net functions */

 /*
     Null function
 */

inline NeuronFunc FUNC_NULL =
[]( real x ) -> real
{
    return x;
};



/*
    Zero function
*/
inline NeuronFunc FUNC_ZERO = []( real x ) -> real
{
    return 0;
};



/*
    Alwayse return one
    Heaviside step Function
*/

inline NeuronFunc FUNC_ONE = []( real x ) -> real
{
    return 1;
};



/*
    Linear function
*/
inline NeuronFunc FUNC_LINE = []( real x ) -> real
{
    return x;
};


extern NeuronFunc FUNC_IDENTITY;



/*
    Heaviside step Function
*/

inline NeuronFunc FUNC_STEP = []( real x ) -> real
{
    return x < EPSILON_D ? 0.0 : 1.0;
};



/*
    ReLU
       ^ y                    **
       | +1.0               **
     - - - - - - - - - - -**
       |                **:
       |              **  :
       |            **    :
       |          **      :
       |        **  0.5   :
       |      **          :
       |    **            :
       |  **              :
       |**                :
    ****------------------o-> x
        0                +s
*/
inline NeuronFunc FUNC_RELU =
[]( real x ) -> real
{
    return x < 0.0 ? 0.0 : x;
};



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

inline NeuronFunc FUNC_SIGMOID =
[]( real x ) -> real
{
    return 1.0 / ( 1.0 + pow( M_E, ( -x )));
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
inline NeuronFunc FUNC_SIGMOID_BACK =
[]( real x ) -> real
{
    return x * ( 1 - x );
};






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
/*
    Convert string to function
*/
inline NeuronFunc* strToFunc
(
    string aArgument
)
{
    if( aArgument == "NULL" )           return &FUNC_NULL;
    if( aArgument == "ZERO" )           return &FUNC_ZERO;
    if( aArgument == "LINE" )           return &FUNC_LINE;
    if( aArgument == "ONE" )            return &FUNC_ONE;
    if( aArgument == "STEP" )           return &FUNC_STEP;
    if( aArgument == "RELU" )           return &FUNC_RELU;
    if( aArgument == "SIGMOID" )        return &FUNC_SIGMOID;
    if( aArgument == "SIGMOID_BACK" )   return &FUNC_SIGMOID_BACK;
    return &FUNC_NULL;
}



inline string neuronFuncToStr
(
    NeuronFunc* a
)
{
    if( a == &FUNC_NULL )               return "NULL";
    if( a == &FUNC_ZERO )               return "ZERO";
    if( a == &FUNC_LINE )               return "LINE";
    if( a == &FUNC_ONE )                return "ONE";
    if( a == &FUNC_STEP )               return "STEP";
    if( a == &FUNC_RELU )               return "RELU";
    if( a == &FUNC_SIGMOID )            return "SIGMOID";
    if( a == &FUNC_SIGMOID_BACK )       return "SIGMOID_BACK";
    return "NULL";
}

