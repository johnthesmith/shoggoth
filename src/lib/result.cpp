#include <iostream>
#include <string>

#include "result.h"


using namespace std; // для того что бы std:: не приходилось каждый раз писать



void Result::destroy()
{
    delete this;
}




Result* Result::setOk()
{
    code = RESULT_OK;
    return this;
}



Result* Result::setCode
(
    string a  /* String for Text */
)
{
    code = a;
    return this;
}



string Result::getCode()
{
    return code;
}



Result* Result::setMessage
(
    string a  /* String for Text */
)
{
    message = a;
    return this;
}



string Result::getMessage()
{
    return message;
}



/*
    Set result and message
*/
Result* Result::setResult
(
    string aCode,       /* Code value */
    string aMessage     /* Message value */
)
{
    setCode( aCode );
    setMessage( aMessage );
    return this;
}



bool Result::isOk()
{
    return code == RESULT_OK;
}



/*
    Set result and message
*/
Result* Result::resultFrom
(
    Result* a
)
{
    setCode( a -> code );
    setMessage( a -> message );
    return this;
}



/*
    Set result and message
*/
Result* Result::resultTo
(
    Result* a
)
{
    a -> setCode( code );
    a -> setMessage( message );
    return this;
}
