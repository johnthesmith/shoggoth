/*
    It is a Param with object value
    Param has a Type and Name
*/

#pragma once

#include "param.h"


using namespace std;



class ParamList;



struct ParamObject : public Param
{
    ParamList* value = NULL;



    /*
        Constructor
    */
    ParamObject();



    /*
        Destructor
    */
    ~ParamObject();



    /*
        Create a new ParamObject
    */
    static ParamObject* create();



    /*
        Destroy the ParamObject
    */
    void destroy();



    /******************************************************************************
        Overrided methods
    */

    /*
        Set string value
    */
    ParamObject* setName
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
        Set param list as value.
        Values will be copyed to new object
    */
    ParamObject* setValue
    (
        const ParamList* /* Value */
    );



    /*
        Return value
    */
    ParamList* getValue();
};
