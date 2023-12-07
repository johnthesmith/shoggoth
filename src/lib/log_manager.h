#pragma once

#include <string>
#include <map>
#include "log.h"



using namespace std;



class LogManager
{
    private:

        Log*    log     = NULL;

        /* List of logs for application */
        map <string, Log*> logList;


        /*
            Return thread id for current payload
        */
        string getThreadId();

    public:

        /**********************************************************************
            Log operations
        */


        /*
            Log manager constructor
        */
        LogManager
        (
            Log*    /* Default log */
        );



        /*
            Log manager  destructor
        */
        ~LogManager();



        /*
            Log manager creator
        */
        static LogManager* create
        (
            Log*    /* Default log */
        );



        /*
            Log manager destroy
        */
        void destroy();



        /*
            Create new log
        */
        Log* createLog
        (
            string  /* Log id */
        );



        /*
            Destroy log
        */
        LogManager* destroyLog();



        /*
            Return log object
        */
        Log* getLog();
};
