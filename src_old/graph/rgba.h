#pragma once



struct Rgba
{
    float red   = 0.0;
    float green = 0.0;
    float blue  = 0.0;
    float alpha = 0.0;



    /*
        Constructor
        Create RGBA component with new
    */
    Rgba
    (
        float = 0.0, /* aRed */
        float = 0.0, /* aGreen */
        float = 0.0, /* aBlue */
        float = 0.0  /* aAlpha */
    );



    /*
        Constructor
        Create RGBA from other Rgba
    */
    Rgba
    (
        const Rgba&
    );



    /*
        Set vector coordinates
    */
    Rgba& set
    (
        float = 0.0, /* aRed */
        float = 0.0, /* aGreen */
        float = 0.0, /* aBlue */
        float = 0.0  /* aAlpha */
    );



    /* Set Red component */
    Rgba& setRed
    (
        float = 0.0 /* Red value */
    );



    /* Set Green component */
    Rgba& setGreen
    (
        float = 0.0 /* Green value */
    );



    /* Set Blue component */
    Rgba& setBlue
    (
        float = 0.0 /* Blue value */
    );



    /* Set Alpha component */
    Rgba& setAlpha
    (
        float = 0.0 /* Alpha value */
    );



    /* Get grey */
    double getGray();



    /*
        Linear interpolation
    */
    Rgba& itpLin
    (
        const Rgba&, /* To color */
        const double
    );



    /*
        Linear interpolation
    */
    Rgba&  itpSgm
    (
        const Rgba&, /* To color */
        const double
    );



    Rgba& temperatura
    (
        double
    );



    Rgba& landscape
    (
        double
    );
};



/*
    Define static constatns
*/
static Rgba RGBA_TRANSPARENT= Rgba( 0.0, 0.0, 0.0, 0.0 );
static Rgba RGBA_BLACK      = Rgba( 0.0, 0.0, 0.0, 1.0 );
static Rgba RGBA_WHITE      = Rgba( 1.0, 1.0, 1.0, 1.0 );
static Rgba RGBA_RED        = Rgba( 1.0, 0.0, 0.0, 1.0 );
static Rgba RGBA_GREEN      = Rgba( 0.0, 1.0, 0.0, 1.0 );
static Rgba RGBA_BLUE       = Rgba( 0.0, 0.0, 1.0, 1.0 );
static Rgba RGBA_YELLOW     = Rgba( 1.0, 1.0, 0.0, 1.0 );
static Rgba RGBA_MAGENTA    = Rgba( 1.0, 0.0, 1.0, 1.0 );
static Rgba RGBA_ORANGE     = Rgba( 1.0, 0.5, 0.0, 1.0 );
static Rgba RGBA_CYAN       = Rgba( 0.0, 1.0, 1.0, 1.0 );

static Rgba RGBA_X          = RGBA_RED;
static Rgba RGBA_Y          = RGBA_GREEN;
static Rgba RGBA_Z          = RGBA_BLUE;
