/*
    Debugger main file for Shoggoth neural network
*/
#include "debugger_application.h"



int main
(
    int argc,
    char** argv
)
{
    DebuggerApplication::create( argc, argv )
    -> run()
    -> destroy();
    return 0;
}
