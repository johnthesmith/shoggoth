#pragma once

#include "../lib/result.h"

#include <stack>
#include <vector>
#include "param_list.h"



enum PairPart
{
    PP_UNKNOWN  = 0,
    PP_NAME     = 1,
    PP_VALUE    = 2
};



struct JsonObject
{
    PairPart pairPart           = PP_UNKNOWN;
    bool    pairBegin           = false;

    bool    fArray              = false;
    bool    fEscape             = false;

    bool    fNameBegin          = false;
    bool    fNameEnd            = false;
    string  name                = "";

    bool        fValueBegin     = false;
    bool        fValueEnd       = false;
    string      value           = "";
    ParamList*  valueParamList  = NULL;

    ParamList*  paramList       = NULL;

    ParamType   valueType       = KT_UNKNOWN;
    JsonObject* prevJsonObject  = NULL;

};



/*
    Convert string to Json
*/
class Json : public Result
{
    private:

        int                 line                = 0;    /* Current line */
        int                 index               = 0;    /* Trace index of string */
        bool                fStringBegin        = false;
        bool                fStringEnd          = false;
        string              label               = "";
        JsonObject*         currentJsonObject   = NULL;             /* State */
        ParamList*          paramList           = NULL;

    public:
        Json();
        ~Json();

        /*
            Create and return new json object
        */
        static Json* create();



        /*
            Create and return new json object
        */
        void destroy();



        /*
            Convert string to Json
        */
        Json* fromString
        (
            const string
        );



        /*
            Convert Json to string
        */
        string toString();



        Json* addChar
        (
            char
        );



        Json* trace
        (
            char c
        );




        Json* pairBegin();
        Json* pairEnd();
        Json* nameBegin();
        Json* nameEnd();
        Json* valueBegin();
        Json* valueEnd();

        Json* newObject();
        JsonObject* getObject();
        Json* deleteObject
        (
            bool /* Destroy parmList object */
        );



        static ParamType getType
        (
            string
        );



        string getString
        (
            string,
            string = ""
        );



        string getString
        (
            vector <string>,
            string = ""
        );



        long long int getInt
        (
            string,
            long long int = 0
        );



        long long int getInt
        (
            vector <string>,
            long long int = 0
        );

};
