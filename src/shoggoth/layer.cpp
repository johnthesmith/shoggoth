#include "../lib/rnd.h"
#include "../lib/math.h"
#include "../lib/moment.h"

#include "../graph/point3.h"
#include "../graph/log_points.h"
#include "../sock/rpc_client.h"

#include "func.h"
#include "io.h"
#include "layer.h"
#include "neuron.h"
#include "net.h"



/*
    Constructor
*/
Layer::Layer
(
    Net* aNet,
    string aId,
    bool aValuesExists,
    bool aErrorsExists,
    bool aScreenExists,
    bool aWorldExists,
    bool aSelectedExists

)
{
    net = aNet;
    id = aId == "" ? Rnd::getUuid() : aId;

    /* Set data plans flags */
    valuesExists    = aValuesExists;
    errorsExists    = aErrorsExists;
    screenExists    = aScreenExists;
    worldExists     = aWorldExists;
    selectedExists  = aSelectedExists;

    /* Actions */
    actions         = ParamList::create();

    /* Log */
    getLog() -> trace( "Create layer" ) -> prm( "id", id );
}



/*
    Internal destructor
*/
Layer::~Layer()
{
    /* Actions destroy */
    actions -> destroy();

    /* Destroy neurons */
    setSize();

    /* Destroy the list of neurons */

    getLog() ->
    trace( "Layer destroy" )
    -> prm( "Name", getNameOrId() );
}



/*
    Creator
*/
Layer* Layer::create
(
    Net* aNet,
    string aId,
    bool aValuesExists,
    bool aErrorsExists,
    bool aScreenExists,
    bool aWorldExists,
    bool aSelectedExists
)
{
    return new Layer
    (
        aNet,
        aId,
        aValuesExists,
        aErrorsExists,
        aScreenExists,
        aWorldExists,
        aSelectedExists
    );
}



/*
    Destructor
*/
void Layer::destroy()
{
    delete this;
}



/*
    Return linear index of neuron by position
*/
int Layer::indexByPos
(
    const Point3i& a
)
{
    return a.x + a.y * size.x + a.z * size.x * size.y;
}



/*
    Calculate neuron position at world
*/
Layer* Layer::neuronPointsCalc
(
    bool aStrict
)
{
    if( aStrict || !pointsRecalc )
    {
        /* Calculate box */
        auto box = Point3d
        (
            drawSize.x == 0 ? ( size.x - 1 ) * neuronDrawBox : drawSize.x,
            drawSize.y == 0 ? ( size.y - 1 ) * neuronDrawBox : drawSize.y,
            drawSize.z == 0 ? ( size.z - 1 ) * neuronDrawBox : drawSize.z
        );

        /* Draw layer box */
        outerBox = Point3d().set( box ).scale( 0.5 ).add( borderSize );
        getLog() -> begin( "Calculate neurons world position" )-> prm( "Layer", getNameOrId() );

        /* Claculate step in world for neurons */
        auto step = box / ( size - POINT_3I_I );
        /* Calculate ege in the world and start point in the world */
        Point3d ege = box * -0.5 + getEye();
        Point3d p = ege;

        int i = 0;

        for( int z = 0; z < size.z; z++ )
        {
            for( int y = 0; y < size.y; y++ )
            {
                for( int x = 0; x < size.x; x++ )
                {
                    setNeuronWorld( i, p );
                    p.x += step.x;
                    i++;
                }
                p.y += step.y;
                p.x = ege.x;
            }
            p.z += step.z;
            p.y = ege.y;
        }

        getLog() -> end();

        setPointsRecalc( true );
    }

    return this;
}



/*
    Clear values for all neurons
*/
Layer* Layer::clearValues()
{
    memset( values, 0, count * sizeof( double ));
    return this;
}



/*
    Clear errors for all neurons
*/
Layer* Layer::clearErrors()
{
    memset( errors, 0, count * sizeof( double ));
    return this;
}



/*
    Clear screen points for all neurons
*/
Layer* Layer::clearScreen()
{
    memset( screen, 0, count * sizeof( Point3d ));
    return this;
}



