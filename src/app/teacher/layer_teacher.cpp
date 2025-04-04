#include "../../shoggoth/limb.h"

#include "layer_teacher.h"



/*
    Constructor
*/
LayerTeacher::LayerTeacher
(
    Limb* aLimb,
    string aId
)
:Layer( aLimb, aId )
{
    getLog() -> trace( "... it was a layer teacher" );
}



/*
    Internal destructor
*/
LayerTeacher::~LayerTeacher()
{
}



/*
    Creator
*/
LayerTeacher* LayerTeacher::create
(
    Limb* aLimb,
    string aId
)
{
    return new LayerTeacher( aLimb, aId );
}



/*
    Destructor
*/
void LayerTeacher::destroy()
{
    delete this;
}



/******************************************************************************
    Fill layer
*/



/*
    Noise fill values of layer neurons
*/
LayerTeacher* LayerTeacher::noiseValue
(
    Rnd*    aRnd,
    double  aMin,
    double  aMax
)
{
    getLimb() -> lock();
    auto c = getCount();
    for( int i = 0; i < c; i ++ )
    {
        setNeuronValue( i, aRnd -> get( aMin, aMax ) );
    }
    getLimb() -> unlock();

    return this;
}




/*
    Noise fill values of layer neurons
*/
LayerTeacher* LayerTeacher::fillValue
(
    ParamList* aValues
)
{
    getLimb() -> lock();
    auto c = getCount();
    auto cv = aValues -> getCount();
    for( int i = 0; i < c; i ++ )
    {
        setNeuronValue( i, aValues -> getByIndex( i % cv ) -> getDouble() ) ;
    }
    getLimb() -> unlock();

    return this;
}



/**********************************************************************
    Load valus
*/



/*
    Set bitmap to neurons
*/
LayerTeacher* LayerTeacher::bitmapToValue
(
    Bitmap* a
)
{
    auto w = a -> getWidth();
    auto h = a -> getHeight();

    Rgba rgba;

    getLimb() -> lock();

    auto size = getSize();

    for( int z = 0; z < size.z; z++ )
    {
        for( int y = 0; y < size.y; y++ )
        {
            for( int x = 0; x < size.x; x++ )
            {
                a -> getRgba
                (
                    ( double ) x / ( double ) size.x * w,
                    (1-(( double ) y / ( double ) size.y )) * h - 1,
                    rgba
                );
                setNeuronValue
                (
                    indexByPos( Point3i( x, y, z )),
                    rgba.getGray()
                );
            }
        }
    }

    getLimb() -> unlock();

    return this;
}



/*
    Load bitmap from source file name and set it
*/
LayerTeacher* LayerTeacher::imageToValue
(
    string aFileName,
    double aRotate,
    double aZoomMin,
    double aZoomMax,
    double aShift,
    Result* result,
    Rnd*    aRnd
)
{
    if( isOk() )
    {
        auto fileExt = getFileExt( aFileName );
        if( fileExt == "png" )
        {
            /* Load from bitmap */
            auto bitmap = Bitmap::create() -> load( aFileName );
            if( bitmap -> isOk() )
            {
                bitmap
                -> rotate( aRnd -> get( -aRotate, aRotate ), true )
                -> zoom( aRnd -> get( aZoomMin, aZoomMax ), true )
                -> shift( aRnd -> get( -aShift, aShift ), aRnd -> get( -aShift, aShift ));

                getLog()
                -> trace( "Image loaded" )
                -> prm( "Layer", getNameOrId() )
                -> prm( "File", aFileName )
                -> prm( "width", bitmap -> getWidth() )
                -> prm( "height", bitmap -> getHeight() )
                -> lineEnd();
                bitmapToValue( bitmap );
            }
            else
            {
                result -> resultFrom( bitmap );
            }
            bitmap -> destroy();
        }
        else
        {
            result
            -> setResult( "unknown_image_format" )
            -> getDetails()
            -> setString( "extention", fileExt );
        }
    }
    else
    {
        result -> resultFrom( this );
    }
    return this;
}



LayerTeacher* LayerTeacher::applyUuid
(
    Hid a
)
{
    getLimb() -> lock();
    auto c = getCount();
    for( int i = 0; i < c; i++ )
    {
        setNeuronValue( i, a.getBit( i ) ? 1.0 : 0.0 );
    }
    getLimb() -> unlock();
    return this;
}
