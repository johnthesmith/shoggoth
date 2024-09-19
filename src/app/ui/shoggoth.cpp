/* System libraries */
#include <iostream>



/* Application libraryes */
#include "../app/shoggoth_role_application.h"



using namespace std;



int main
(
    int argc,
    char** argv
)
{
    ShoggothRoleApplication::create( argc, argv )
    -> run()
    -> destroy();

    return 0;
}
