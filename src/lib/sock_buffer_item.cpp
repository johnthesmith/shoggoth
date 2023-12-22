#include "sock_buffer_item.h"



/*
    Constructor
*/
SockBufferItem::SockBufferItem
(
    unsigned int aSize   /* Real size */
)
{
    realSize = aSize;
    pointer = new char[ realSize ];
}



/*
    Destructor
*/
SockBufferItem::~SockBufferItem()
{
    delete [] pointer;
}



/*
    Create item
*/
SockBufferItem* SockBufferItem::create
(
    unsigned int a  /* Size of item */
)
{
    return new SockBufferItem( a );
}



/*
    Destroy item
*/
void SockBufferItem::destroy()
{
    delete this;
}



/*
    Return Pointer
*/
char* SockBufferItem::getPointer()
{
    return pointer;
}



/*
    Count of readed bytes
*/
SockBufferItem* SockBufferItem::setReadSize
(
    unsigned int a
)
{
    readSize = a;
    return this;
}



/*
    Return the readed bytes
*/
unsigned int SockBufferItem::getReadSize()
{
    return readSize;
}



/*
    Return the real size
*/
unsigned int SockBufferItem::getRealSize()
{
    return realSize;
}
