#include <iostream>

#include "json.h"
#include "param_type.h"



Json::Json()
{
    paramList = new ParamList();
}



Json::~Json()
{
    delete paramList;
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
    newObject();
    auto obj = getObject();
    obj -> name = "ROOT";
    pairBegin();
    nameEnd();
    obj -> pairPart = PP_VALUE;

    for( index = 0; index < c && isOk(); index++ )
    {
        obj = getObject();
        label = "";

        switch( a[ index ] )
        {
            case '{':
                if( !fStringBegin )
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
                                newObject();
                                obj = getObject();
                            }
                            if( obj -> fValueBegin || obj -> fValueEnd )
                            {
                                setResult( "Unexpected { in value" );
                            }
                        break;
                    }
                }
                else
                {
                    addChar( a[ index ] );
                }
            break;

            case '}':
                if( !fStringBegin )
                {
                    switch( obj -> pairPart )
                    {
                        case PP_NAME:
                            setResult( "Unexpected } in name" );
                        break;
                        case PP_UNKNOWN:
                        case PP_VALUE:
                            if( objects.size() > 1 )
                            {
                                valueEnd();
                                popObject();
                                pairEnd();
                            }
                            else
                            {
                                setResult( "Unexpected } and heracly error" );
                            }
                        break;
                    }
                }
                else
                {
                    addChar( a[ index ] );
                }
            break;

            case '"':
                if( !fStringBegin )
                {
                    fStringBegin = true;
                    fStringEnd = false;

                    switch( obj -> pairPart )
                    {
                        case PP_UNKNOWN:
                        case PP_NAME:
                            if( !obj -> fNameBegin && !obj -> fNameEnd )
                            {
                                pairBegin();
                                nameBegin();
                            }
                            else
                            {
                                setResult( "Unexpected \" in name" );
                            }
                            break;
                        case PP_VALUE:
                            if( !obj -> fValueBegin && !obj -> fValueEnd )
                            {
                                valueBegin();
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
                        if( obj -> fValueBegin && fStringBegin )
                        {
                            valueEnd();
                        }


                        if( obj -> fNameBegin )
                        {
                            nameEnd();
                        }

                        fStringBegin = false;
                        fStringEnd = true;
                    }
                    else
                    {
                        addChar( a[ index ] );
                    }
                }
            break;

            case '\\':
                if( fStringBegin )
                {
                    if( obj -> fEscape )
                    {
                        addChar( a[ index ] );
                    }
                    else
                    {
                        obj -> fEscape = true;
                    }
                }
            break;

            case ':':
                if( !fStringBegin )
                {
                    if( obj -> pairPart != PP_NAME )
                    {
                        setResult( "Unexpected : in value part" );
                    }
                    else
                    {
                        obj -> pairPart = PP_VALUE;
                    }
                }
                else
                {
                    addChar( a[ index ] );
                }
            break;

            case ',':
                if( !fStringBegin )
                {
                    if( obj -> fValueBegin )
                    {
                        valueEnd();
                    }
                    if( !obj -> fValueBegin && obj -> fValueEnd )
                    {
                        pairEnd();
                    }
                }
                else
                {
                    addChar( a[ index ] );
                }
            break;

            case ' ':
                if( obj -> fValueBegin || obj -> fNameBegin )
                {
                    addChar( a[ index ]);
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

                addChar( a[ index ]);
            break;

            case '\n':
                if( obj -> fEscape )
                {
                    addChar( a[ index ] );
                }
                else
                {
                    line ++;
                }
            break;
        }
        trace( a[ index ] );
    }

    if( objects.size() != 1 )
    {
        if( isOk() )
        {
            setResult( "HeraclyError" );
        }
    }
    else
    {
        popObject();
    }

    return this;
}



Json* Json::addChar
(
    char a
)
{
    auto obj = getObject();
    if( obj -> fNameBegin && !obj -> fNameEnd )
    {
        obj -> name += a;
    }

    if( obj -> pairPart == PP_VALUE && !obj -> fValueBegin && !obj -> fValueEnd )
    {
       valueBegin();
    }

    if( obj -> fValueBegin && !obj -> fValueEnd )
    {
        obj -> value += a;
    }

    if( obj -> fEscape )
    {
        obj -> fEscape = false;
    }
    return this;
}




Json* Json::nameBegin()
{
    auto obj = getObject();
    obj -> fNameBegin = true;
    obj -> fNameEnd = false;
    return this;
}



Json* Json::nameEnd()
{
    auto obj = getObject();

    obj -> fNameBegin = false;
    obj -> fNameEnd = true;

    if( obj -> name == "" )
    {
        setResult( "NameIsEmpty" );
    }
    return this;
}



Json* Json::valueBegin()
{
    auto obj = getObject();

    obj -> fValueBegin = true;
    obj -> fValueEnd = false;
    return this;
}



Json* Json::valueEnd()
{
    auto obj = getObject();

    obj -> fValueBegin = false;
    obj -> fValueEnd = true;

    return this;
}



Json* Json::pairBegin()
{
    auto obj = getObject();
    obj -> pairPart = PP_NAME;
    return this;
}



Json* Json::pairEnd()
{
    auto obj = getObject();

    obj -> fValueBegin = false;
    obj -> fValueEnd = false;
    obj -> fNameBegin = false;
    obj -> fNameEnd = false;
    obj -> name = "";
    obj -> value = "";
    obj -> pairPart = PP_UNKNOWN;

    return this;
}





/******************************************************************************
    Objects work
*/

Json* Json::newObject()
{
    auto object = new JsonObject();
    objects.push( object );
    return this;
}



Json* Json::popObject()
{
    auto object = getObject();
    delete object;
    objects.pop();
    return this;
}




JsonObject* Json::getObject()
{
    return objects.top();
}




/******************************************************************************
    Utils
*/

Json* Json::trace
(
    char c
)
{
    auto obj = getObject();

    cout << "---------------------------------------\n";
    cout << "result     " << getCode() << "\n";
    cout << "label      \"" << label << "\"\n";
    cout << "stack      " << objects.size() << "\n";
    cout << "i          " << index << "\n";
    cout << "char       '" << c << "'\n";
    cout << "line       " << line << "\n";

    cout << "pairPart   " << obj -> pairPart << "\n";
    cout << "fString    " << fStringBegin << " " << fStringEnd << "\n";
    cout << "fEscape    " << obj -> fEscape << "\n";
    cout << "fName      " << obj -> fNameBegin << " " << obj -> fNameEnd << "\n";
    cout << "fValue     " << obj -> fValueBegin << " " << obj -> fValueEnd << "\n";

    cout << "valueType  " << getObject() -> valueType << "\n";
    cout << "Name       \"" << getObject() -> name << "\"\n";
    cout << "value      \"" << getObject() -> value << "\"\n";


    return this;
}


