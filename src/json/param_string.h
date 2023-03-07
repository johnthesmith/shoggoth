/*
    It is a Param with string
    Param has a Type and Name
*/

#pragma once

#include <string>
#include "param.h"



using namespace std;



struct ParamString : public Param
{
    string value = "";

    /*
        Constructor
    */
    ParamString();



    /*
        Destructor
    */
    ~ParamString();



    /*
        Create a new ParamString
    */
    static ParamString* create();



    /*
        Destroy the ParamString
    */
    void destroy();



    /******************************************************************************
        Overrided methods
    */

    /*
        Set string value
    */
    ParamString* setName
    (
        string /* Value */
    ) override;



    /*
        Return the type of the Param
    */
    ParamType getType() override;



    /*
        Return the type of the Param
    */
    string getNameOfType() override;




    /******************************************************************************
        Specific methods
    */

    /*
        Return value
    */
    string getValue();



    /*
        Set string value
    */
    ParamString* setValue
    (
        string /* Value */
    );



    /*
        Return the value as string
    */
    string getString() override;



    /*
        Return the value as bool
    */
    bool getBool() override;



    /*
        Return the value as integer
    */
    long long int getInt() override;



    /*
        Return the value as float value
    */
    double getDouble() override;

};
