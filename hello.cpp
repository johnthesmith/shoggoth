/* System libraries */
#include <iostream>

/* Libraryes */
#include "./lib/log.h"
#include "./lib/scene.h"
#include "./lib/matrix.h"
#include "./lib/log_points.h"

/* Application libraryes */
#include "./app/form.h"



using namespace std;



int main(int argc, char** argv)
{
    auto log = Log();

    log.begin( "Application start" );

    auto scene      = Scene( log );
    auto payload    = ScenePayload( log );
    scene.init().setPayload( payload ).loop().finit();

    log.end( "Application stop" );

    return 0;
}



//auto m = Matrix4().look( Point3( 1, 1, 1 ).norm(), Point3( 0,1,0 ).get() );
//auto &k = m;
//
//m.l1.x = 11;
//
//LogPoints::write( log, m );
//LogPoints::write( log, k );
//    log -> begin();
//    for( int i=0; i<2e6; i++ )
//    {
//        p1.add( &VECTOR_3D_X ).add( &VECTOR_3D_Y );
//    }
//    log -> end();
//
//    log -> begin();
//    for( int i=0; i<2e6; i++ )
//    {
//        p2.add1( VECTOR_3D_X ).add1( VECTOR_3D_Y );
//    }
//    log -> end() -> lineEnd();