/*
    Clear world for all neurons
*/
Layer* Layer::clearWorld()
{
    memset( world, 0, count * sizeof( Point3d ));
    return this;
}



/*
    Clear selected flag for all neurons
*/
Layer* Layer::clearSelected()
{
    memset( selected, 0, count * sizeof( bool ));
    return this;
}




Layer* Layer::calcValue
(
    int aProcessorNumber,
    int aProcessorCount
)
{
    int b = calcNeuronFrom( aProcessorNumber, aProcessorCount );
    int e = calcNeuronTo( aProcessorNumber, aProcessorCount );

    for( int i = b; i < e; i ++ )
    {
        neuronCalcValue( i );
    }

    return this;
}



/*
    Calculate neurons error for learning
*/
Layer* Layer::learning
(
    double  aErrorNormalize,
    double  aLearningSpeed,
    double  aWakeupWeight,
    int     aProcessorNumber,
    int     aProcessorCount
)
{
    int b = calcNeuronFrom( aProcessorNumber, aProcessorCount );
    int e = calcNeuronTo( aProcessorNumber, aProcessorCount );

    for( int i = b; i < e; i ++ )
    {
        neuronLearning
        (
            i,
            aErrorNormalize,
            aLearningSpeed,
            aWakeupWeight
        );
    }

    return this;
}



/***********************************************************************
    Setters and getters
*/

/*
    Return the log object
*/
Log* Layer::getLog()
{
    return net -> getLog();
}


/*
    Return the Net object
*/
Net* Layer::getNet()
{
    return net;
}



/*
    Set dimations size
*/
Layer* Layer::setSize
(
    const Point3i& a
)
{
    auto newCount = a.x * a.y * a.z;

    if( newCount != count )
    {
        /* The size was changed */
        getLog()
        -> begin( "Layer resize" )
        -> prm( "id", getNameOrId() )
        -> prm( "from", count )
        -> prm( "to", newCount )
        -> lineEnd();

        /* Change count and size */
        count = newCount;
        size = a;

        /* Resize values plan */
        if( valuesExists )
        {
            /* Delete old plan */
            if( values != NULL )
            {
                delete [] values;
            }

            /*Create new plan */
            values = new double[ newCount ];
            clearValues();
        }

        /* Resize errors plan */
        if( errorsExists )
        {
            /* Delete old plan */
            if( errors != NULL )
            {
                delete [] errors;
            }

            /*Create new plan */
            errors = new double[ newCount ];
            clearErrors();
        }

        /* Resize world plan */
        if( worldExists )
        {
            /* Delete old plan */
            if( world != NULL )
            {
                delete [] world;
            }

            /*Create new plan */
            world = new Point3d[ newCount ];
            /* Clear world position */
            clearWorld();
            /* Recalculate neuron points */
            neuronPointsCalc( true );
        }

        /* Resize screen plan */
        if( screenExists )
        {
            /* Delete old plan */
            if( screen != NULL )
            {
                delete [] screen;
            }

            /*Create new plan */
            screen = new Point3d[ newCount ];
            clearScreen();
        }

        /* Resize selected plan */
        if( selectedExists )
        {
            /* Delete old plan */
            if( selected != NULL )
            {
                delete [] selected;
            }

            /*Create new plan */
            selected = new bool[ newCount ];
            clearSelected();
        }

        getLog() -> end();
    }

    return this;
}




/*
    Set dimations size
*/
Layer* Layer::setSize
(
    ParamList* a
)
{
    auto jsonSize = a -> getObject( "size" );
    if( jsonSize != NULL )
    {
        setSize
        (
            Point3i
            (
                jsonSize -> getInt( 0 ),
                jsonSize -> getInt( 1 ),
                jsonSize -> getInt( 2 )
            )
        );

        /* Set neurons recalculation task if draw size was changed */
        if(  getChanged() )
        {
            setPointsRecalc( false );
        }
    }
    return this;
}



