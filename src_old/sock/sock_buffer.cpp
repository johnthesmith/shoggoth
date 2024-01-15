#include <iostream>
#include <cstring>

#include "sock_buffer.h"



/*
    Constructor
*/
SockBuffer::SockBuffer()
{
};



/*
    Destructor
*/
SockBuffer::~SockBuffer()
{
    destroyResultBuffer();
    clear();
}



/*
    Create buffer
*/
SockBuffer* SockBuffer::create()
{
    return new SockBuffer();
}



/*
    Destroy buffer
*/
void SockBuffer::destroy()
{
    delete this;
}



/*
    Clear list of items
*/
SockBuffer* SockBuffer::clear()
{
    for( auto item:items )
    {
        item -> destroy();
    }
    items.clear();
    return this;
}



/*
    Create and return new buffer
*/
SockBufferItem* SockBuffer::add
(
    unsigned int aSize   /* Size of buffer */
)
{
    resultBufferBuilded = false;
    auto result = SockBufferItem::create( aSize );
    items.push_back( result );
    return result;
}



SockBuffer* SockBuffer::buildResultBuffer()
{
    if( !resultBufferBuilded )
    {
        destroyResultBuffer();

        /* Calculate size of buffer */
        resultBufferSize = calcReadSize();

        /* Create result buffer */
        resultBuffer = new char[ resultBufferSize ];

        /* Collect and Destroy buffers */
        unsigned int collectedSize = 0;

        for( auto item:items )
        {

            memcpy
            (
                &resultBuffer[ collectedSize ],
                item -> getPointer(),
                item -> getReadSize()
            );
            collectedSize += item -> getReadSize();
        }

        resultBufferBuilded = true;
    }

    return this;
}



/*
    Destroy result buffer
*/
SockBuffer* SockBuffer::destroyResultBuffer()
{
    /**/
    if( resultBuffer != NULL )
    {
        delete [] resultBuffer;
        resultBuffer        = NULL;
        resultBufferSize    = 0;
   }
    return this;
}



/*
    Return first element of buffer
*/
char* SockBuffer::getBuffer()
{
    buildResultBuffer();
    return resultBuffer;
}



/*
    Return buffer size
*/
unsigned int SockBuffer::getBufferSize()
{
    buildResultBuffer();
    return resultBufferSize;
}



/*
    Return buffer like string
*/
string SockBuffer::getString()
{
    buildResultBuffer();
    string result( resultBuffer );
    return result;
}



/*
    Calculate size of buffer
*/
unsigned int SockBuffer::calcReadSize()
{
    unsigned int result = 0;
    for( auto item:items )
    {
        result += item -> getReadSize();
    }
    return result;
}



/*
    Return first buffer
*/
SockBufferItem* SockBuffer::getFirstBuffer()
{
    return items.front();
}
