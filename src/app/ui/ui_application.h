/*
    Ui appliction definitions
    Called from ui.h
*/

#pragma once

/* Local libraries */
#include "../shoggoth_application.h"


using namespace std;



class UiApplication : public ShoggothApplication
{
    private:
    public:

        UiApplication
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Creator
        */
        static UiApplication* create
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Run application
        */
        UiApplication* run();



        /*
            Main loop of Ui
        */
        UiApplication* loop();

};

