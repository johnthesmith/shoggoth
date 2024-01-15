#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>

#include "log_manager.h"



/*
    Log manager constructor
*/
LogManager::LogManager
(
    Log* aLog    /* Default log */
)
{
    log = aLog;
}



/*
    Log manager  destructor
*/
LogManager::~LogManager()
{
    for( auto item : logList )
    {
        item.second
        -> close()
        -> destroy();
    }
}



/*
    Log manager creator
*/
LogManager* LogManager::create
(
    Log* aLog   /* Default log */
)
{
    return new LogManager( aLog );
}



/*
    Log manager destroy
*/
void LogManager::destroy()
{
    delete this;
}




/*
    Create new log
*/
Log* LogManager::createLog
(
    string aId
)
{
    auto threadId = getThreadId();
    auto result = getLog();

    if( result == log && threadId != "" )
    {
        /* Create and return new log */
        result
        = Log::create()
        -> clone( log )
        -> setFileName( aId + ".log" )
        -> trace( "Thread log started" )
        -> prm( "log id", aId )
        -> prm( "thread id", threadId )
        -> lineEnd();
        /* Registrate in list of the logs */
        logList[ threadId ] = result;
    }
    return result;
}



/*
    Destroy log by id
*/
LogManager* LogManager::destroyLog()
{
    auto threadId = getThreadId();

    auto result = getLog();
    if( result != log )
    {
        logList[ threadId ] -> destroy();
        logList.erase( threadId );
    }
    return this;
}




/*
    Return log object by id log
    or default log application
*/
Log* LogManager::getLog()
{
    auto threadId = getThreadId();
    return
    threadId != "" && logList.find( threadId ) != logList.end()
    ? logList[ threadId ]
    : log;
}



/*
    Return thread id for current payload
*/
string LogManager::getThreadId()
{
    auto threadId = this_thread::get_id();
    stringstream s;
    s << threadId;
    return s.str();
}
