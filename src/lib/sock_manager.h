#pragma once

#include <string>
#include <map>

#include "sock.h"


using namespace std;



class SockManager
{
    private:

        /* List of handles for threads */
        map
        <
            string,         /* Full sock threadId@sockId */
            Sock*           /* Sock */
        >
        handles;


        /*
            Return thread id for current payload
        */
        string getId
        (
            string /* Sock id */
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
            Create new sock or treturn exists by id
        */
        Sock* getSock
        (
            string          = "defaul", /* Sock id  from user */
            SocketDomain    = SD_INET,
            SocketType      = ST_TCP
        );



        /*
            Close and destroy sock
        */
        SockManager* closeSock
        (
            string  = "default" /* Handle id */
        );


        /*
            Return sock object by user id for thread
        */
        Sock* findSock
        (
            string /* Sock id */
        );
};
