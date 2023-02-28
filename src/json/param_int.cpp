#include <iostream>

#include "param_int.h"
#include "param_type.h"



using namespace std;



/*
    Constructor
*/
ParamInt::ParamInt()
{
}



/*
    Destructor
*/
ParamInt::~ParamInt()
{
}



/*
    Create a new ParamInt
*/
ParamInt* ParamInt::create()
{
    return new ParamInt();
}



/*
    Destroy the ParamInt
*/
void ParamInt::destroy()
{
    delete this;
}



/******************************************************************************
    Overrided methods
*/



/*
    Set string value
*/
ParamInt* ParamInt::setName
(
    string a /* Value */
)
{
    Param::setName( a );
    return this;
}



/*
    Return the type of the Param
*/
ParamType ParamInt::getType()
{
    return KT_INT;
}



/*
    Return the type of the Param
*/
string ParamInt::getNameOfType()
{
    return "INT";
}



/******************************************************************************
    Specific methods
*/


/*
    Set string value
*/
ParamInt* ParamInt::setValue
(
    long long int a /* Value */
)
{
    value = a;
    return this;
}



/*
    Return value as string
*/
long long int ParamInt::getValue()
{
    return value;
}

