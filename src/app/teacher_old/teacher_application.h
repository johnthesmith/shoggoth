/*
TODO переделать на использование тещего модуля
    Teacher application definitions
*/



#pragma once



/* Local libraries */
#include "../shoggoth_application.h"



using namespace std;



class TeacherApplication : public ShoggothApplication
{
    private:
    public:

        TeacherApplication
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Creator
        */
        static TeacherApplication* create
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Run application
        */
        TeacherApplication* run();



        /*
            Main loop of Teacher
        */
        TeacherApplication* loop();

};

