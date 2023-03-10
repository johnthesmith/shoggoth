#include <iostream>

#include "param_string.h"
#include "param_type.h"



using namespace std;



/*
    Constructor
*/
ParamString::ParamString()
{
}



/*
    Destructor
*/
ParamString::~ParamString()
{
}



/*
    Create a new ParamString
*/
ParamString* ParamString::create()
{
    return new ParamString();
}



/*
    Destroy the ParamString
*/
void ParamString::destroy()
{
    delete this;
}



/******************************************************************************
    Overrided methods
*/



/*
    Set string value
*/
ParamString* ParamString::setName
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
ParamType ParamString::getType()
{
    return KT_STRING;
}



/*
    Return the type of the Param
*/
string ParamString::getNameOfType()
{
    return "STRING";
}



/******************************************************************************
    Specific methods
*/


/*
    Set string value
*/
ParamString* ParamString::setValue
(
    string a /* Value */
)
{
    value = a;
    return this;
}



/*
    Return value as string
*/
string ParamString::getValue()
{
    return value;
}



/*
    Return the value as string
*/
string ParamString::getString()
{
    return getValue();
}



/*
    Return the value as bool
*/
bool ParamString::getBool()
{
    return getValue() == "true" || getValue() == "TRUE";
}



/*
    Return the value as integer
*/
long long int ParamString::getInt()
{
    return atol( getValue().c_str() );
}



/*
    Return the value as float value
*/
double ParamString::getDouble()
{
    return atof( getValue().c_str() );
}
