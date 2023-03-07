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
    return KT_UNKNOWN;
}



/*
    Return the type of the Param
*/
string Param::getNameOfType()
{
    return "UNKNOWN";
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
    return "";
}



/*
    Return the value as bool
*/
bool Param::getBool()
{
    return false;
}



/*
    Return the value as integer
*/
long long int Param::getInt()
{
    return 0;
}



/*
    Return the value as float value
*/
double Param::getDouble()
{
    return 0.0;
}



/*
    Return the value as param list
*/
ParamList* Param::getObject()
{
    return NULL;
}
