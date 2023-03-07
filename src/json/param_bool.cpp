#include <iostream>

#include "param_bool.h"
#include "param_type.h"



using namespace std;



/*
    Constructor
*/
ParamBool::ParamBool()
{
}



/*
    Destructor
*/
ParamBool::~ParamBool()
{
}



/*
    Create a new ParamBool
*/
ParamBool* ParamBool::create()
{
    return new ParamBool();
}



/*
    Destroy the ParamBool
*/
void ParamBool::destroy()
{
    delete this;
}



/******************************************************************************
    Overrided methods
*/



/*
    Set string value
*/
ParamBool* ParamBool::setName
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
ParamType ParamBool::getType()
{
    return KT_BOOL;
}



/*
    Return the type of the Param
*/
string ParamBool::getNameOfType()
{
    return "BOOL";
}



/******************************************************************************
    Specific methods
*/


/*
    Set string value
*/
ParamBool* ParamBool::setValue
(
    bool a /* Value */
)
{
    value = a;
    return this;
}



/*
    Return value as string
*/
bool ParamBool::getValue()
{
    return value;
}



/*
    Return the value as string
*/
string ParamBool::getString()
{
    return getValue() ? "true" : "false";
}



/*
    Return the value as bool
*/
bool ParamBool::getBool()
{
    return getValue();
}



/*
    Return the value as integer
*/
long long int ParamBool::getInt()
{
    return getValue() ? 1 : 0;
}



/*
    Return the value as float value
*/
double ParamBool::getDouble()
{
    return getValue() ? 1.0 : 0.0;
}
