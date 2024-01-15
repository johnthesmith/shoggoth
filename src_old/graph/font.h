#pragma once

#include <functional>   /* for lyambda */


/* TODO next 2 lines must be removed */
#include <GL/gl.h>
#include <GL/glu.h>

#include "../lib/result.h"
#include "../lib/log.h"
#include "../lib/heap.h"

#include "point2d.h"
#include "bitmap.h"
#include "texture.h"



struct Gliph
{
    Point2d     leftTop     = POINT_2D_Y;
    Point2d     rightTop    = POINT_2D_XY;
    Point2d     rightBottom = POINT_2D_X;
    Point2d     leftBottom  = POINT_2D_0;
    double      width       = 1.0;
    Gliph*      list        = NULL;

    Gliph* set
    (
        int, /* imageWidth*/
        int, /* imageHeight */
        int, /* left */
        int, /* top */
        int, /* right */
        int, /* bottom */
        int  /* gliphSize */
    );
};



class Font : public Bitmap
{
    private:

        Log*        log             = NULL;
        float       space           = 0.3;
        float       letterSpacing   = 0.0;
        Texture*    texture         = NULL;
        Gliph*      gliphList       = NULL;
        string      fontName        = "/usr/share/fonts/truetype/ubuntu/Ubuntu-Th.ttf";
        int         gliphSize       = 128;
        string      charSet         = " !@#$£€¥%^&*()_+=-{}[]\\|/~`<>?.,;:'\"«»†∞1234567890abcdefghijklmnopqarstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXWZ";
        string      cachePath       = "/tmp";

    public:



        /*
            Constructor
        */
        Font( Log* );



        /*
            Destructor
        */
        ~Font();



        /*
            Create and return Font
        */
        static Font* create( Log* );



        /*
            Destroy the Font
        */
        void destroy();



        /*
            Build font from bitmap with attributes
            - font name
            - gliph size
            - list of symbols
            - cache path
        */
        Font* build();



        /*
            Remove font bitmap
        */
        Font clear();



        /*
            Return log
        */
        Log* getLog();



        Font* getGliphPos
        (
            int sourceLeft,
            int sourceTop,
            int sourceRight,
            int sourceBottom,
            int &gliphLeft,
            int &gliphTop,
            int &gliphRight,
            int &gliphBottom
        );



        Texture* getTexture();



        /*
            Loop for each UTF
        */
        Font* utfLoop
        (
            const string,
            function <bool ( Gliph& )>
        );



        float getSpace();
        float getLetterSpacing();



        /*
            Return the file name
        */
        string getFontName();



        /*
            Set font name or file with ttf
            Rebuild font in build method
        */
        Font* setFontName
        (
            string  /* File name */
        );



        /*
            Return the gliph size in pixels
        */
        int getGliphSize();



        /*
            Set the gliph size in pixels
            Rebuild font in build method
        */
        Font* setGliphSize
        (
            int
        );



        /*
            Return the charset
        */
        string getCharSet();



        /*
            Set the charset
            Rebuild font in build method
        */
        Font* setCharSet
        (
            string
        );



        /*
            Return the cache path
        */
        string getCachePath();



        /*
            Set cache path
            Rebuild font in build method
        */
        Font* setCachePath
        (
            string
        );



        double calcTextWidth
        (
            string
        );
};
