#include <cstddef>

/*
    Part of socket buffer after read
*/

class SockBufferItem
{
    private:

        char*           pointer     = NULL;
        unsigned int    realSize    = 0;        /* */
        unsigned int    readSize    = 0;

    public:

        /*
            Constructor
        */
        SockBufferItem
        (
            unsigned int    /* Real size */
        );



        /*
            Destructor
        */
        ~SockBufferItem();



        /*
            Create item
        */
        static SockBufferItem* create
        (
            unsigned int
        );



        /*
            Destroy item
        */
        void destroy();



        /*
            Return Pointer
        */
        char* getPointer();

        /*
            Count of readed bytes
        */
        SockBufferItem* setReadSize
        (
            unsigned int
        );



        /*
            Return the readed bytes
        */
        unsigned int getReadSize();



        /*
            Return the real size
        */
        unsigned int getRealSize();

};

