/* System libraries */
#include <iostream>

/* Application libraryes */
#include "shoggoth_application.h"
#include "../../../../lib/core/moment.h"


using namespace std;



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
