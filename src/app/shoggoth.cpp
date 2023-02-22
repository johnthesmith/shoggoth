/* System libraries */
#include <iostream>

/* Libraryes */
#include "../lib/log.h"
#include "../lib/param_list.h"
#include "../graph/scene.h"

/* Application libraryes */
#include "../app/form.h"



using namespace std;



int main
(
    int argc,
    char** argv
)
{
    auto log = Log();
    log.begin( "Application start" );


    auto scene      = Scene( log );
    auto payload    = Form::create( log );

    scene.init().setPayload( payload ).loop().finit();

    payload -> destroy();

    log.end( "Application stop" );

auto p = ParamList::create();
cout << p -> setString( "asd", "asd" ) -> getString( "asd", "def" ) << "\n";
cout << p -> setString( "asd", "asd1" ) -> getString( "asd", "def" ) << "\n";
p -> destroy();




    return 0;
}
