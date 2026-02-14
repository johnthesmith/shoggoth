/* System libraries */
#include <iostream>

/* Application libraryes */
#include "ui_application.h"
#include "ui_payload.h"



using namespace std;



UiApplication::UiApplication
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
    getLog() -> trace( "Ui constructor" );
}



/*
    Creator
*/
UiApplication* UiApplication::create
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
{
    return new UiApplication( aCount, aList );
}



/*
    Run application
*/
UiApplication* UiApplication::run()
{
    getLog() -> begin( "Start the Shoggoth Ui application" );

    prepareConfiguration();

//    UiPayload::create( this, "ui", getNetId(), getNetVersion() )
//    -> start()
//    -> destroy();

    getLog() -> end( "Application stop" );

    return this;
}

