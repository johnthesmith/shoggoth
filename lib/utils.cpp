#include <time.h>
#include <iostream>
#include <string>

using namespace std;



/*
    Return now value in microseconds
*/
long long now()
{
    struct timespec Current;
    return
    clock_gettime( CLOCK_REALTIME, &Current ) == 0
    ? Current.tv_sec * 1000000 + Current.tv_nsec / 1000
    : 0;
}



uint32_t rand32()
{
    return ((rand() & 0x3) << 30) | ((rand() & 0x7fff) << 15) | (rand() & 0x7fff);
}


/*
    Return UUID
*/
string getUuid()
{
    char dst[37];
    snprintf
    (
        dst,
        37,
        "%08x-%04x-%04x-%04x-%04x%08x",
        rand32(),                       /* Generates a 32-bit Hex number */
        rand32() & 0xffff,              /* Generates a 16-bit Hex number */
        ((rand32() & 0x0fff) | 0x4000), /* Generates a 16-bit Hex number of the form 4xxx (4 indicates the UUID version) */
        (rand32() & 0x3fff) + 0x8000,   /* Generates a 16-bit Hex number in the range [0x8000, 0xbfff] */
        rand32() & 0xffff, rand32()     /* Generates a 48-bit Hex number */
    );
    return string( dst );
}
