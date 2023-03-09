#include <iostream>
#include <regex>

#include "json.h"
#include "param_type.h"
#include "param_object.h"

#include "../lib/utils.h"



Json::Json()
{
    paramList = ParamList::create();
}



Json::~Json()
{
    paramList -> destroy();
}



Json* Json::create()
{
    return new Json();
}



void Json::destroy()
{
    delete this;
}



/*
    Convert string to Json
*/
Json* Json::fromString
(
    const string a
)
{

    int c = a.length();

    /* Default object */
    auto obj = JsonObject::create();
    obj -> name = "ROOT";
    obj -> pairBegin();
    obj -> nameEnd();
    obj -> pairPart = PP_VALUE;

    for( index = 0; index < c && isOk(); index++ )
    {
        switch( a[ index ] )
        {
            case '{':
                if( !obj -> fStringBegin )
                {
                    switch( obj -> pairPart )
                    {
                        case PP_UNKNOWN:
                            setResult( "Unexpected { outside of object" );
                        break;
                        case PP_NAME:
                            setResult( "Unexpected { in name" );
                        break;
                        case PP_VALUE:
                            if( !obj -> fValueBegin )
                            {
                                obj = JsonObject::create( obj );
                            }
                            if( obj -> fValueBegin || obj -> fValueEnd )
                            {
                                setResult( "Unexpected { in value" );
                            }
                        break;
                    }
                    obj -> pairBegin();
                }
                else
                {
                    obj -> addChar( a[ index ] );
                }
            break;

            case '}':
                if( !obj -> fStringBegin )
                {
                    if( !obj -> fPairBegin )
                    {
                        setResult( "Unexpected } without name" );
                    }
                    else
                    {
                        switch( obj -> pairPart )
                        {
                            case PP_NAME:
                                if( obj -> fNameBegin )
                                {
                                    setResult( "Unexpected } in name" );
                                }
                                else
                                {
                                    /* Close ROOT case */
                                    if( obj -> prevJsonObject != NULL )
                                    {
                                        if( obj -> name != "")
                                        {
                                            obj -> pairEnd();
                                        }
                                        obj = obj -> deleteObject( false );
                                    }
                                }
                            break;
                            case PP_UNKNOWN:
                            case PP_VALUE:
                                if( obj -> prevJsonObject != NULL )
                                {
                                    if( obj -> name != "")
                                    {
                                        obj -> pairEnd();
                                    }
                                    obj = obj -> deleteObject( false );
                                }
                                else
                                {
                                    setResult( "Unexpected } and heracly error" );
                                }
                            break;
                        }
                    }
                }
                else
                {
                    obj -> addChar( a[ index ] );
                }
            break;

            case '"':
                if( !obj -> fStringBegin )
                {
                    obj -> fStringBegin = true;
                    obj -> fStringEnd = false;

                    switch( obj -> pairPart )
                    {
                        case PP_UNKNOWN:
                        case PP_NAME:
                            if( !obj -> fNameBegin && !obj -> fNameEnd )
                            {
                                obj -> nameBegin();
                                obj -> pairBegin();
                            }
                            else
                            {
                                setResult( "Unexpected \" in name" );
                            }
                            break;
                        case PP_VALUE:
                            if( !obj -> fValueBegin && !obj -> fValueEnd )
                            {
                                obj -> valueBegin();
                            }
                            else
                            {
                                setResult( "Unexpected \" in value part" );
                            }
                            break;
                    }
                }
                else
                {
                    if( !obj -> fEscape )
                    {
                        if( obj -> fValueBegin && obj -> fStringBegin )
                        {
                            obj -> valueEnd();
                        }

                        if( obj -> fNameBegin )
                        {
                            obj -> nameEnd();
                            if( obj -> name == "" )
                            {
                                setResult( "NameIsEmpty" );
                            }
                        }

                        obj -> fStringBegin = false;
                        obj -> fStringEnd = true;
                    }
                    else
                    {
                        obj -> addChar( a[ index ] );
                    }
                }
            break;

            case '\\':
                if( obj -> fStringBegin )
                {
                    obj -> addChar( a[ index ] );
                    obj -> fEscape = true;
                }
                else
                {
                    setResult( "Unexpected \\" );
                }
            break;

            case ':':
                if( !obj -> fStringBegin )
                {
                    if( obj -> pairPart == PP_NAME )
                    {
                        obj -> fStringBegin = false;
                        obj -> fStringEnd = false;
                        obj -> pairPart = PP_VALUE;
                    }
                    else
                    {
                        setResult( "Unexpected : in value part" );
                    }
                }
                else
                {
                    obj -> addChar( a[ index ] );
                }
            break;

            case ' ':
                if( !obj -> fStringBegin )
                {
                    if( obj -> fValueEnd && obj -> pairPart == PP_VALUE )
                    {
                        obj -> pairEnd();
                    }
                }
                else
                {
                    obj -> addChar( a[ index ] );
                }
            break;

            case ',':
                if( !obj -> fStringBegin )
                {
                    if( obj -> pairPart == PP_VALUE )
                    {
                        obj -> pairEnd();
                    }
                }
                else
                {
                    obj -> addChar( a[ index ] );
                }
            break;

            case '\n':
                if( obj -> fStringBegin )
                {
                    obj -> addChar( a[ index ] );
                }
                else
                {
                    line ++;
                }
            break;

            default:
                if( obj -> fValueEnd )
                {
                    setResult( "Unexpected character after value" );
                }
                if( !obj -> fNameBegin && !obj -> fNameEnd )
                {
                    setResult( "Unexpected character before name" );
                }
                if( obj -> fNameEnd && obj -> pairPart == PP_NAME )
                {
                    setResult( "Unexpected character after name" );
                }

                if( isOk() )
                {
                    obj -> addChar( a[ index ]);
                }
            break;
        }
        trace( a[ index ], obj );
    }

    /* Check heracly */
    if( isOk() )
    {
        if( obj -> prevJsonObject != NULL )
        {
            setResult( "HeraclyError" );
        }
        else
        {
            obj -> pairEnd();
            if( obj -> paramList -> getByIndex( 0 ) != NULL )
            {
                paramList -> destroy();
                paramList = ((ParamObject*)( obj -> paramList -> getByIndex( 0 ))) -> getValue();
            }
            else
            {
                setResult( "UnknownFormat" );
            }
        }
    }

    /* Clear stack */
    while( obj != NULL )
    {
        obj = obj -> deleteObject( !isOk() );
    }

    paramList -> dump();

    return this;
}



