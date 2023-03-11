#pragma once

#include <string>
#include "../json/param_list.h"



using namespace std;



const string RESULT_OK = "ok";


class Result
{
    private:
        /* Состояния текстового процессора */
        string      message     = "";
        string      code        = RESULT_OK;
        ParamList*  details     = NULL;

    public:

        Result& setCode
        (
            string /* String for Text */
        );

        /*
            Return result code
        */
        string getCode();



        /*
            Set message for object
        */
        Result& setMessage
        (
            string /* String for Text */
        );



        /*
            Return message for object
        */
        string getMessage();



        /*
            Set result and message
        */
        Result& setResult
        (
            string,             /* Code value */
            string = "",        /* Message value */
            ParamList* = NULL
        );

        /*
            Return true if result code is ok
        */
        bool isOk();



        /*
            Copy result from object
        */
        Result& resultFrom
        (
            Result&
        );



        /*
            Copy result to object
        */
        Result& resultTo
        (
            Result&
        );

};
