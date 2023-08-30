#pragma once

#include "sys/time.h"
#include <string>



using namespace std;



class Rnd
{
    private:

        static unsigned long long seed;            /* Current seed */
        static unsigned long long storedSeed;      /* Stored seed */

    public:

        /*
            Begin randomize from current time
        */
        static void randomize();



        /*
            Store seed value
        */
        static void storeSeed
        (
            unsigned long long
        );



        /*
            Restore seed value
        */
        static void restoreSeed();



        /*
            Get random value
        */
        static void calcSeed();



        /*
            Get random value
        */
        static double get();



        /*
            Get random value from AMin to AMax
        */
        static int get
        (
            int,
            int
        );



        /*
            Get random value from AMin to AMax
        */
        static unsigned int get
        (
            unsigned int,
            unsigned int
        );



        /*
            Get random value from AMin=0 to AMax=0xFFFF FFFF FFFF
        */
        static unsigned long long get
        (
            unsigned long long,
            unsigned long long
        );



        /*
            Get random value from AMin to AMax
        */
        static double get
        (
            double,
            double
        );



        /*
            Return uuid
        */
        static string getUuid();



        /*
            Return uuid by seed
        */
        static string getUuid
        (
            unsigned long long
        );



        /*
            возвращает необходимость исолнения вероятного события
        */
        static bool chanceByTime
        (
            long,
            long
        );
};
