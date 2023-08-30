#include <iostream>
#include <cstring>

#include "param_list.h"
#include "param.h"
#include "../lib/buffer_to_hex.h"



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
    resize( 0 );
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
    Return the type of the Param
*/
ParamType Param::getType()
{
    return type;
}



/*
    Return the type of the Value
*/
string Param::getNameOfType()
{
    switch( getType() )
    {
        case KT_STRING  : return "STRING";
        case KT_INT     : return "INT";
        case KT_DOUBLE  : return "DOUBLE";
        case KT_BOOL    : return "BOOL";
        case KT_DATA    : return "DATA";
        case KT_OBJECT  : return "OBJECT";
        default:
        case KT_UNKNOWN : return "UNKNOWN";
    }
}



/******************************************************************************
    Specific methods
*/


/*
    Set string value
*/
Param* Param::setValue
(
    ParamType   aType,
    char*       aBuffer,  /* Buffer */
    size_t      aSize /* Size of buffer */
)
{
    if( getType() == KT_OBJECT )
    {
        getObject() -> destroy();
    }

    resize( aSize );

    /* Set type */
    type = aType;

    if( aSize == 0 )
    {
        value = aBuffer;
    }
    else
    {
        memcpy( value, aBuffer, size );
    }

    return this;
}



/*
    Return value pointer
*/
char* Param::getValue()
{
    return value;
}



/*
    Return size pointer
*/
size_t Param::getSize()
{
    return size;
}



/*
    Set new size
*/
Param* Param::resize
(
    size_t aSize
)
{
    if( aSize != size )
    {
        if( size != 0 )
        {
            delete [] value;
            value = NULL;
        }

        if( aSize != 0 )
        {
            size = aSize;
            value = new char[ size ];
        }
    }
    return this;
}




/*
    Return the value as string
*/
string Param::getString()
{
    string result = "";
    switch( getType() )
    {
        case KT_BOOL:
            result = getBool() ? "true" : "false";
        break;
        case KT_NULL:
            result = "null";
        break;
        case KT_UNKNOWN:
            result = "unknown";
        break;
        case KT_INT:
            result = to_string( getInt() );
        break;
        case KT_DOUBLE:
            result = to_string( getDouble() );
        break;
        case KT_DATA:
        case KT_STRING:
            result.resize( getSize() );
            memcpy( &result[0], getValue(), getSize() );
        break;
        case KT_OBJECT:
            result = "[object]";
        break;
    }
    return result;
}



/*
    Return the value as bool
*/
bool Param::getBool()
{
    bool result = false;

    switch( getType() )
    {
        case KT_BOOL:
            result = getSize() !=0 && getValue()[ 0 ] != 0;
        break;
        case KT_NULL:
            result = false;
        break;
        case KT_UNKNOWN:
            result = false;
        break;
        case KT_INT:
            result = getInt() != 0;
        break;
        case KT_DOUBLE:
            result = getDouble() != 0.0;
        break;
        case KT_DATA:
            result = getSize() != 0;
        break;
        case KT_STRING:
        {
            auto s = getString();
            toLowerCase( s );
            result = s != "" && s != " " && s != "false" && s != "null" && s != "0" && s != "0.0";
        }
        break;
        case KT_OBJECT:
        {
            auto p = getObject();
            result = p != NULL ? ( p -> getCount() == 0 ? false : true ): false;
        }
        break;
    }

    return result;
}



/*
    Return the value as integer
*/
long long int Param::getInt()
{
    long long int result = 0;

    switch( getType() )
    {
        case KT_BOOL:
            result = getBool() ? 1 : 0;
        break;
        case KT_NULL:
        case KT_UNKNOWN:
            result = 0;
        break;
        case KT_INT:
            memcpy( &result, getValue(), getSize() );
        break;
        case KT_DOUBLE:
            result = (long long int) getDouble();
        break;
        case KT_DATA:
        {
            auto copySize = min( sizeof( result ), getSize());
            memcpy( &(( char* ) &result )[ sizeof( result ) - copySize ], getValue(), copySize );
        }
        break;
        case KT_STRING:
        {
            result = stringToInt( getString() );
        }
        break;
        case KT_OBJECT:
        {
            auto p = getObject();
            result = p != NULL ? p -> getCount() : 0;
        }
        break;
    }

    return result;
}



/*
    Return the value as float value
*/
double Param::getDouble()
{
    double result = 0.0;

    switch( getType() )
    {
        case KT_BOOL:
            result = getBool() ? 1.0 : 0.0;
        break;
        case KT_NULL:
        case KT_UNKNOWN:
            result = 0.0;
        break;
        case KT_INT:
            result = (double) getInt();
        break;
        case KT_DOUBLE:
            memcpy( &result, getValue(), getSize() );
        break;
        case KT_DATA:
        {
            auto copySize = min( sizeof( result ), getSize());
            memcpy( &(( char* ) &result )[ sizeof( result ) - copySize ], getValue(), copySize );
        }
        break;
        case KT_STRING:
        {
            result = stringToDouble( getString() );
        }
        break;
        case KT_OBJECT:
        {
            auto p = getObject();
            result = p != NULL ? ( double )p -> getCount() : 0.0;
        }
        break;
    }

    return result;
}



/*
    Return the value as object if defined
*/
ParamList* Param::getObject()
{
    return getType() == KT_OBJECT ? (ParamList*)value : NULL;
}




/*
    Return the value as object if defined
*/
char* Param::getData()
{
    return value;
}



/*
    Set the value as string
*/
Param* Param::setString
(
    string aValue
)
{
    setValue( KT_STRING, &aValue[ 0 ], aValue.length() );
    return this;
}



/*
    Set the value as bool
*/
Param* Param::setBool
(
    bool aValue
)
{
    setValue( KT_BOOL, (char*)&aValue, sizeof( aValue ));
    return this;
}



/*
    Set the value as integer
*/
Param* Param::setInt
(
    long long int aValue
)
{
    setValue( KT_INT, (char*)&aValue, sizeof( aValue ));
    return this;
}



/*
    Set the value as float value
*/
Param* Param::setDouble
(
    double aValue
)
{
    setValue( KT_DOUBLE, (char*)&aValue, sizeof( aValue ));
    return this;
}



/*
    Set the value as object
*/
Param* Param::setObject
(
    ParamList* aValue
)
{
    setValue( KT_OBJECT, (char*)aValue );
    return this;
}



/*
    Set any data value
*/
Param* Param::setData
(
    char*   aBuffer,  /* Buffer */
    size_t  aSize /* Size of buffer */
)
{
    setValue( KT_DATA, aBuffer, aSize );
    return this;
}