/*
    Set draw size for layer
*/
Layer* Layer::setDrawSize
(
    ParamList* a
)
{
    auto jsonPosition = a -> getObject( "drawSize" );
    if( jsonPosition != NULL )
    {
        setDrawSize
        (
            Point3d
            (
                jsonPosition -> getDouble( 0 ),
                jsonPosition -> getDouble( 1 ),
                jsonPosition -> getDouble( 2 )
            )
        );

        /* Set neurons recalculation task if draw size was changed */
        if(  getChanged() )
        {
            setPointsRecalc( false );
        }
    }

    return this;
}



/*
    Set dimations size
*/
Layer* Layer::setPosition
(
    ParamList* a
)
{
    auto jsonPosition = a -> getObject( "position" );
    if( jsonPosition != NULL )
    {
        setEye
        (
            Point3d
            (
                jsonPosition -> getDouble( 0 ),
                jsonPosition -> getDouble( 1 ),
                jsonPosition -> getDouble( 2 )
            )
        );

        /* Set neurons recalculation task if position was changed */
        if(  getChanged() )
        {
            setPointsRecalc( false );
        }
    }
    return this;
}




/*
    Return the name of layer
*/
string Layer::getName()
{
   return name;
}



/*
    Set the name of layer
*/
Layer* Layer::setName
(
    string a
)
{
   name = a;
   return this;
}



string Layer::getId()
{
   return id;
}



/*
    Set the id of layer
*/
Layer* Layer::setId
(
    string a
)
{
   id = a;
   return this;
}




string Layer::getNameOrId()
{
   return name == "" ? id : name;
}



Layer* Layer::setNeuronDrawBox
(
    const double a
)
{
    neuronDrawBox = a;
    return this;
}



Layer* Layer::setNeuronDrawSize
(
    const double a
)
{
    neuronDrawSize = a;
    return this;
}



Layer* Layer::setBorderSize
(
    const Point3d& a
)
{
    borderSize = a;
    return this;
}



Layer* Layer::setDrawSize
(
    const Point3d& a
)
{
    if( drawSize != a )
    {
        drawSize = a;
        setChanged( true );
    }
    return this;
}



Layer* Layer::setPointsRecalc
(
    bool a
)
{
    pointsRecalc = a;
    return this;
}



/*
    Return true if neurons error changed in calcError method
*/
bool Layer::getErrorChange()
{
    return errorChange;
}



/*
    Return list of neurons in screen rect
*/
Layer* Layer::getNeuronsByScreenRect
(
    NeuronList* aList,
    Point3d& aTopLeft,      /* Top left point */
    Point3d& aBottomRight   /* Bottom right point */
)
{
    auto list = new int[ count ];
    int iBuffer = 0;

    /* Coolect indexes to list */
    for( int i = 0; i < count; i++ )
    {
        if
        (
            getNeuronScreen( i ).testRectXY( aTopLeft, aBottomRight )
        )
        {
            list[ iBuffer ] = i;
            iBuffer++;
        }
    }

    int c = aList -> getCount();
    /* Resize the rusult buffer */
    aList -> resize( c + iBuffer );

    for( int i = 0; i < iBuffer; i++ )
    {
        aList -> setByIndex( c + i, Neuron::create( this, list[ i ]));
    }

    delete [] list;

    return this;
}



/*
    Return list of neurons around  the screen position
*/
Layer* Layer::getNeuronsByScreenPos
(
    NeuronList*     aList,
    const Point3d&  aPosition,
    const int       aScreenRadius
)
{
    auto list = new int[ count ];
    int iBuffer = 0;

    /* Coolect indexes to list */
    for( int i = 0; i < count; i++ )
    {
        auto s = getNeuronScreen( i );
        if
        (
            s.z < 1.0 &&
            s.distXY( aPosition ) < aScreenRadius
        )
        {
            list[ iBuffer ] = i;
            iBuffer++;
        }
    }

    int c = aList -> getCount();

    /* Resize the rusult buffer */
    aList -> resize( c + iBuffer );
    for( int i = 0; i < iBuffer; i++ )
    {
        aList -> setByIndex
        (
            c + i,
            Neuron::create( this, list[ i ])
        );
    }

    delete [] list;

    return this;
}



