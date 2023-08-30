/*
    Socket buffer
    Contains list of readed items
*/


#include "string"
#include <vector>

#include "sock_buffer_item.h"


using namespace std;


class SockBuffer
{
    private:

        vector <SockBufferItem*>    items;
        char*                       resultBuffer        = NULL;
        unsigned int                resultBufferSize    = 0;
        bool                        resultBufferBuilded = false;

        /*
            Build resuld buffer before request buffer
        */
        SockBuffer* buildResultBuffer();

        /*
            Destroy result buffer
        */
        SockBuffer* destroyResultBuffer();

    public:
        /*
            Constructor
        */
        SockBuffer();



        /*
            Destructor
        */
        ~SockBuffer();



        /*
            Create buffer
        */
        static SockBuffer* create();



        /*
            Destroy buffer
        */
        void destroy();



        /*
            Clear list of items
        */
        SockBuffer* clear();



        /*
            Create and return new buffer
        */
        SockBufferItem* add
        (
            unsigned int    /* Size of buffer */
        );



        /*
            Return first element of buffer
        */
        char* getBuffer();



        /*
            Return size of buffer
        */
        unsigned int getBufferSize();



        /*
            Return buffer as string
        */
        string getString();



        /*
            Calculate size of buffer
        */
        unsigned int calcReadSize();


        /*
            Return first buffer
        */
        SockBufferItem* getFirstBuffer();

};
