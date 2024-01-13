/*
    Sys libaries
*/
#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>
#include <unistd.h>

/*
    Local libraries
*/
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
SockManager* SockManager::addHandle
(
    string aUserId, /* User id */
    int    aHandle  /* Handle */
)
{
    auto id = getId( aUserId );
    sync.lock();
    handles.insert_or_assign( id, aHandle );
    sync.unlock();
    return this;
}



/*
    Create new Sock or exists Sock return
*/
int SockManager::getHandle
(
    string aUserId  /* User id */
)
{
    /* Find the Sock by sock id */
    auto id = getId( aUserId );
    sync.lock();
    auto result = handles.find( id ) != handles.end()
    ? handles[ id ]
    : -1;
    sync.unlock();
    return result;
}



/*
    Close and destroy Sock by id
*/
SockManager* SockManager::closeHandlesByThread
(
    string aId
)
{
    auto mask =  getId( aId );

    map
    <
        string,
        int
    >
    save;

    sync.lock();
    {
        for( const auto&[ id, handle ] : handles )
        {
            if( id.find( mask ) != string::npos )
            {
                close( handle );
            }
            else
            {
                save.insert({ id, handle });
            }
        }

        handles = save;
    }
    sync.unlock();

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
