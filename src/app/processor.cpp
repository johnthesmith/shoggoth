#include <iostream>

/* User libraries */
#include "processor.h"




using namespace std;



/*
    Constructor
*/
Processor::Processor
(
    Net* aNet
)
/* Call parent constructor */
: Payload( aNet -> getApplication() )
{
    getLog() -> trace( "Create processor" );
    limb = LimbProcessor::create( aNet );
}





/*
    Destructor
*/
Processor::~Processor()
{
    limb -> destroy();
    getLog() -> trace( "Processor destroed" );
}



/*
    Creator
*/
Processor* Processor::create
(
    Net* a
)
{
    return new Processor( a );
}



/*
    Destructor
*/
void Processor::destroy()
{
    delete this;
}



ShoggothApplication* Processor::getApplication()
{
    return ( ShoggothApplication* ) Payload::getApplication();
}



/******************************************************************************
    Methods
*/



/*
    Return limb
*/
LimbProcessor* Processor::getLimb()
{
    return limb;
}




/******************************************************************************
    Events
*/



/*
    Net calculateion
*/
void Processor::onLoop()
{
    limb -> calc();
}