Layer* Layer::switchShowBinds()
{
    switch( showBinds )
    {
        case BDM_HIDDEN : setShowBinds( BDM_WEIGHT ); break;
        case BDM_WEIGHT : setShowBinds( BDM_TYPE ); break;
        case BDM_TYPE   : setShowBinds( BDM_HIDDEN ); break;
    }
    return this;
}






Layer* Layer::switchShowLayer()
{
    showLayer = !showLayer;
    return this;
}



Point3i Layer::getSize()
{
    return size;
}



int Layer::getNeuronsCount()
{
    return count;
}



BindDrawMode Layer::getShowBinds()
{
    return showBinds;
}



Layer* Layer::setShowBinds
(
    BindDrawMode a
)
{
    showBinds = a;
    return this;
}



bool Layer::getShowLayer()
{
    return showLayer;
}



/*
    Return sum of error
*/
double Layer::getError()
{
    double result = 0.0;

    for( int i = 0; i < count; i ++ )
    {
        result += abs( errors[ i ] );
    }

    return result;
}



/*
    Return sum of value
*/
double Layer::getValue()
{
    double result = 0.0;

    for( int i = 0; i < count; i ++ )
    {
        result += values[ i ];
    }

    return result;
}



/*
    Set bitmap to neurons
*/
Layer* Layer::bitmapToValue
(
    Bitmap* a
)
{
    auto w = a -> getWidth();
    auto h = a -> getHeight();

//    if( w > 0 && h > 0 && size.x > 0 && size.y > 0 )
//    {
//        double kx = (double) size.x / (double) w;
//        double ky = (double) size.y / (double) h;
//
//        double scale = min( kx, ky );
//
//        a -> scale( ( int ) ( w * scale ), (int) ( h * scale ) );
//    }
//    w = a -> getWidth();
//    h = a -> getHeight();

//    /* Shift */
//    int sx = ( size.x - w ) / 2;
//    int sy = ( size.y - h ) / 2;

    Rgba rgba;

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


    return this;
}



/*
    Load bitmap from source file name and set it
*/
Layer* Layer::imageToValue
(
    string aFileName
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
                getLog()
                -> warning( "File read error" )
                -> prm( "code", bitmap -> getCode() )
                -> prm( "message", bitmap -> getMessage() );
            }
            bitmap -> destroy();
        }
        else
        {
            getLog()
            -> warning( "Unknown image format" )
            -> prm( "extention", fileExt );
        }
    }
    else
    {
        getLog()
        -> warning( "Write unable image to the value because layer has the error" )
        -> prm( "code", getCode() );
    }
    return this;
}



Layer* Layer::applyUuid
(
    Hid a
)
{
    for( int i = 0; i < count; i++ )
    {
        setNeuronValue( i, a.getBit( i ) ? 1.0 : 0.0 );
    }
    return this;
}



/*
    Storage path
*/

Layer* Layer::setStoragePath
(
    string a
)
{
    storagePath = a;
    return this;
}



/*
*/
string Layer::getStoragePath()
{
    return storagePath;
}




string Layer::getLayerPath()
{
    return storagePath == "" ? "" : storagePath + "/" + getId();
}



/*
    Return layer file name
*/
string Layer::getStorageValueName()
{
    return storagePath == "" ? "" : ( getLayerPath() + "/value.bin" );
}



/*
    Return the source filename
*/
//string Layer::getSourceFilename()
//{
//    string result
//    = sourcePath == ""
//    ? getLayerPath() + "/source/"
//    : sourcePath;
//
//    vector <string> exts = { "uuid", "jpg", "json" };
//
//    auto list = getFileList( result );
//
//    for( int i; i < list.size(); i++ )
//    {
//        auto ext = getFileExt( list[ i ] );
//
//        if( find( exts.begin(), exts.end(), ext ) != exts.end() )
//        {
//            result = result + list[ i ];
//            break;
//        }
//    }
//    return result;
//}





/*
    Return outer box size
*/
Point3d& Layer::getOuterBox()
{
    return outerBox;
}



