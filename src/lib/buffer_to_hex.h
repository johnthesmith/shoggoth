#pragma once

#include <iostream>
#include <string>



using namespace std;


/*
    Conver buffer to string
*/

string bufferToHex
(
    const void*,              /* aBuffer */
    const size_t,             /* aSize */
    const string      = " ",  /* aHexDelimiter */
    const string      = "\n", /* aLineDelimiter */
    const string      = " | ",  /* aGroupDelimiter */
    const short int   = 16,   /* aLineWidth = 16 */
    const short int   = 4     /* aGroupWidth = 16 */
);

