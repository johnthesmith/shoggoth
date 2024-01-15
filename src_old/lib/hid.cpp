#include "hid.h"
#include "utils.h"



/*
    Hid from string
*/
Hid& Hid::setString
(
    string a
)
{
    MD5( a ).toBuffer( data );
    return *this;
}



/*
    Fill hid from GUID
*/
Hid& Hid::newGuid()
{
    *(( unsigned int* ) &data[0])       = Rnd::get((unsigned int)0, (unsigned int)0xFFFFFFFF);
    *(( unsigned short* ) &data[4])     = Rnd::get(0, 0xFFFF);
    *(( unsigned short* ) &data[6])     = Rnd::get(0, 0xFFFF);
    *(( unsigned short* ) &data[8])     = Rnd::get(0, 0xFFFF);
    *(( unsigned short* ) &data[10])    = Rnd::get(0, 0xFFFF);
    *(( unsigned int* ) &data[12])      = Rnd::get((unsigned int) 0, (unsigned int) 0xFFFFFFFF);
    return *this;
}



/*
    Get bit true or falce
*/
bool Hid::getBit
(
    int a
)
{
    auto i = 15 - (a / 8);
    auto b = a % 8;
    return
    ( a >= 0 && a < 128 ) &&
    ( data[ i ] & ( 1 << b )) != 0;
}



/*
    Return hex notation
*/
string Hid::getHex()
{
    char buffer[37];

    for( int i = 0; i < 16; i++ )
    {
        sprintf(buffer + i * 2, "%02x", data[i]);
    }

    string result( buffer );
    return result;
}



/*
    Return uuid notation
*/
string Hid::getUuid()
{
    char buffer[37];
    sprintf
    (
        buffer,
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ],
        data[ 4 ], data[ 5 ],
        data[ 6 ], data[ 7 ],
        data[ 8 ], data[ 9 ],
        data[ 10 ], data[ 11 ], data[ 12 ], data[ 13 ], data[ 14 ], data[ 15 ]
    );

    string result( buffer );
    return result;
}


/*
    Set uuid from string
*/
Hid& Hid::setUuid
(
    string a
)
{
    auto s = implode( explode( a, "-" ), "" );
    int l = s.size() / 2;

    for( int i = 0; i < l; i++ )
    {
        data[ i ] = hexToInt( s.substr( i*2, 2 ));
    }
    return *this;
}


