/*
    It is a dafault Param container with type UNKNOWN
    Param has a Type and Name
*/

#pragma once

#include <string>
#include "param_type.h"



using namespace std;



struct Param
{
    string  name = "";           /* Name of Param */



    /*
        Constructor
    */
    Param();



    /*
        Destructor
    */
    ~Param();



    /*
        Create a new Param
    */
    static Param* create();



    /*
        Destroy the Param
    */
    void destroy();




    /*
        Set name of the Param
    */
    Param* setType
    (
        ParamType /* Value for Param type */
    );



    /*
        Return the type of the Param
    */
    virtual ParamType getType();



    /*
        Return the type of the Param
    */
    virtual string getNameOfType();



    /*
        Set Param Name
    */
    virtual Param* setName
    (
        string
    );



    /*
        Get Param name
    */
    string getName();
};
