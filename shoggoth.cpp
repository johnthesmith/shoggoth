/* System libraries */
#include <iostream>

/* Libraryes */
#include "./src/lib/log.h"
#include "./src/lib/graph/scene.h"
#include "./src/lib/shoggoth/neuron.h"

/* Application libraryes */
#include "./src/app/form.h"



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

    return 0;
}