/*
    Write value to io
*/
Layer* Layer::writeValues()
{
    if( values != NULL )
    {
        auto io = Io::create( net );
        io -> getRequest()
        -> setString( "idLayer", this -> getId() )
        -> setData( "data", (char*)values, count * sizeof( double ) );

        io
        -> call( CMD_WRITE_VALUES )
        -> destroy();

        getLog() -> trace( "Write layer values" ) -> prm( "id", this -> getId() );
    }

    return this;
}




Layer* Layer::valuesFromBuffer
(
    char* aBuffer,
    size_t  aSize
)
{
    if( aBuffer != NULL && aSize == count * sizeof( double ) )
    {
        memcpy( values, aBuffer, aSize );
    }
    return this;
}



/*
    Return buffer and size of buffer of values
*/
Layer* Layer::getValuesBuffer
(
    char* &aBuffer, /* Buffer pointer */
    size_t &aSize   /* Size of buffer */
)
{
    aBuffer = ( char* )values;
    aSize = count * sizeof( double );
    return this;
}



/*
    Read value from io
*/
Layer* Layer::readValues()
{
    auto io = Io::create( net );

    io
    -> getRequest()
    -> setString( "idLayer", this -> getId() );

    if( io -> call( CMD_READ_VALUES ) -> isOk() )
    {
        char* buffer = NULL;
        size_t size = 0;

        io -> getAnswer() -> getData( "data", buffer, size );
        valuesFromBuffer( buffer, size );
        getLog() -> trace( "Read layer values" ) -> prm( "id", this -> getId() );
    }

    io -> destroy();

    return this;
}



/*
    Write errors to io
*/
Layer* Layer::writeErrors()
{
    if( errors != NULL )
    {
        getLog() -> begin( "Write layer errors" ) -> prm( "id", this -> getId() );
        auto io = Io::create( net );
        io -> getRequest()
        -> setString( "idLayer", this -> getId() )
        -> setData( "data", (char*)errors, count * sizeof( double ) );

        io
        -> call( CMD_WRITE_ERRORS )
        -> destroy();

        getLog() -> end();
    }

    return this;
}



/*
    Read value from io
*/
Layer* Layer::readErrors()
{
    auto io = Io::create( net );

    io
    -> getRequest()
    -> setString( "idLayer", this -> getId() );

    if( io -> call( CMD_READ_ERRORS ) -> isOk() )
    {
        char* buffer = NULL;
        size_t size = 0;
        io -> getAnswer() -> getData( "data", buffer, size );

        if( buffer != NULL && size == count * sizeof( double ) )
        {
            memcpy( errors, buffer, size );
            getLog() -> trace( "Read layer errors" ) -> prm( "id", this -> getId() );
        }
    }

    io -> destroy();

    return this;
}



Layer* Layer::errorsFromBuffer
(
    char* aBuffer,
    size_t aSize
)
{
    if( aBuffer != NULL && aSize == count * sizeof( double ) )
    {
        memcpy( errors, aBuffer, aSize );
    }
    return this;
}



/*
    Return buffer and size of buffer of errors
*/
Layer* Layer::getErrorsBuffer
(
    char* &aBuffer, /* Buffer pointer */
    size_t &aSize   /* Size of buffer */
)
{
    aBuffer = ( char* )errors;
    aSize = count * sizeof( double );
    return this;
}



/*
    Calculate start neuron for processors operations
*/
int Layer::calcNeuronFrom
(
    int aNumber,
    int aCount
)
{
    return floor
    (
        (double) count * (double) aNumber / (double) aCount
    );
}



/*
    Caluculate end of neurons for processors operations
*/
int Layer::calcNeuronTo
(
    int aNumber,
    int aCount
)
{
    return calcNeuronFrom( aNumber + 1, aCount );
}


/******************************************************************************
    Layer calculateion service
*/


Layer* Layer::calcReset()
{
    forward = -1;
    backward = -1;
    return this;
}



Layer* Layer::calcStartForward()
{
    forward = 0;
    return this;
}



Layer* Layer::calcStartBackward()
{
    backward = 0;
    return this;
}



Layer* Layer::calcCompleteForward()
{
    forward ++;
    return this;
}



Layer* Layer::calcCompleteBackward()
{
    backward ++;
    return this;
}



