//#include "../../../../../lib/core/chart_data.h"
//#include <iostream>
//
//using namespace std;
//
//int main
//(
//    int argc,
//    char** argv
//)
//{
//    auto d = ChartData::create();
//    d -> createLast( 1.0 );
//    d -> createLast( 2.0 );
//    d -> createLast( 3.0 );
//    cout << d -> toString( 2 ) << "\n";
//    d -> destroy();
//    return 0;
//}
//


/*
    Processor main file for Shoggoth neural network
*/
#include "brain_application.h"


int main
(
    int argc,
    char** argv
)
{
    BrainApplication::create( argc, argv )
    -> run()
    -> destroy();
    return 0;
}
