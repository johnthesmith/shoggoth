/*
    Processor main file for Shoggoth neural network
*/
#include "brain_application.h"
#include "../../../../../lib/core/thread_manager.h"


int main
(
    int argc,
    char** argv
)
{
//    mutex m;
//
//    auto app = BrainApplication::create( argc, argv );
//
//    auto manager = ThreadManager::create( app -> getLogManager() );
//
//    struct data
//    {
//        int id;
//        ThreadManager* manager;
//        mutex& m;
//        int count;
//    };
//
//    int c = 0;
//    while( true )
//    {
//        c++;
//        for( int i = 0; i < 100; i++ )
//        {
//            data d = { i, manager, m, c };
//
//            manager
//            -> add( toString( (long long int) d.id ))
//            -> run
//            (
//                []
//                ( void* d )
//                {
//                    auto dataRecord = (data*) d;
//                    auto manager = dataRecord -> manager;
//                    double a = 0.0;
//                    for( int l = 0; l < 10; l++ )
//                    {
//                        a += 3.1;
//                    }
//                }
//                ,
//                &d,
//                sizeof( d )
//            )
//            ;
//        }
//        cout << c << "\n";
//    }
//
//    manager -> wait();
//    manager -> destroy();
//    app -> destroy();

    BrainApplication::create( argc, argv )
    -> run()
    -> destroy();

    return 0;
}