/*
    Convert json to string
*/
string Json::toString()
{
    string result = "";
    return result;
}



/******************************************************************************
    Utils
*/

Json* Json::trace
(
    char c,
    JsonObject* obj
)
{
    cout << "-begin--------------------------------------\n";
    cout << "result     " << getCode() << "\n";
    cout << "i          " << index << "\n";
    cout << "char       '" << c << "'\n";
    cout << "line       " << line << "\n";

    cout << "pairPart   " << obj -> pairPart << "\n";
    cout << "pairBegin  " << obj -> fPairBegin << "\n";
    cout << "fString    " << obj -> fStringBegin << " " << obj -> fStringEnd << "\n";
    cout << "fEscape    " << obj -> fEscape << "\n";
    cout << "fName      " << obj -> fNameBegin << " " << obj -> fNameEnd << "\n";
    cout << "fValue         " << obj -> fValueBegin << " " << obj -> fValueEnd << "\n";
    cout << "valueParamList " << obj -> valueParamList << "\n";

    cout << "valueType  " << obj -> valueType << "\n";
    cout << "Name       \"" << obj -> name << "\"\n";
    cout << "value      \"" << obj -> value << "\"\n";
    cout << "-end--------------------------------------\n";

    return this;
}





ParamType Json::getType
(
    string a
)
{
    ParamType result = KT_STRING;

    if( a == "true" || a == "false" || a == "TRUE" || a == "FALSE" )
    {
        result = KT_BOOL;
    }
    else
    {
        if( a == "null" || a == "NULL" )
        {
            result = KT_NULL;
        }
        else
        {
            if( regex_match( a, regex( "[-+]?([0-9]+)" ) ))
            {
                result = KT_INT;
            }
            else
            {
                if( regex_match( a, regex( "[-+]?([0-9]*[\\.\\,][0-9]+)" ) ))
                {
                    result = KT_DOUBLE;
                }
            }
        }
    }
    return result;
}




