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
    Return value
*/
double ParamDouble::getValue()
{
    return value;
}




/*
    Return the value as string
*/
string ParamDouble::getString()
{
    return to_string( getValue() );
}



/*
    Return the value as bool
*/
bool ParamDouble::getBool()
{
    return getValue() == 0.0 ? false : true;
}



/*
    Return the value as integer
*/
long long int ParamDouble::getInt()
{
    return (long long int) getValue();
}



/*
    Return the value as float value
*/
double ParamDouble::getDouble()
{
    return getValue();
}
