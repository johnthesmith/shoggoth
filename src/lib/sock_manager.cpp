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
    /* Close all socks for SockManager */
    for( auto item : handles )
    {
        /* Close handle for sock */
        close( item.second );
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
    Add handle for sock manager
    Unsafe! Do not  use it. Only for Sock.
*/
SockManager* addHandle
(
    string aUserId, /* User id */
    int    aHandle  /* Handle */
)
{
    handles[ getId( aUserId )] = aHandle;
    return this;
}



/*
    Create new Sock or exists Sock return
*/
int SockManager::getHandle
(
    string aUserId,   /* User id */
)
{
    /* Find the Sock by sock id */
    auto id = getId( aSockId );
    return handles.find( id ) != handles.end()
    ? handles[ aId ]
    : -1;
}



/*
    Close and destroy Sock by id
*/
SockManager* SockManager::closeHandlesByThread()
{
    auto mask =  getId( "" );
    for( const auto&[ id, handle ] : handles )
    {
        if( id.find( mask ) != string::npos )
        {
            close( item.handle );
        }
    }
    return this;
}



/*
    Return id for current thread and user name
*/
string SockManager::getId
(
    string aUserId
)
{
    /* Get thread id */
    auto threadId = this_thread::get_id();
    /* Build id */
    stringstream s;
    s << threadId << "@" << aUserId;
    return s.str();
}
