#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>

using namespace std;


/*
    List of types
*/
enum ParamType
{
    KT_UNKNOWN, /* 0 */
    KT_BOOL,    /* 1 */
    KT_STRING,  /* 2 */
    KT_DATA,
    KT_INT,
    KT_DOUBLE,
    KT_OBJECT,
    KT_ARRAY,
    KT_NULL
};



/*
    Return now value in microseconds
*/
long long now();



bool stringToBool
(
    string
);



long long int stringToInt
(
    string
);



double stringToDouble
(
    string
);



const vector<string> explode
(
    const string&,
    const string&,
    const bool = true,
    const int = 0
);




/*
    Implode the vector string
*/
string implode
(
    const vector<string>,
    const string&
);



bool mkDir
(
    string,         /* Path */
    int = S_IRWXU   /* Right */
);



bool fileExists
(
    string
);



long int fileLastUpdate
(
    string
);



/*
    Check file update
*/
bool checkFileUpdate
(
    const string,   /* File name */
    long int&       /* Last moment update. Will be updated if file was chenged */
);



long int fileSize
(
    string
);



bool checkPath
(
    const string
);



string getPath
(
    const string
);



vector <string> getFileList
(
    const string
);



/*
    Return file extention
*/
string getFileExt
(
    const string
);



/*
    Return type by string
*/
ParamType getType
(
    string
);



/*
    Convert hex string to integer
*/
unsigned int hexToInt
(
    string a
);



/*
    Decode byte to utf length
*/
unsigned char utfLength
(
    const unsigned char
);



/*
    Return UTF symbol from string as string symbol
    funcation may return one or more bytes
*/
string utfSimbol
(
    const string,           /* string Utf8*/
    const unsigned int,     /* index in string */
    const unsigned char = 0 /* length from utfLength */
);



/*
    Return count of UTF symbols
*/
unsigned int utfCount
(
    const string a
);




/*
    Convert char memory to string
*/
string toString
(
    char*,
    unsigned long long int
);



/*
    Convert string to lower
*/
void toLowerCase
(
    string&
);




void b
(
    string = "break"
);



void b
(
    double = 0
);
