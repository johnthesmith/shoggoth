#include <iostream>
#include <cstring>

#include "neuron_list.h"
#include "neuron.h"
#include "../lib/utils.h"

using namespace std;



/*
    Create and return list of neurons
*/
NeuronList* NeuronList::create()
{
    return new NeuronList();
}




/*
    Add neurons from argument list to this list
*/
NeuronList* NeuronList::add
(
    NeuronList* a
)
{
    Heap::add(( Heap* ) a );

    return this;
}




/*
    Return index by neuron
*/
int NeuronList::indexBy
(
    Neuron* a
)
{
    return Heap::indexBy(( void* ) a );
}




/*
    Return index by neuron
*/
Neuron* NeuronList::getByIndex
(
    int a
)
{
    return ( Neuron* )Heap::getByIndex( a );
}




/*
    Set value by index
*/
NeuronList* NeuronList::setByIndex
(
    int aIndex,
    Neuron* aNeuron
)
{
    Heap::setByIndex(aIndex, ( void* ) aNeuron );
    return this;
}



/*
    Resize
*/
NeuronList* NeuronList::resize
(
    int a
)
{
    Heap::resize( a );
    return this;
}



/*
    Loop with lyambda
*/
NeuronList* NeuronList::loop
(
    function <bool ( Neuron* )> callback,
    int aFrom,
    int aTo
)
{
    bool stop = false;
    auto c = aTo == 0 ? getCount() : aTo;
    auto items = getItems();

    for( int i = aFrom; i < c && !stop; i++ )
    {
        stop = callback( (Neuron*) items[ i ] );
    }
    return this;
}



/******************************************************************************
    Storage utils
*/



/*
    Return storage size for neuron list
*/
int NeuronList::getStorageSize()
{
    return sizeof( StorageNeuron ) * getCount();
}



bool NeuronList::checkStorage
(
    string aFilePath
)
{
    return fileSize( aFilePath ) == getStorageSize();
}



/*
    Create storage if it has an invalid size
*/
NeuronList* NeuronList::createStorage
(
    string aFilePath
)
{
    if( aFilePath != "" )
    {
        if( checkPath( getPath( aFilePath )))
        {
            auto size = getStorageSize();
            if( fileSize( aFilePath ) != size )
            {
                auto h = fopen( aFilePath.c_str(), "w+" );
                if( h )
                {
                    fseek( h, size - 1, SEEK_SET );
                    fputc( '\0', h );
                    fclose( h );
                }
            }
        }
    }
    return this;
}



/*
    Save neurons to memory buffer
    Buffer must be delete after using with deleteBuffer
*/
NeuronList* NeuronList::writeToBuffer
(
    char* &aBuffer,
    size_t &aSize
)
{
    if( aBuffer == NULL )
    {
        int sizeOfPart = sizeof( StorageNeuron );
        size_t pos = 0;
        aSize = sizeof( StorageNeuron ) * getCount();
        aBuffer = new char[ aSize ];

        loop
        (
            [ this, &aBuffer, &pos, sizeOfPart ]
            (
                Neuron* iNeuron
            )
            {
                StorageNeuron part;
                part.value = iNeuron -> getValue();
                part.error = iNeuron -> getValue();

                memcpy( &aBuffer[ pos ], &part, sizeOfPart );
                pos += sizeOfPart;

                return false;
            }
        );
    }
    else
    {
        setResult( "BufferIsNotEmpty" );
    }
    return this;
}



/*
    Load neurons from memory buffer
*/
NeuronList* NeuronList::readFromBuffer
(
    const char* aBuffer,
    const size_t aSize
)
{
    size_t size = sizeof( StorageNeuron );

    if( size * getCount()  == aSize )
    {
        size_t pos = 0;
        loop
        (
            [ this, &pos, &size, &aBuffer ]
            (
                Neuron* iNeuron
            )
            {
                StorageNeuron part;

                memcpy( &part, &aBuffer[ pos ], size );

                iNeuron -> setValue( part.value );
                iNeuron -> setError( part.error );

                pos += size;

                return false;
            }
        );
    }
    else
    {
        this -> setCode( "NeuronsCountNotMatchingDataSize" );
    }

    return this;
}
