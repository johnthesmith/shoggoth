/*
    List of Params
*/

#pragma once

#include <vector>
#include <functional>   /* for lyambda */

#include "../lib/heap.h"
#include "param.h"



using namespace std;



struct ParamList : public Heap
{

    ParamList();



    ~ParamList();



    static ParamList* create();



    /*
        Add Params from argument list to this list
    */
    ParamList* add
    (
        ParamList*
    );




    /*
        Return index by Param
    */
    int indexBy
    (
        Param*
    );




    /*
        Return paramer or null by name
    */
    Param* getByName
    (
        string
    );



    /*
        Return parameter by list of name path
    */
    Param* getByName
    (
        vector <string> /* Names of parameter */
    );



    /*
        Return Param by index
    */
    Param* getByIndex
    (
        int
    );



    /*
        Return Param by index
    */
    ParamList* setByIndex
    (
        int,
        Param*
    );



    /*
        Resize
    */
    ParamList* resize
    (
        int
    );




    /*
        Delete Param by index
    */
    ParamList* deleteParam
    (
        int /* Index of the Param */
    );



    /*
        Clear list
        All parameters will be destroy and list will be resize
    */
    ParamList* clear();



    /*
        Return Param by name
    */
    int getIndexByName
    (
        string  /* Name of parameter */
    );



    string getString
    (
        int,            /* Index of parameter */
        string  = ""    /* Dafault value */
    );



    string getString
    (
        string,         /* Name of parameter */
        string  = ""    /* Dafault value */
    );



    string getString
    (
        vector <string>,/* Name of parameter */
        string  = ""    /* Dafault value */
    );



    bool getBool
    (
        int,            /* Index of parameter */
        bool = false    /* Dafault value */
    );



    bool getBool
    (
        string,         /* Name of parameter */
        bool = false    /* Dafault value */
    );



    /*
        Get boolean value by path
    */
    bool getBool
    (
        vector <string>,    /* Name path of parameter */
        bool = false        /* default value */
    );



    long long int getInt
    (
        int,                /* Name of parameter */
        long long int = 0   /* Dafault value */
    );



    long long int getInt
    (
        string,             /* Name of parameter */
        long long int = 0   /* Dafault value */
    );



    /*
        Get integer value by path
    */
    long long int getInt
    (
        vector <string>,    /* Name of parameter */
        long long int = 0   /* default value */
    );



    double getDouble
    (
        int,            /* Name of parameter */
        double = 0.0    /* Dafault value */
    );



    double getDouble
    (
        string,         /* Name of parameter */
        double = 0.0    /* Dafault value */
    );



    /*
        Get double value by path
    */
    double getDouble
    (
        vector <string>,  /* Name of parameter */
        double = 0        /* default value */
    );



    /*
        Get object value by name
    */
    ParamList* getObject
    (
        string,             /* Name of parameter */
        ParamList* = NULL   /* Value */
    );



    /*
        Get object value by index
    */
    ParamList* getObject
    (
        int aIndex,         /* Name of parameter */
        ParamList* = NULL   /* Value */
    );



    /*
        Get object value by path
    */
    ParamList* getObject
    (
        vector <string>,    /* Name of parameter */
        ParamList* = NULL   /* default value */
    );



    /*
        Push string value
    */
    ParamList* pushString
    (
        string = ""     /* Value */
    );


    /*
        Push bool value
    */
    ParamList* pushBool
    (
        bool = false     /* Value */
    );


    /*
        Push integer value
    */
    ParamList* pushInt
    (
        long long int = 0     /* Value */
    );


    /*
        Push double value
    */
    ParamList* pushDouble
    (
        double = 0.0     /* Value */
    );



    /*
        Push object value
    */
    ParamList* pushObject
    (
        ParamList*    /* Value */
    );



    /*
        Set string value
    */
    ParamList* setString
    (
        string,         /* Name of parameter */
        string = ""     /* Value */
    );




    /*
        Set bool value
    */
    ParamList* setBool
    (
        string,         /* Name of parameter */
        bool = false    /* Value */
    );



    /*
        Set int value
    */
    ParamList* setInt
    (
        string,             /* Name of parameter */
        long long int = 0   /* Value */
    );



    /*
        Set double value
    */
    ParamList* setDouble
    (
        string,         /* Name of parameter */
        double = 0.0    /* Value */
    );



    /*
        Set double value
    */
    ParamList* setObject
    (
        string,         /* Name of parameter */
        ParamList*    /* Value */
    );



    ParamList* dump
    (
        int = 0 /* depth */
    );



    /*
        Loop with lyambda
    */
    ParamList* loop
    (
        function <bool ( Param* )>
    );
};
