#include <iostream>

#include "param_double.h"
#include "param_type.h"



using namespace std;



/*
    Constructor
*/
ParamDouble::ParamDouble()
{
}



/*
    Destructor
*/
ParamDouble::~ParamDouble()
{
}



/*
    Create a new ParamDouble
*/
ParamDouble* ParamDouble::create()
{
    return new ParamDouble();
}



/*
    Destroy the ParamDouble
*/
void ParamDouble::destroy()
{
    delete this;
}



/******************************************************************************
    Overrided methods
*/



/*
    Set string value
*/
ParamDouble* ParamDouble::setName
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
ParamType ParamDouble::getType()
{
    return KT_DOUBLE;
}



/*
    Return the type of the Param
*/
string ParamDouble::getNameOfType()
{
    return "DOUBLE";
}



/******************************************************************************
    Specific methods
*/



/*
    Set string value
*/
ParamDouble* ParamDouble::setValue
(
    double a /* Value */
)
{
    value = a;
    return this;
}



/*
    Return value as string
*/
double ParamDouble::getValue()
{
    return value;
}

