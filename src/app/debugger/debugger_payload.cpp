/*
    Debugger payload
*/


/* Vanilla libraris */
#include <iostream>
#include <cmath>

/* Shoggoth libraries */
#include "../../../../../lib/core/str.h"
#include "../../../../../lib/core/utils.h"

/* User libraries */
#include "debugger_application.h"
#include "debugger_consts.h"
#include "debugger_payload.h"


using namespace std;



/*
    Constructor
*/
DebuggerPayload::DebuggerPayload
(
    DebuggerApplication* aApplication,
    string aPayloadId,
    string aNetId,
    string aNetVersion
)
: PayloadEngine( aApplication, aPayloadId ) /* Call parent constructor */
{
//    -> getTerminal()
//    -> addColor( "header", COLOR_WHITE, COLOR_BLUE )
//
//    -> addWindow( "header", 0, 0, 1, 1 )
//    -> window( "header" )
//    -> color( "header" )
//    -> setColorDefault()
//
//    -> addWindow( "bottom", 0, 0, 1, 1 )
//    -> window( "bottom" )
//    -> color( "header" )
//    -> setColorDefault()
//
//    -> cursor( 0 )
//    ;
}



/*
    Destructor
*/
DebuggerPayload::~DebuggerPayload()
{
    net -> destroy();
}



/*
    Creator
*/
DebuggerPayload* DebuggerPayload::create
(
    DebuggerApplication* a,
    string aPayloadId,
    string aNetId,
    string aNetVersion
)
{
    return new DebuggerPayload( a, aPayloadId, aNetId, aNetVersion );
}



/*
    Destructor
*/
void DebuggerPayload::destroy()
{
    delete this;
}



/*
    Return application object
*/
DebuggerApplication* DebuggerPayload::getApplication()
{
    return ( DebuggerApplication* ) Payload::getApplication();
}



/******************************************************************************
    Payload events
*/



/*
    Main loop event
*/
void DebuggerPayload::onEngineLoop
(
    bool
)
{
    auto terminal = getApplication() -> getTerminal();

    auto key = terminal -> readKey();

    if( key != 0 )
    {
        terminal
        -> window()
        -> text( toString( key, false ) + " | ");
    }

    terminal
    -> window( "header" )
    -> clear()
    -> alignWindow( TOP )
    -> moveTo( 0, 0 )
    -> text( " Shoggoth debugger | connected 192.168.0.1 | tick #3444 | pause | " )

    -> window( "bottom" )
    -> clear()
    -> alignWindow( BOTTOM )
    -> moveTo( 0, 0 )
    -> text( " Layers Nerves Weights Stop/Start " )

    /* Финализация вывода */
    -> window()
    -> refresh()
    -> window( "header" )
    -> refresh()
    -> window( "bottom" )
    -> refresh()
    -> finish()
    ;
}
