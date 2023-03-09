#include "param.h"
#include "param_type.h"



using namespace std;

/*
    Constructor
*/
Param::Param()
{
}



/*
    Destructor
*/
Param::~Param()
{
}



/*
    Create a new Param
*/
Param* Param::create()
{
    return new Param();
}



/*
    Destroy the Param
*/
void Param::destroy()
{
    delete this;
}



/*
    Set name of the Param
*/
Param* Param::setType
(
    ParamType a /* Value for Param type */
)
{
    return this;
}



/*
    Return the type of the Param
*/
ParamType Param::getType()
{
    return Value::getType();
}



/*
    Return the type of the Param
*/
string Param::getNameOfType()
{
    return Value::getNameOfType();
}



/*
    Set Param Name
*/
Param* Param::setName
(
    string a
)
{
    name = a;
    return this;
}



/*
    Get Param name
*/
string Param::getName()
{
    return name;
}



/*
    Return the value as string
*/
string Param::getString()
{
    return Value::getString();
}



/*
    Return the value as bool
*/
bool Param::getBool()
{
    return Value::getBool();
}



/*
    Return the value as integer
*/
long long int Param::getInt()
{
    return Value::getInt();
}



/*
    Return the value as float value
*/
double Param::getDouble()
{
    return Value::getDouble();
}



/*
    Return the value as param list
*/
ParamList* Param::getObject()
{
    return Value::getObject();
}



/*
    Return the value as array list
*/
ParamList* Param::getObject()
{
    return Value::getArray();
}
