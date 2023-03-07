#include <iostream>

#include "param_list.h"
#include "param_object.h"
#include "param_type.h"


using namespace std;



/*
    Constructor
*/
ParamObject::ParamObject()
{
    value = ParamList::create();
}



/*
    Destructor
*/
ParamObject::~ParamObject()
{
    value -> clear() -> destroy();
}



/*
    Create a new ParamObject
*/
ParamObject* ParamObject::create()
{
    return new ParamObject();
}



/*
    Destroy the ParamObject
*/
void ParamObject::destroy()
{
    delete this;
}



/******************************************************************************
    Overrided methods
*/



/*
    Set string value
*/
ParamObject* ParamObject::setName
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
ParamType ParamObject::getType()
{
    return KT_OBJECT;
}



/*
    Return the type of the Param
*/
string ParamObject::getNameOfType()
{
    return "OBJECT";
}



/******************************************************************************
    Specific methods
*/



/*
    Set string value
*/
ParamObject* ParamObject::setValue
(
    ParamList* a /* Value */
)
{
    value = a;
    return this;
}



/*
    Return value as string
*/
ParamList* ParamObject::getValue()
{
    return value;
}



/*
    Return the value as string
*/
string ParamObject::getString()
{
    return "[OBJECT]";
}



/*
    Return the value as bool
*/
bool ParamObject::getBool()
{
    return getValue() != NULL;
}



/*
    Return the value as integer
*/
ParamList* ParamObject::getObject()
{
    return getValue();
}

