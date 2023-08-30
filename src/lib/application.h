#pragma once

#include <string>
#include "result.h"
#include "log.h"
#include "../json/param_list.h"
#include "../json/param_list_file.h"
#include "../json/json.h"



using namespace std;




class Application : public Result
{
    private:

        ParamList*      cli     = NULL;
        ParamListFile*  config  = NULL;
        Log*            log     = NULL;

    public:

        /*
            Constructor
        */
        Application
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Destructor
        */
        ~Application();



        /*
            Creator
        */
        static Application* create
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Destroy
        */
        void destroy();



        /*
            Return log object
        */
        Log* getLog();



        /*
            Return config object
        */
        ParamList* getCli();



        /*
            Return config object
        */
        ParamListFile* getConfig();
};
