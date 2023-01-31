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

    auto layer1     = Layer( log );
    auto layer2     = Layer( log );

    layer1.setSize( Point3i( 640, 480, 1 ));
    layer2.setSize( Point3i( 50, 50, 1 ));

   layer1.connectTo(layer2);

    scene.init().setPayload( payload ).loop().finit();

    log.end( "Application stop" );

    return 0;
}
