/*
    It is a Param with double value
    Param has a Type and Name
*/

#pragma once

#include "param.h"



using namespace std;



struct ParamDouble : public Param
{
    double value = 0.0;

    /*
        Constructor
    */
    ParamDouble();



    /*
        Destructor
    */
    ~ParamDouble();



    /*
        Create a new ParamDouble
    */
    static ParamDouble* create();



    /*
        Destroy the ParamDouble
    */
    void destroy();



    /******************************************************************************
        Overrided methods
    */

    /*
        Set string value
    */
    ParamDouble* setName
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
    ParamDouble* setValue
    (
        const double /* Value */
    );



    /*
        Return value
    */
    double getValue();
};
