/*
    It is a Param with string
    Param has a Type and Name
*/

#pragma once

#include "param.h"



using namespace std;



struct ParamBool : public Param
{
    bool value = false;

    /*
        Constructor
    */
    ParamBool();



    /*
        Destructor
    */
    ~ParamBool();



    /*
        Create a new ParamBool
    */
    static ParamBool* create();



    /*
        Destroy the ParamBool
    */
    void destroy();



    /******************************************************************************
        Overrided methods
    */

    /*
        Set string value
    */
    ParamBool* setName
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
    ParamBool* setValue
    (
        bool /* Value */
    );



    /*
        Return value
    */
    bool getValue();
};
