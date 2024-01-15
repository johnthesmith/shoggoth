#include <iostream>
#include <cmath>
#include "rnd.h"


unsigned long long Rnd::seed = 0;
unsigned long long Rnd::storedSeed = 0;


/*
    Begin randomize from current time
*/
void Rnd::randomize()
{
    timeval currentTime;
    gettimeofday( &currentTime, 0 );
    seed = ( currentTime.tv_usec );
}



/*
    Store seed
*/
void Rnd::storeSeed
(
    unsigned long long aSeed
)
{
    storedSeed = seed;
    seed = aSeed;
}



void Rnd::restoreSeed()
{
    seed = storedSeed;
}



/*
    Get random value
*/
void Rnd::calcSeed()
{
    seed = (seed * 71103515245 + 17344371) & 0xFFFFFFFFFFFF;
}



/*
    Get random value
*/
double Rnd::get()
{
    calcSeed();
    return (double) seed / (double) 0x1000000000000;
}



/*
    Get random value from AMin to AMax
*/
int Rnd::get
(
    int aMin,
    int aMax
)
{
    return aMin + floor( get() * ( aMax + 1  - aMin ));
}




/*
    Get random value from AMin to AMax
*/
unsigned int Rnd::get
(
    unsigned int aMin,
    unsigned int aMax
)
{
    return aMin + floor( get() * ( aMax + 1  - aMin ));
}



/*
    Get random value from AMin=0 to AMax=0xFFFF FFFF FFFF
*/
unsigned long long Rnd::get
(
    unsigned long long aMin,
    unsigned long long aMax
)
{
    return aMin + floor( get() * ( aMax + 1  - aMin ));
}



/*
    Get random value from AMin to AMax
*/
double Rnd::get
(
    double aMin,
    double aMax
)
{
    return aMin + floor( get() * ( aMax + 1  - aMin ));
}



/*
    Return uuid
*/
string Rnd::getUuid()
{
    char buffer[37];
    sprintf(buffer, "%08x-%04x-%04x-%04x-%04x%08x",
            get((unsigned int)0, (unsigned int)0xFFFFFFFF),
            get(0, 0xFFFF),
            ((get(0, 0xFFFF) & 0x0fff) | 0x4000),
            get(0, 0xFFFF) % 0x3fff + 0x8000,
            get((unsigned int) 0, (unsigned int) 0xFFFF),
            get((unsigned int) 0, (unsigned int) 0xFFFFFFFF));
            string result( buffer );
    return result;
}



/*
    Return uuid by seed
*/
string Rnd::getUuid
(
    unsigned long long aSeed    /* Seed for current UUID */
)
{
    string result;
    storeSeed( aSeed );
    result = getUuid();
    restoreSeed();
    return result;
}



/*
    возвращает необходимость исолнения вероятного события
*/
bool Rnd::chanceByTime
(
    long aInterval,
    long aFrequancy
)
{
    double r = get();
    double c = (double) aFrequancy / (double) aInterval;
    return r < c;
}

