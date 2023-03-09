/* System libraries */
#include <iostream>


#include <string>
#include <streambuf>
#include <sstream>
#include <fstream>

/* Libraryes */
#include "../lib/log.h"
#include "../json/json.h"
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
//    auto log = Log();
//    log.begin( "Application start" );
//
//    auto scene      = Scene( log );
//    auto payload    = Form::create( log );
//
//    scene.init().setPayload( payload ).loop().finit();
//
//    payload -> destroy();
//
//    log.end( "Application stop" );


ifstream t( "example.json" );
stringstream b;
b << t.rdbuf();

auto json = Json::create();
json -> fromString( b.str() );
cout << json -> getCode() << "\n";

cout << json -> getObject( vector <string> { "dd", "dd" } ) << "\n";

json -> destroy();

return 0;
}
