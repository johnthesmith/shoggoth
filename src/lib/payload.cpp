/* Default libraries */
#include <iostream>
#include <unistd.h>         /* usleep */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/* Local libraries */
#include "payload.h"
#include "rnd.h"



using namespace std;


/*
    Constructor of paylod
*/
Payload::Payload
(
    Application* aApplication
)
{
    application = aApplication;
    getLog() -> trace( "Create payload" );
}



/*
    Destructor of payload
*/
Payload::~Payload()
{
    if( threadObject != NULL )
    {
        terminate();
        threadObject -> join();
        delete threadObject;
    }
    getLog() -> trace( "Destroy payload" );
}



/*
    Creator
*/
Payload* Payload::create
(
    Application* aApplication
)
{
    return new Payload( aApplication );
}



/*
    Destructor
*/
void Payload::destroy()
{
    delete this;
}



/*
    Get application log or personal payloads log if exists
*/
Log* Payload::getLog()
{
    return log == NULL ? application -> getLog() : log;
}



/*
    Get application object
*/
Application* Payload::getApplication()
{
    return application;
}



/*
    Run payload
*/
Payload* Payload::run
(
    bool aThread    /* True for run like thread */
)
{
    if( aThread )
    {
        /* Run loop in the personal thread if it does not early */
        if( threadObject == NULL )
        {
            log = Log::create()
            -> clone( application -> getLog() )
            -> setFileName(( id == "" ? Rnd::getUuid() : id ) + ".log" )
            -> begin( "" ) -> lineEnd();

            threadObject = new thread
            (
                [ this ]
                ()
                {
                    onRun();
                    log -> end() -> close() -> destroy();
                    log = NULL;
                }
            );
        }
        else
        {
            application -> getLog() -> warning( "thread already runing" );
        }
    }
    else
    {
        onRun();
    }
    return this;
}



/*
    Mail loop
*/
Payload* Payload::loop
(
    bool aThread    /* True for run like thread */
)
{
    if( aThread )
    {
        /* Run loop in the personal thread if it does not early */
        if( threadObject == NULL )
        {
            threadObject = new thread
            (
                [ this ]
                ()
                {
                    /* Log create and registration */
                    log = application -> createThreadLog()
                    -> clone( application -> getLog() )
                    -> setFileName(( id == "" ? Rnd::getUuid() : id ) + ".log" );

                    /* Run loop */
                    internalLoop();

                    /* Destroy and nullate log */
                    application -> destroyThreadLog();
                    log = NULL;
                }
            );
        }
        else
        {
            application -> getLog() -> warning( "thread already runing" );
        }
    }
    else
    {
        /* Run loop in the parent thread */
        internalLoop();
    }

    return this;
}



/*
    Internal loop emplimentation
    This method calls a user onLoop
*/
Payload* Payload::internalLoop()
{
    terminated  = false;

    bool            idling      = true;
    bool            reconfig    = true;
    unsigned int    sleep       = 0;

    while( !terminated )
    {
        if( reconfig )
        {
            sleep = getApplication()
            -> getConfig()
            -> getInt( "loop_sleep_mcs", 1000000 );
            reconfig = false;
        }

        onLoop( terminated, idling, sleep, reconfig );

        if( idling )
        {
            usleep( sleep );
        }
    }

    return this;
}



/*
    Set terminate flag
*/
Payload* Payload::terminate()
{
    terminated = true;
    return this;
}



/******************************************************************************
    Events
*/


/*
    Loop for payload
*/
void Payload::onLoop
(
    bool&           aTreminated,
    bool&           aIdling,
    unsigned int&   aSleep,
    bool&           aReconfig
)
{
    getLog() -> trace( "Paylaod default on loop event" );
}



/*
    On Run event
*/
void Payload::onRun()
{
    getLog() -> trace( "Run" );
}


/******************************************************************************
    Setters and getters
*/


/*
    Set payload id
*/
Payload* Payload::setId
(
    string aId
)
{
    id = aId;
    return this;
}




/*
    Set order for pause
*/
Payload* Payload::setPause
(
    bool a
)
{
    pause = a;
    return this;
}



/*
    Get order for pause
*/
bool Payload::getPause()
{
    return pause;
}



/*
    Set paused confirmation
*/
Payload* Payload::setPaused
(
    bool a
)
{
    paused = a;
    return this;
}



/*
    Get paused confirmation
*/
bool Payload::getPaused()
{
    return paused;
}