CalcStage Layer::getForwardStage
(
    int aThreadCount
)
{
    return forward == -1 ? CALC_NOT_START :
    (
        forward == aThreadCount ? CALC_COMPLETE : CALC_START
    );
}



CalcStage Layer::getBackwardStage
(
    int aThreadCount
)
{
    return backward == -1 ? CALC_NOT_START :
    (
        backward == aThreadCount ? CALC_COMPLETE : CALC_START
    );
}




/**********************************************************************
    Neurons setters and getters
*/



/*
    Set neuron value
*/
Layer* Layer::setNeuronValue
(
    int aIndex,            /* Index of neuron */
    double aValue          /* Value */
)
{
    if( values != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            values[ aIndex ] = aValue;
        }
        else
        {
            setResult( "IndexValueOutOfRangeForSet" );
        }
    }
    else
    {
        setResult( "ValueArrayNotDefinedForSet" );
    }

    return this;
}



/*
    Return neuron vaalue or default value
*/
double Layer::getNeuronValue
(
    int aIndex            /* Index of neuron */
)
{
    double result = 0.0;

    if( values != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            result = values[ aIndex ];
        }
        else
        {
            setResult( "IndexValueOutOfRangeForGet" );
        }
    }
    else
    {
        setResult( "ValueArrayNotDefinedForGet" );
    }
    return result;
}



/*
    Set neuron error
*/
Layer* Layer::setNeuronError
(
    int aIndex,            /* Index of neuron */
    double aValue          /* Value */
)
{
    if( errors != NULL && aIndex >= 0 && aIndex < count )
    {
        errors[ aIndex ] = aValue;
    }
    else
    {
        setResult( "SetIndexValueOutOfRange" );
    }
    return this;
}



/*
    Return neuron error or default value
*/
double Layer::getNeuronError
(
    int aIndex  /* Index of neuron */
)
{
    double result = 0.0;

    if( errors != NULL && aIndex >= 0 && aIndex < count )
    {
        result = errors[ aIndex ];
    }
    else
    {
        setResult( "GettingIndexValueOutOfRange" );
    }

    return result;
}



/*
    Set neuron world position
*/
Layer* Layer::setNeuronWorld
(
    int aIndex,           /* Index of neuron */
    Point3d& aValue       /* Value */
)
{
    if( world != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            world[ aIndex ] = aValue;
        }
        else
        {
            setResult( "IndexWorldOutOfRangeForSet" );
        }
    }
    else
    {
        setResult( "NeuronWorldArrayNotDefinedForSet" );
    }
    return this;
}



/*
    Return neuron world position or default value
*/
Point3d Layer::getNeuronWorld
(
    int aIndex  /* Index of neuron */
)
{
    auto result = POINT_3D_0;

    if( world != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            result = world[ aIndex ];
        }
        else
        {
            setResult( "IndexWorldPointsOutOfRangeForGet" );
        }
    }
    else
    {
        setResult( "WorldPointsNotDefinedforForGet" );
    }
    return result;
}



/*
    Set neuron screen position
*/
Layer* Layer::setNeuronScreen
(
    int aIndex,          /* Index of neuron */
    Point3d& aValue      /* Value */
)
{
    if( screen != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            screen[ aIndex ] = aValue;
        }
        else
        {
            setResult( "IndexWorldPointsOutOfRangeForSet" );
        }
    }
    else
    {
        setResult( "WorldPointsNotDefinedforForSet" );
    }
    return this;
}



/*
    Return neuron screen position or default value
*/
Point3d Layer::getNeuronScreen
(
    int aIndex  /* Index of neuron */
)
{
    auto result = POINT_3D_0;

    if( screen != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            result = screen[ aIndex ];
        }
        else
        {
            setResult( "IndexScreenPointsOutOfRangeForGet" );
        }
    }
    else
    {
        setResult( "ScreenPointsNotDefinedforForGet" );
    }
    return result;
}




/*
    Set neuron selected
*/
Layer* Layer::setNeuronSelected
(
    int aIndex,     /* Index of neuron */
    bool aValue     /* Value */
)
{
    if( selected != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            selected[ aIndex ] = aValue;
        }
        else
        {
            setResult( "IndexScreenPointsOutOfRangeForSet" );
        }
    }
    else
    {
        setResult( "ScreenPointsNotDefinedForSet" );
    }
    return this;
}



