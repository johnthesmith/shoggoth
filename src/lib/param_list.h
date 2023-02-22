/*
    List of Params
*/

#pragma once


#include "heap.h"
#include "param.h"



using namespace std;



class Param;



struct ParamList : public Heap
{

    ParamList();


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
        Return Param by name
    */
    int getIndexByName
    (
        string  /* Name of parameter */
    );



    string getString
    (
        string,         /* Name of parameter */
        string  = ""    /* Dafault value */
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
        Set double value
    */
    ParamList* setDouble
    (
        string,         /* Name of parameter */
        double = 0.0    /* Value */
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
        Set bool value
    */
    ParamList* setBool
    (
        string,         /* Name of parameter */
        bool = false    /* Value */
    );
};
