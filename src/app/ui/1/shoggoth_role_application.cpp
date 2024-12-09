/* System libraries */
#include <iostream>



/* Libraryes */
#include "../../../../lib/core/utils.h"
#include "../../../../lib/core/log.h"
#include "../../../../lib/json/param_list.h"
#include "../../../../lib/core/buffer_to_hex.h"

/* Application libraryes */
#include "shoggoth_application.h"
#include "loop.h"



using namespace std;



ShoggothRoleApplication::ShoggothRoleApplication
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
}



/*
    Creator
*/
ShoggothRoleApplication* ShoggothRoleApplication::create
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
{
    return new ShoggothRoleApplication( aCount, aList );
}



/*
    Return Role from string
*/
Role ShoggothRoleApplication::roleFromString
(
    string aRole
)
{
    if( aRole == roleToString( ROLE_TEACHER ))  return ROLE_TEACHER;
    if( aRole == roleToString( ROLE_UI ))       return ROLE_UI;
    if( aRole == roleToString( ROLE_SERVER ))   return ROLE_SERVER;
    return ROLE_PROCESSOR;
}



string ShoggothRoleApplication::roleToString
(
    Role aRole
)
{
    switch( aRole )
    {
        case ROLE_TEACHER   : return "teacher";
        case ROLE_UI        : return "ui";
        case ROLE_SERVER    : return "server";
        default:
        case ROLE_PROCESSOR : return "processor";
    }
}



/*
    Run application
*/
ShoggothRoleApplication* ShoggothRoleApplication::run()
{
    getLog() -> begin( "Start the Shoggoth role application" );

    prepareConfiguration();

    /* Main loop */
    while( true )
    {
        Loop::create( this, getNetId(), getNetVersion() )
        -> resume()
        -> loop()
        -> destroy();
    }

    getLog() -> end( "Application stop" );

    return this;
}
