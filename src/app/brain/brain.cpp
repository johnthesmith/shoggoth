/*
    Processor main file for Shoggoth neural network
*/
#include "brain_application.h"



int main
(
    int argc,
    char** argv
)
{
    BrainApplication::create( argc, argv )
    -> run()
    -> destroy();
    return 0;
}
