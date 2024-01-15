#include <iostream>
#include <cmath>

#include "font.h"
#include "../lib/utils.h"
#include "../lib/md5.h"



Gliph* Gliph::set
(
    int imageWidth,
    int imageHeight,
    int left,
    int top,
    int right,
    int bottom,
    int gliphSize
)
{
    leftTop = Point2d
    (
        ( double )( left ) / ( double )imageWidth,
        (( double )( top ) / ( double )imageHeight)
    );

    rightTop = Point2d
    (
        ( double )( right ) / ( double )imageWidth,
        (( double )( top ) / ( double )imageHeight)
    );

    rightBottom = Point2d
    (
        ( double )( right ) / ( double )imageWidth,
        ( double )( bottom ) / ( double )imageHeight
    );

    leftBottom = Point2d
    (
        ( double )( left ) / ( double )imageWidth,
        ( double )( bottom ) / ( double )imageHeight
    );

    width = (( double ) right - (double) left ) / (double)gliphSize;
    list = NULL;

    return this;
}




Font::Font
(
    Log* aLog
)
{
    log = aLog;

    /* Create texture */
    texture = Texture::create( aLog );

    gliphList = new Gliph[ 256 ];
}




Font::~Font()
{
    /* Destroy texture */
    texture -> destroy();

    /* Destroy gliphs  */
    for( unsigned int i = 0; i < 256; i++ )
    {
//        delete ( Gliph* ) gliphList -> getByIndex( i );
    }

    /* Destrpy gliph list */
    delete [] gliphList;
}



/*
    Create and return Font
*/
Font* Font::create
(
    Log* aLog
)
{
    return new Font( aLog );
}



/*
    Destroy the Font
*/
void Font::destroy()
{
    delete this;
}



/*
    Build font from bitmap
*/
Font* Font::build()
{
    getLog()
    -> begin( "Build font ")
    -> trace( "" )
    -> prm( "Font name", fontName )
    -> prm( "Charset", charSet );

//    string fileName = cachePath + "/font_" + MD5( charSet + to_string( gliphSize ) + fontName ).hexdigest() + ".bin";
//    getLog() ->trace( "Cache file" ).prm( "Name", fileName );

    unsigned int symbolCount = utfCount( charSet );  /* Count of UTF symbols */
    unsigned int gliphCols = ceil( sqrt( symbolCount ));    /* Count gliph columns */
    unsigned int gliphRows = ceil( (double )symbolCount / (double)gliphCols);  /* Count gliph rows */

    getLog()
    -> trace( "Info" )
    -> prm( "Symbols count", symbolCount )
    -> prm( "Gliph columns count", gliphCols )
    -> prm( "Gliph rows count", gliphRows )
    -> lineEnd();

    unsigned int c = charSet.size();
    unsigned int i = 0;  /* Byte number in string */
    unsigned int n = 0;  /* Symbol number */
    unsigned int lGliphSize = gliphSize * 1.3;

    /* Set image size */
    unsigned int imageWidth = lGliphSize * gliphCols;
    unsigned int imageHeight = lGliphSize * gliphRows;

    setSize( imageWidth, imageHeight );
    erase();

    while( i < c )
    {
        auto l = utfLength( charSet[ i ] );
        auto s = utfSimbol( charSet, i, l );

        unsigned int y = n / gliphCols;
        unsigned int x = n - y * gliphCols;

        /* Out the letter at canvas */
        text( s, fontName, gliphSize, x * lGliphSize, ( y + 1 - 0.2 ) * lGliphSize );

        /* Cut canvas position to letter position */
        int left, top, right, bottom;
        getGliphPos
        (
            x * lGliphSize,
            y * lGliphSize,
            x * lGliphSize + lGliphSize - 1,
            y * lGliphSize + lGliphSize - 1,
            left, top, right, bottom
        );

        /* Get first item */
        Gliph* gliph = &gliphList[ (unsigned char)s[ 0 ] ];

        /* Convert canvas to gliph texture position */
        for( int iChar = 1; iChar < l; iChar++ )
        {
            if( gliph -> list == NULL )
            {
                gliph -> list = new Gliph[ 256 ];
            }
            gliph = &gliph -> list[ (unsigned char)s[ iChar ] ];
        }
        gliph -> set( imageWidth, imageHeight, left, top, right, bottom, gliphSize );

        n++;
        i += l;
    }

    save( "font.png" );

    /* Build texture */
    texture -> buildFromBitmap( this );

    getLog() -> end();

    return this;
};



/*
    Remove font bitmap
*/
//Font clear();




