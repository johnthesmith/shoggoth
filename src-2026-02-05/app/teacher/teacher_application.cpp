/* System libraries */
#include <iostream>

/* Application libraryes */
#include "teacher_application.h"
#include "teacher_payload.h"



using namespace std;


/*
    Teacher constructor
*/
TeacherApplication::TeacherApplication
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
:ShoggothApplication
(
    aCount,
    aList
)
{
    getLog() -> trace( "Teacher constructor" );

    registerSignal(SIGINT);
    registerSignal(SIGTERM);
    registerSignal(SIGSEGV);
    registerSignal(SIGQUIT);
}



/*
    Create and return teacher application
*/
TeacherApplication* TeacherApplication::create
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
{
    return new TeacherApplication( aCount, aList );
}



/*
    Run application
*/
TeacherApplication* TeacherApplication::run()
{
    getLog() -> begin( "Start the Shoggoth Teacher application" );

    prepareConfiguration();

    payload = TeacherPayload::create( this, "teacher", "", getNetVersion() );

    payload
    -> setLoopTimeoutMcs(0)
    -> start()
    -> destroy();

    getLog() -> end( "Application stop" );

    return this;
}




/*
    on signale event handler
*/
bool TeacherApplication::onSignal
(
    int aSignal
)
{
    payload -> stop();
    return true;
}