/*
    Return neuron selected or default value
*/
Point3d Layer::getNeuronSelected
(
    int aIndex     /* Index of neuron */
)
{
    auto result = false;

    if( selected != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            result = selected[ aIndex ];
        }
        else
        {
            setResult( "IndexSelectedOutOfRangeForGet" );
        }
    }
    else
    {
        setResult( "SelectedNotDefinedForGet" );
    }
    return result;
}




/*
    Loop for each parents of this neuron
*/
Layer* Layer::parentsLoop
(
    int aIndex,
    parentsLambda aCallback
)
{
    /* Loop by nerves */
    getNet() -> getNerves() -> loop
    (
        [ this, &aCallback, &aIndex ]
        ( void* aNerve )
        {
            auto iNerve = ( Nerve* ) aNerve;
            if( iNerve -> getChild() == this )
            {
                int from = 0;
                int to = 0;
                iNerve -> getWeightsRangeByChildIndex
                (
                    aIndex, from, to
                );
                /* Loop by weights */
                for( int i = from; i < to;  i++ )
                {
                    aCallback
                    (
                        iNerve -> getParent(),
                        iNerve -> getParentByWeightIndex( i ),
                        iNerve,
                        iNerve -> getWeight( i ),
                        i
                    );
                }
            }
            return false;
        }
    );
    return this;
}




/*
    Loop for each child of i neuron
*/
Layer* Layer::childrenLoop
(
    int aIndex,
    childrenLambda aCallback
)
{
    /* Loop by nerves */
    getNet() -> getNerves() -> loop
    (
        [ this, &aCallback, &aIndex ]
        ( void* aNerve )
        {
            auto iNerve = ( Nerve* ) aNerve;
            if( iNerve -> getParent() == this )
            {
                int from = 0;
                int to = 0;
                int step = 0;

                iNerve -> getWeightsRangeByParentIndex
                (
                    aIndex, from, to, step
                );

                /* Loop by weights */
                for( int i = from; i < to;  i += step )
                {
                    aCallback
                    (
                        iNerve -> getChild(),
                        iNerve -> getChildByWeightIndex( i ),
                        iNerve,
                        iNerve -> getWeight( i ),
                        i
                    );
                }
            }
            return false;
        }
    );
    return this;
}



/*
    Calculate neuron
*/
Layer* Layer::neuronCalcValue
(
    int aIndex
)
{
    double summValue        = 0.0;
    double summCommand      = 0.0;
    double summSample       = 0.0;
    double summErrorValue   = 0.0;
    int countSample         = 0;
    int countValue          = 0;
    int countErrorValue     = 0;

    /* Neuron has a binds and it is not a Receptor */
    parentsLoop
    (
        aIndex,
        [
            this,
            &summValue,
            &summSample,
            &summCommand,
            &countSample,
            &countValue,
            &countErrorValue,
            &summErrorValue
        ]
        (
            Layer* aParentLayer,
            int aParentIndex,
            Nerve* aNerve,
            double aWeight,
            int aWeightIndex /* not use */
        ) -> bool
        {
            auto w = aParentLayer -> getNeuronValue( aParentIndex ) * aWeight;
            /* Calculate summ */
            switch( aNerve -> getBindType())
            {
                case BT_VALUE:
                    summValue += w;
                    countValue ++;
                break;
                case BT_ERROR_TO_VALUE:
                    summErrorValue += abs
                    (
                        aParentLayer -> getNeuronError( aParentIndex ) * aWeight
                    );
                    countErrorValue++;
                break;
                case BT_COMMAND:
                    summCommand += w;
                break;
                case BT_SAMPLE:
                    summSample += w;
                    countSample ++;
                break;
            }
            return false;
        }
    );

    if( countValue > 0 )
    {
        setNeuronValue( aIndex, FUNC_SIGMOID_LINE_ZERO_PLUS( summValue, 1.0 ));
    }

    if( countErrorValue > 0 )
    {
        /* Write avg error from parent to layer */
        setNeuronValue( aIndex, summErrorValue / countErrorValue );
    }

    if( countSample > 0 )
    {
        setNeuronError
        (
            aIndex,
            FUNC_SIGMOID_LINE_MINUS_PLUS
            (
                ( summSample - getNeuronValue( aIndex ) ) *
                ( summCommand > EPSILON_D ? 1.0 : 0.0 ),
                1.0
            )
        );
    }

    return this;
}



