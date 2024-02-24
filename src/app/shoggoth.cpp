/* System libraries */
#include <iostream>

/* Application libraryes */
#include "shoggoth_application.h"



using namespace std;



int main
(
    int argc,
    char** argv
)
{
//    /* Read actions */
//    auto a = ParamListFile::create();
////    a -> setPath( Path{"a","b", "c", "d", "e", "f" } )-> setString( "c", "b")
//    a -> fromJsonFile( "actions1.json" )
//    ;
//
//    a -> dump();
//    a -> destroy();

    ShoggothApplication::create( argc, argv )
    -> run()
    -> destroy()
    ;
    return 0;
}
