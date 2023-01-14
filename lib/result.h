#pragma once

#include <string>

using namespace std; // для того что бы std:: не приходилось каждый раз писать


class Result
{
    private:
        /* Состояния текстового процессора */
        string      message     = "";
        string      code        = "";

    public:

        Result * setCode
        (
            string /* String for Text */
        );

        Result * setMessage
        (
            string /* String for Text */
        );

        /*
            Set result and message
        */
        Result * setResult
        (
            string,     /* Code value */
            string = "" /* Message value */
        );

        /*
            Return true if result code is ok
        */
        bool isOk();
};