string Json::getString
(
    string aName,
    string aDefault
)
{
    return paramList -> getString( aName, aDefault );
}




string Json::getString
(
    vector <string> aName,
    string aDefault
)
{
    return paramList -> getString( aName, aDefault );
}




long long int Json::getInt
(
    string aName,
    long long int aDefault
)
{
    return paramList -> getInt( aName, aDefault );
}




long long int Json::getInt
(
    vector <string> aName,
    long long int aDefault
)
{
    return paramList -> getInt( aName, aDefault );
}




ParamList* Json::getObject
(
    string aName,
    ParamList* aDefault
)
{
    return paramList -> getObject( aName, aDefault );
}




ParamList* Json::getObject
(
    vector <string> aName,
    ParamList* aDefault
)
{
    return paramList -> getObject( aName, aDefault );
}




/******************************************************************************
    JsonObject
*/


JsonObject* JsonObject::create
(
    JsonObject* aParent
)
{
    auto result = new JsonObject();
    result -> prevJsonObject = aParent;
    result -> paramList = new ParamList();
    return result;
}



JsonObject* JsonObject::deleteObject
(
    bool aDestroyParamList
)
{
    if( aDestroyParamList )
    {
        /* The object paramList must be completely destroy */
        paramList -> destroy();
    }
    else
    {
        /* ParamList using like value in parent Object */
        if( prevJsonObject != NULL )
        {
            prevJsonObject -> valueParamList = paramList;
        }
    }
    auto result = prevJsonObject;
    delete this;

    return result;
}



JsonObject* JsonObject::nameBegin()
{
    fNameBegin = true;
    fNameEnd = false;
    return this;
}



JsonObject* JsonObject::nameEnd()
{
    fNameBegin = false;
    fNameEnd = true;
    return this;
}



JsonObject* JsonObject::valueBegin()
{
    fValueBegin = true;
    fValueEnd = false;
    return this;
}



JsonObject* JsonObject::valueEnd()
{
    fValueBegin = false;
    fValueEnd = true;
    return this;
}




JsonObject* JsonObject::pairBegin()
{
    fPairBegin = true;
    pairPart = PP_NAME;
    return this;
}



JsonObject* JsonObject::pairEnd()
{
    if( fStringEnd )
    {
        paramList -> setString( name, value );
    }
    else
    {
        if( valueParamList != NULL)
        {
            paramList -> setObject( name, valueParamList);
        }
        else
        {
            switch( Json::getType( value ) )
            {
                case KT_NULL    : paramList -> setInt( name, 0 ); break;
                case KT_STRING  : paramList -> setString( name, value ); break;
                case KT_BOOL    : paramList -> setBool( name, stringToBool( value )); break;
                case KT_INT     : paramList -> setInt( name, stringToInt( value )); break;
                case KT_DOUBLE  : paramList -> setDouble( name, stringToDouble( value )); break;
            }
        }
    }

    fStringBegin = false;
    fStringEnd = false;
    fValueBegin = false;
    fValueEnd = false;
    fNameBegin = false;
    fNameEnd = false;
    name = "";
    value = "";
    valueParamList = NULL;
    pairPart = PP_UNKNOWN;

    return this;
}



JsonObject* JsonObject::addChar
(
    char a
)
{
    if( fNameBegin && !fNameEnd )
    {
        name += a;
    }

    if( pairPart == PP_VALUE && !fValueBegin && !fValueEnd )
    {
        valueBegin();
    }

    if( fValueBegin && !fValueEnd )
    {
        value += a;
    }

    if( fEscape )
    {
        fEscape = false;
    }

    return this;
}




