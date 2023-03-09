/*
    It is a dafault Param container with type UNKNOWN
    Param has a Type and Name
*/

#pragma once

#include <string>
#include "param_type.h"
#include "value.h"



using namespace std;


struct ParamList;


struct Param : public Value
{
    string  name = "";      /* Name of Param */



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



    /*
        Return the value as string
    */
    virtual string getString();



    /*
        Return the value as bool
    */
    virtual bool getBool();



    /*
        Return the value as integer
    */
    virtual long long int getInt();



    /*
        Return the value as float value
    */

    virtual double getDouble();



    /*
        Return the value as param list
    */
    virtual ParamList* getObject();



    /*
        Return the value as array list
    */
    virtual ParamList* getArray();
};
