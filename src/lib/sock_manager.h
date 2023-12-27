#pragma once

#include <string>
#include <map>



using namespace std;



class SockManager
{
    private:

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
        SockManager* closeHandlesByThread();
};
