/* Default libraries */
#include <iostream>

/* Local libraries */
#include "payload.h"



using namespace std;


/*
    Constructor of paylod
*/
Payload::Payload
(
    Log& aLog
):
log( aLog ) /* define the prived log from aLog argument */
{
    getLog().trace( "Create payload" );
}



/*
    Destructor of payload
*/
Payload::~Payload()
{
    getLog().trace( "Destroy payload" );
}



/*
    Creator
*/
Payload* Payload::create
(
    Log& aLog
)
{
    return new Payload( aLog );
}



/*
    Destructor
*/
void Payload::destroy()
{
    delete this;
}



/*
    Get scene value
*/
Log& Payload::getLog()
{
    return log;
}
