#pragma once

#include <GLES3/gl31.h>
#include <GLFW/glfw3.h>
#include <GL/glext.h>

#include "../lib/log.h"
#include "../lib/result.h"

#include "bitmap.h"

class Texture: public Result
{
    private:
        Log*            log = NULL;
        unsigned int    openglId    = 0;
    public:


        /*
            Constructor
        */
        Texture
        (
            Log*
        );



        /*
            Destructor
        */
        ~Texture();



        /*
            Create and return texture
        */
        static Texture* create
        (
            Log*
        );



        /*
            Destroy texture
        */
        void destroy();



        /*
            Build from bitmap
        */
        Texture* buildFromBitmap
        (
            Bitmap*
        );


        /*
            Free texture
        */
        Texture* free();


        Texture* use();
};
