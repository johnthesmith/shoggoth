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
