/* System libraries */
#include <iostream>

/* Application libraryes */
#include "shoggoth_application.h"

#include "../../../../lib/core/moment.h"
#include "../../../../lib/json/json.h"


using namespace std;



int main
(
    int argc,
    char** argv
)
{
//    auto json = Json::create() -> fromFile( "./net/alpha/zero/net.json" );
//cout <<    json -> toString( false );
//
//   auto param = json -> getParamList() -> getByName( Path{ "layers", "cortex_1", "size", "1" });
//   if( param != NULL )
//   {
//       cout << param -> getNameOfType();
//   }

    ShoggothApplication::create( argc, argv )
    -> run()
    -> destroy();

    return 0;
}
