/* Default libraries */
#include <iostream>
#include <unistd.h>         /* usleep */

/* Local libraries */
#include "payload.h"


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


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
    Get scene value
*/
Log* Payload::getLog()
{
    return application -> getLog();
}



/*
    Get application object
*/
Application* Payload::getApplication()
{
    return application;
}



//void term_handler(int i)
//{
//    cout << "Terminating\n";
//}



Payload* Payload::loop()
{
    bool            terminated  = false;
    bool            idling      = true;
    bool            reconfig    = true;
    unsigned int    sleep       = 0;

//    struct sigaction sa;
//    sigset_t newset;
//    sigemptyset(&newset);                   // чистимся
//    sigaddset( &newset, SIGINT );           // добавляем сигнал SIGHUP
//    sigprocmask(SIG_BLOCK, &newset, 0);     // блокируем его
//    sa.sa_handler = term_handler;           // указываем обработчик
//    sigaction(  SIGINT, &sa, 0);            // обрабатываем сигнал SIGTERM

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
    getLog() -> trace( "Loop" );
}





