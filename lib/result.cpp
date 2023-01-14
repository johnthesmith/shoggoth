#include <iostream>
#include <string>

#include "result.h"


using namespace std; // для того что бы std:: не приходилось каждый раз писать



Result* Result::setCode
(
    string a  /* String for Text */
)
{
    code = a;
    return this;
}



Result* Result::setMessage
(
    string a  /* String for Text */
)
{
    message = a;
    return this;
}



/*
    Set result and message
*/
Result* Result::setResult
(
    string aCode,   /* Code value */
    string aMessage /* Message value */
)
{
    setCode( aCode );
    setMessage( aMessage );
    return this;
}



bool Result::isOk()
{
    return code == "";
}
