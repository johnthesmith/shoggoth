/*
    Ui main file for Shoggoth neural network
*/
#include "ui_application.h"



int main
(
    int argc,
    char** argv
)
{
    UiApplication::create( argc, argv )
    -> run()
    -> destroy();
    return 0;
}
