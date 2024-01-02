/*
    Object for sock handles manager This object can be created at main application and
    passed to Sock object constructor. Each sock uses this object to storing handles.
    Main thread and children thread may create many Socks, but all Socks will use
    single sock handle. This prevents the multiple WAIT_TIMOUT states for a port with
    large number of requests.
*/

#pragma once


/*
    Sys libraries
*/

#include <string>
#include <map>
#include <mutex>



using namespace std;



class SockManager
{
    private:
        /* Main mutex for synchronizing handles */
        mutex sync;

        /* List of handles for threads */
        map
        <
            string,         /* thread_id@user_id */
            int             /* sock handle */
        >
        handles;

        /*
            Return thread id for current payload
        */
        string getId
        (
            string /* User id */
        );

    public:

        /**********************************************************************
            Sock operations
        */


        /*
            Sock manager  destructor
        */
        ~SockManager();



        /*
            Sock manager creator
        */
        static SockManager* create();



        /*
            Sock manager destroy
        */
        void destroy();



        /*
            Add handle for sock manager
            Unsafe! Do not  use it. Only for Sock.
        */
        SockManager* addHandle
        (
            string, /* User id */
            int     /* Handle */
        );



        /*
            Return handle by thread and user id
        */
        int getHandle
        (
            string  = "defaul" /* User id */
        );



        /*
            Close and destroy sock
        */
        SockManager* closeHandlesByThread
        (
            string = ""
        );
};
