/*
    Processor main file for Shoggoth neural network
*/
#include "processor_application.h"



int main
(
    int argc,
    char** argv
)
{
    ProcessorApplication::create( argc, argv )
    -> run()
    -> destroy();
    return 0;
}
