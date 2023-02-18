/* System libraries */
#include <iostream>

/* Libraryes */
#include "./lib/log.h"
#include "./lib/graph/scene.h"
#include "./lib/graph/matrix.h"
#include "./lib/graph/point3i.h"


#include "./lib/shoggoth/neuron.h"
#include "./lib/shoggoth/layer.h"
#include "./lib/utils.h"

/* Application libraryes */
#include "./app/form.h"



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
