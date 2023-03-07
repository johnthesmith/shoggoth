#include <time.h>
#include <iostream>
#include <string>
#include <cstring>

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




bool stringToBool
(
    string a
)
{
    return a == "true" || a == "TRUE";
}



long long int stringToInt
(
    string a
)
{
    return atol( a.c_str() );
}



double stringToDouble
(
    string a
)
{
    return atof( a.c_str() );
}

