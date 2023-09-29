#pragma once



#include "param_list.h"
#include "../lib/log.h"



using namespace std;



class ParamListLog
{
    public:
        /*
            Dump param list to log
        */
        static void dump
        (
            Log*,
            ParamList*,
            string = "dump"
        );

    private:

        static void dumpInternal
        (
            Log*,
            ParamList*,
            string,
            int
        );
};
