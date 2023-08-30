#include "texture.h"



/*
    Constructor
*/
Texture::Texture
(
    Log* aLog
)
{
    log = aLog;
}



/*
    Destructor
*/
Texture::~Texture()
{
}



/*
    Create and return texture
*/
Texture* Texture::create
(
    Log* log
)
{
    return new Texture( log );
}



/*
    Destroy texture
*/
void Texture::destroy()
{
    free();
    delete this;
}



/*
    Build from bitmap
*/
Texture* Texture::buildFromBitmap
(
    Bitmap* aBitmap
)
{
    resultFrom( aBitmap );
    if( isOk() )
    {
        /* Get RGBA bbuffer */
        auto buffer = aBitmap -> buildRGBA();

        /* Get size of inage */
        int w = aBitmap -> getWidth();
        int h = aBitmap -> getHeight();

        /* Rebuild componencts */
        for( int y = 0; y < h; y++ )
        {
            for( int x = 0; x < w; x++ )
            {
                unsigned int index = 4 * ( x + y * w );

                buffer[ index + 3 ] = buffer[ index ];
                buffer[ index + 0 ] = 0xFFFF;
                buffer[ index + 1 ] = 0xFFFF;
                buffer[ index + 2 ] = 0xFFFF;
            }
        }


        /* Free texture */
        free();

        /* Create texture */
        glGenTextures( 1, &openglId );

        glBindTexture( GL_TEXTURE_2D, openglId );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            w,
            h,
            0,
            GL_RGBA,
            GL_UNSIGNED_SHORT,
            buffer
        );
        glGenerateMipmap( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, 0 );


        aBitmap -> clearRGBA( buffer );
    }
    return this;
}



Texture* Texture::free()
{
    if( openglId != 0 )
    {
        glDeleteTextures( 1, &openglId );
    }
    return this;
}



Texture* Texture::use()
{
    glBindTexture( GL_TEXTURE_2D, openglId );
    return this;
}