/*
    Calculate neuron
*/
Layer* Layer::neuronLearning
(
    int aIndex,
    double  aErrorNormalize,    /**/
    double  aLearningSpeed,     /* k for weight changing */
    double  aWakeupWeight
)
{
    /* Define variables */
    double summWeight   = 0.0;
    double summError    = 0.0;
    int countValue      = 0;

    /* Caclulate error form all children for current neuron */
    childrenLoop
    (
        aIndex,
        [
            this,
            &summWeight,
            &summError,
            &countValue,
            &aIndex
        ]
        (
            Layer* aChild,
            int aChildIndex,
            Nerve* aNerve,
            double aWeight,
            int aWeightIndex    /* Not use */
        ) -> bool
        {
            switch( aNerve -> getBindType())
            {
                case BT_VALUE:
                    summError += aChild -> getNeuronError( aChildIndex ) * aWeight;
                    summWeight += abs( aWeight );
                break;
            }
            return false;
        }
    );

    /* Neuron error is calculated */
    /* If children neurons afected the parent neuron ... */
    if( summWeight > EPSILON_D )
    {
        /*
            Сумму ошибок всех дочерних нейронов и делим
            на сумму весов для дочерних нейронов + 1
            что бы исклюить деление на 0. Считаем что +1 это мелоч
            aErrorNormalize - сила распространения ошибки.
        */
        setNeuronError
        (
            aIndex,
            FUNC_SIGMOID_LINE_MINUS_PLUS
            (
                summError / ( 1.0 + summWeight * aErrorNormalize ),
                1.0
            )
        );
    }

    /* Learning */
    parentsLoop
    (
        aIndex,
        [
            this,
            &aLearningSpeed,
            &aWakeupWeight,
            &aIndex
        ]
        (
            Layer*  aParentLayer,
            int     aParentIndex,
            Nerve*  aNerve,
            double  aWeight,
            int     aWeightIndex
        ) -> bool
        {
            switch( aNerve -> getBindType())
            {
                case BT_VALUE:
                {
                    /*
                        Calculate delta
                    */
                    double w = abs( aWeight );
                    /* aWakeupWeight epsilon for zero weight подъем нулевых связей */
                    double wv = ( w < aWakeupWeight ? aWakeupWeight : w )
                    * aParentLayer -> getNeuronValue( aParentIndex );
                    double deltaWeight = getNeuronError( aIndex ) * wv;
                    aNerve -> setWeight
                    (
                        aWeightIndex,
                        FUNC_SIGMOID_LINE_MINUS_PLUS
                        (
                            aWeight + deltaWeight * aLearningSpeed,
                            1.0
                        )
                    );
                }
            }
            return false;
        }
    );

    return this;
}



/*
    Return event actions
*/
ParamList* Layer::getActions()
{
    return actions;
}



/******************************************************************************
    Fill layer
*/



/*
    Noise fill values of layer neurons
*/
Layer* Layer::noiseValue
(
    int     aSeed,
    double  aMin,
    double  aMax
)
{
    Rnd::storeSeed( aSeed );

    for( int i = 0; i < count; i ++ )
    {
        setNeuronValue( i, Rnd::get( aMin, aMax ) );
    }

    Rnd::restoreSeed();

    return this;
}



//
///* Move values and errors data to processor cache */
//Layer* Layer::dataToCache()
//{
//    if( count != cacheCount )
//    {
//        if( errorCache != NULL )
//        {
//            delete errorsCache valuesCache;
//        }
//
//    }
//    return this;
//}
//
//
//
///* Move values and errors from processor cache to data */
//Layer* Layer::cacheToData()
//{
//    return this;
//}
