#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>

#include "sock_manager.h"



/*
    Sock manager destructor
*/
SockManager::~SockManager()
{
    for( auto item : handles )
    {
        item.second
        -> closeHandle()
        -> destroy();
    }
}



/*
    Sock manager creator
*/
SockManager* SockManager::create()
{
    return new SockManager();
}



/*
    Sock manager destroy
*/
void SockManager::destroy()
{
    delete this;
}




/*
    Create new Sock or exists Sock return
*/
Sock* SockManager::getSock
(
    string          aSockId,   /* Sock id from user */
    SocketDomain    aDomain,
    SocketType      aType
)
{
    /* Find the Sock by sock id */
    auto id = getId( aSockId );
    Sock* result = findSock( id );

    if( result == NULL )
    {
        /* Create and return new sock */
        result
        = Sock::create()
        -> openHandle( aDomain, aType );
        /* Registrate in list of the sock handles */
        handles[ id ] = result;
    }

    return result;
}



/*
    Close and destroy Sock by id
*/
SockManager* SockManager::closeSock
(
    string  aSockId /* Sock id */
)
{
    auto id = getId( aSockId );
    auto result = getSock( id );

    if( result != NULL )
    {
        handles[ id ] -> destroy();
        handles.erase( id );
    }
    return this;
}



/*
    Return sock object by user id for thread
*/
Sock* SockManager::findSock
(
    string  aId /* Sock id */
)
{
    return
    handles.find( aId ) != handles.end()
    ? handles[ aId ]
    : NULL;
}



/*
    Return id for current thread and user name
*/
string SockManager::getId
(
    string aSockId
)
{
    /* Get thread id */
    auto threadId = this_thread::get_id();
    /* Build id */
    stringstream s;
    s << threadId << "@" << aSockId;
    return s.str();
}
