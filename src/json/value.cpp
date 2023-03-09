#include "value.h"
#include "param_type.h"



using namespace std;

/*
    Constructor
*/
Value::Value()
{
}



/*
    Destructor
*/
Value::~Value()
{
}



/*
    Create a new Value
*/
Value* Value::create()
{
    return new Value();
}



/*
    Destroy the Value
*/
void Value::destroy()
{
    delete this;
}



/*
    Set name of the Value
*/
Value* Value::setType
(
    ParamType a /* Value for Value type */
)
{
    return this;
}



/*
    Return the type of the Value
*/
ParamType Value::getType()
{
    return KT_UNKNOWN;
}



/*
    Return the type of the Value
*/
string Value::getNameOfType()
{
    return "UNKNOWN";
}



/*
    Return the value as string
*/
string Value::getString()
{
    return "";
}



/*
    Return the value as bool
*/
bool Value::getBool()
{
    return false;
}



/*
    Return the value as integer
*/
long long int Value::getInt()
{
    return 0;
}



/*
    Return the value as float value
*/
double Value::getDouble()
{
    return 0.0;
}



/*
    Return the value as Value list
*/
ParamList* Value::getObject()
{
    return NULL;
}



/*
    Return the value as Value list
*/
ArrayList* Value::getArray()
{
    return NULL;
}
