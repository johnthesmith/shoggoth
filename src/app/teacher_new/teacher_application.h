/*
    The Teacher application definitions
    Called from teacher.h
*/

#pragma once

/* Local libraries */
/* Load base shoggoth application class */
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