//ParamList* Font::createBranch()
//{
//    return ParamList::create() -> resize( 255 ) -> loop
//    (
//        []
//        ( void* aParam )
//        {
//            aParam = ParamInt::create() -> setValue( 0 );
//            return false;
//        }
//    );
//}



/*
    Return log
*/
Log* Font::getLog()
{
    return log;
}



/*
*/
Font* Font::getGliphPos
(
    int sourceLeft,
    int sourceTop,
    int sourceRight,
    int sourceBottom,
    int &gliphLeft,
    int &gliphTop,
    int &gliphRight,
    int &gliphBottom
)
{
    /* Reset left and rigtht ages */
    gliphLeft = sourceRight;
    gliphTop = sourceTop;
    gliphRight = sourceLeft;
    gliphBottom = sourceBottom;
    /*
            *sourceLeft      *sourceRight
            |                |
            |       ooo      |
            |      o   o     |
            |      o   o     |
            |      |ooo|     |
                   |   |
          gliphLeft*   *gliphRight
    */
    bool last = false;
    bool curr = false;
    bool space = true;
    for( unsigned int y = sourceTop; y < sourceBottom; y++ )
    {
        for( unsigned int x = sourceLeft; x < sourceRight; x++ )
        {
            Rgba color;
            getRgba( x, y, color );
            curr = color.red > 0.1;

            if( last != curr )
            {
                if( curr )
                {
                    if( gliphLeft > x )
                    {
                        gliphLeft = x;
                    }
                }
                else
                {
                    if( gliphRight < x )
                    {
                        gliphRight = x;
                    }
                }
                space = false;
            }

            last = curr;
        }
    }


    /* */
    if( space )
    {
        /* It is a space character */
        gliphLeft = sourceLeft;
        gliphRight = sourceLeft;
    }
    else
    {
        /* Expend borders at 5% from width */
        int border = ( sourceRight - sourceLeft ) * 0.04;
        gliphLeft = gliphLeft - border;
        gliphRight = gliphRight + border;
    }


    /*
        draw eages
        rectangle( gliphLeft, sourceTop+2, gliphLeft+1, sourceBottom-2 );
        rectangle( gliphRight, sourceTop+2, gliphRight+1, sourceBottom-2 );
    */
    return this;
}



Texture* Font::getTexture()
{
    return texture;
}



/*
    Loop for each UTF char in string
*/
Font* Font::utfLoop
(
    const string aChars,
    function <bool ( Gliph& )> aCallback
)
{
    unsigned int c = aChars.size();
    unsigned int i = 0;  /* Byte number in string */
    bool stop = false;

    while( i < c && !stop )
    {
        auto l = utfLength( aChars[ i ] );
        auto s = utfSimbol( aChars, i, l );

        /* Get first item */
        Gliph* gliph = &gliphList[ (unsigned char)s[ 0 ] ];

        /* Convert canvas to gliph texture position */
        for( int iChar = 1; iChar < l && gliph != NULL; iChar++ )
        {
            gliph = gliph -> list != NULL
            ? &gliph -> list[ (unsigned char)s[ iChar ] ]
            : NULL;
        }

        if( gliph != NULL )
        {
            stop = aCallback( *gliph );
        }

        i += l;
    }

    return this;
}




float Font::getSpace()
{
    return space;
}



float Font::getLetterSpacing()
{
    return letterSpacing;
}



/*
    Return the file name
*/
string Font::getFontName()
{
    return fontName;
}



/*
    Set font name or file with ttf
    Rebuild font in build method
*/
Font* Font::setFontName
(
    string  a
)
{
    fontName = a;
    return this;
}



/*
    Return the gliph size in pixels
*/
int Font::getGliphSize()
{
    return gliphSize;
}



/*
    Set the gliph size in pixels
    Rebuild font in build method
*/
Font* Font::setGliphSize
(
    int a
)
{
    gliphSize = a;
    return this;
}



/*
    Return the charset
*/
string Font::getCharSet()
{
    return charSet;
}



/*
    Set the charset
    Rebuild font in build method
*/
Font* Font::setCharSet
(
    string a
)
{
    charSet = a;
    return this;
};



/*
    Return the cache path
*/
string Font::getCachePath()
{
    return cachePath;
}



/*
    Set cache path
    Rebuild font in build method
*/
Font* Font::setCachePath
(
    string a
)
{
    cachePath = a;
    return this;
}



/*
    Return gliph in width points
*/
double Font::calcTextWidth
(
    string aString
)
{
    double result = 0;
    utfLoop
    (
        aString,
        [ this, &result ]
        ( Gliph& iGliph )
        {
            result = result + ( iGliph.width <= 0 ? space : iGliph.width );
            return false;
        }
    );
    return result;
}
