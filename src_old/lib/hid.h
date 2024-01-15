#pragma once

#include <string>

#include "rnd.h"
#include "md5.h"



using namespace std;



class Hid
{
    private:

        unsigned char data[ 16 ] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };

    public:

        /*
            Hid from string
        */
        Hid& setString
        (
            string
        );

        /*
            Fill hid from GUID
        */
        Hid& newGuid();

        /*
            Get bit true or falce
        */
        bool getBit
        (
            int
        );

        /*
            Return hex notation
        */
        string getHex();


        /*
            Return uuid notation
        */
        string getUuid();



        /*
            Set uuid notation
        */
        Hid& setUuid
        (
            string
        );
};
