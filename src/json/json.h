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
    PairPart    pairPart        = PP_UNKNOWN;
    bool        fPairBegin      = false;

    bool        fArray          = false;
    bool        fEscape         = false;

    bool        fNameBegin      = false;
    bool        fNameEnd        = false;
    string      name            = "";

    bool        fValueBegin     = false;
    bool        fValueEnd       = false;
    bool        fStringBegin    = false;
    bool        fStringEnd      = false;

    string      value           = "";

    ParamList*  valueParamList  = NULL;
    ParamList*  paramList       = NULL;

    ParamType   valueType       = KT_UNKNOWN;
    JsonObject* prevJsonObject  = NULL;

    static JsonObject* create
    (
        JsonObject* = NULL
    );

    JsonObject* nameBegin();
    JsonObject* nameEnd();
    JsonObject* valueBegin();
    JsonObject* valueEnd();
    JsonObject* pairBegin();
    JsonObject* pairEnd();
    JsonObject* deleteObject
    (
        bool /* Destroy parmList object */
    );

    JsonObject* addChar
    (
        char
    );
};



/*
    Convert string to Json
*/
class Json : public Result
{
    private:

        int                 line                = 0;    /* Current line */
        int                 index               = 0;    /* Trace index of string */
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
            Mearge paramlist to other paramList
        */
        Json* copyTo
        (
            ParamList*
        );



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



        Json* fromFile
        (
            string
        );



        Json* dump();



        Json* trace
        (
            char c,
            JsonObject*
        );



        Json* error
        (
            string,     /* message */
            JsonObject*
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



        double getDouble
        (
            string,
            double = 0.0
        );



        double getDouble
        (
            vector <string>,
            double = 0.0
        );



        ParamList* getObject
        (
            string,
            ParamList* = NULL
        );



        ParamList* getObject
        (
            vector <string>,
            ParamList* = NULL
        );


};
