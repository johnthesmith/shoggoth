/*
    It is a Param with integer value
    Param has a Type and Name
*/

#pragma once

#include "param.h"



using namespace std;



struct ParamInt : public Param
{
    long long int value = 0;

    /*
        Constructor
    */
    ParamInt();



    /*
        Destructor
    */
    ~ParamInt();



    /*
        Create a new ParamInt
    */
    static ParamInt* create();



    /*
        Destroy the ParamInt
    */
    void destroy();



    /******************************************************************************
        Overrided methods
    */

    /*
        Set string value
    */
    ParamInt* setName
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
        Set string value
    */
    ParamInt* setValue
    (
        long long int /* Value */
    );



    /*
        Return value
    */
    long long int getValue();
};
