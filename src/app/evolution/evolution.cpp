/*
    Evolution main file for Shoggoth neural network
*/
#include "evolution_application.h"



int main
(
    int argc,
    char** argv
)
{
    EvolutionApplication::create( argc, argv )
    -> run()
    -> destroy();
    return 0;
}
