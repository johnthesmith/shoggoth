/*
    Processor main file for Shoggoth neural network
*/
#include "shoggoth_application.h"

int main
(
    int argc,
    char** argv
)
{
    ShoggothApplication::create( argc, argv )
    -> run()
    -> destroy();
    return 0;
}
