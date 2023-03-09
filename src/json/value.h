/*
    It is a dafault Value container with type UNKNOWN
    Value has a Type and Name
*/

#pragma once

#include <string>
#include "param_type.h"



using namespace std;


struct ParamList;
struct Array;


struct Value
{
    /*
        Constructor
    */
    Value();



    /*
        Destructor
    */
    ~Value();



    /*
        Create a new Value
    */
    static Value* create();



    /*
        Destroy the Value
    */
    void destroy();




    /*
        Set name of the Value
    */
    Value* setType
    (
        ParamType /* Value for Value type */
    );



    /*
        Return the type of the Value
    */
    virtual ParamType getType();



    /*
        Return the type of the Value
    */
    virtual string getNameOfType();



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
        Return the value as Value list
    */
    virtual ParamList* getObject();



    /*
        Return the value as Value list
    */
    virtual ArrayList* getArray();

};
