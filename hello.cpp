/* System libraries */
#include <iostream>

/* Libraryes */
#include "./lib/log.h"
#include "./lib/scene.h"
#include "./lib/matrix.h"
#include "./lib/log_points.h"
#include "./lib/point3i.h"

#include "./lib/neuron/neuron.h"
#include "./lib/neuron/layer.h"

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
    auto payload    = Form( log );
    auto layer      = Layer( log );

    layer.setSize( Point3i( 1000,1000,100) ).setSize( Point3i( 100,100,100) );

    scene.init().setPayload( payload ).loop().finit();

    log.end( "Application stop" );

    return 0;
}
