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

    TeacherPayload::create( this,  getNetId(), getNetVersion() )
    -> resume()
//    -> loop()
    -> destroy();

    getLog() -> end( "Application stop" );

    return this;
}

