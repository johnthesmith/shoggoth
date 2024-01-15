#pragma once

/*
    The Bitmap muzle on ImageMagic
    http://www.graphicsmagick.org/Magick++/Image.html
*/

#include <Magick++.h>
#include "../lib/result.h"
#include "rgba.h"


using namespace std;



class Bitmap : public Result
{
    private:

        int             width   = 0;
        int             height  = 0;

        Magick::Image*  image   = NULL;
        void*           buffer  = NULL;

        Bitmap* updateSize();

    public:

        /*
            Constructor
        */
        Bitmap();



        /*
            Destructor
        */
        ~Bitmap();


        /*
            Creator
        */
        static Bitmap* create();



        /*
            Destroy the bitmap
        */
        void destroy();



        /*
            Load bitmap from file
        */
        Bitmap* load
        (
            string  /* File name */
        );




        /*
            Load bitmap from file
        */
        Bitmap* save
        (
            string  /* File name */
        );



        /*
            Resize bitmap
        */
        Bitmap* scale
        (
            int /* Width */,
            int /* Height */
        );



        /*
            Get width of bitmap
        */
        int getWidth();



        /*
            Get height of bitmap
        */
        int getHeight();




        /*
            Set width of bitmap
        */
        Bitmap* setSize
        (
            int,
            int
        );




        /*
            Get pixel
        */
        Bitmap* getRgba
        (
            int,
            int,
            Rgba&
        );



        /*
            Rectangle
        */
        Bitmap* rectangle
        (
            int,
            int,
            int,
            int
        );


        /*
            Draw text on bitamp
        */
        Bitmap* text
        (
            string,
            string,
            int,    /* Size */
            int,
            int
        );



        /*
            Clear bitmap
        */
        Bitmap* erase();



        /*
            Return image buffer
        */
        unsigned short* buildRGBA();
        Bitmap* clearRGBA
        (
            unsigned short*
        );

};



static bool imageMagicInit = false;    /* Global flag for image magic initialization in Bitmap constructor */


