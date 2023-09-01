#include "../lib/rnd.h"
#include "../lib/moment.h"

#include "../graph/point3.h"
#include "../lib/rpc_client.h"

#include "rpc_protocol.h"

#include "func.h"
#include "layer.h"
#include "neuron.h"
#include "net.h"



/*
    Constructor
*/
Layer::Layer
(
    Net* aNet,
    string aId
)
{
    net = aNet;
    id = aId == "" ? Rnd::getUuid() : aId;
    getLog() -> trace( "Create layer" ) -> prm( "id", id );

    /* Create list of neurons */
    neurons = new NeuronList();

}



/*
    Internal destructor
*/
Layer::~Layer()
{
    /* Destroy neurons */
    setSize();

    /* Destroy the list of neurons */
    delete neurons;

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
    string aId
)
{
    return new Layer( aNet, aId );
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



Neuron* Layer::neuronByPos
(
    const Point3i& a
)
{
    return neurons -> getByIndex( indexByPos( a ));
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
                    Neuron* n = neurons -> getByIndex( i );
                    n -> setWorldPoint( p );
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

        pointsRecalc = true;
    }

    return this;
}



/*
    Clear values for all neurons
*/
Layer* Layer::clearValues()
{
    /* Calculate neurons */
    neurons -> loop
    (
        []( Neuron* iNeuron ) -> bool
        {
            /* Calc neuron */
            iNeuron -> setValue( 0 );
            return false;
        }
    );
    return this;
}



Layer* Layer::calcValue
(
    int aProcessorNumber,
    int aProcessorCount
)
{
    /* Calculate neurons */
    neurons -> loop
    (
        []( Neuron* neuron ) -> bool
        {
            /* Calc neuron */
            neuron -> calcValue();
            return false;
        },
        calcNeuronFrom( aProcessorNumber, aProcessorCount ),
        calcNeuronTo( aProcessorNumber, aProcessorCount )
    );
    return this;
}



/*
    Calculate neurons error for learning
*/
Layer* Layer::learning
(
    double  aErrorNormalize,
    double  aLearningSpeed,
    double  aWakeupWeight
)
{
//    bool calculatedErrorFinish = true;
//
//    /* Calculate neurons */
//    neurons -> loop
//    (
//        [
//            &calculatedErrorFinish,
//            &aLoopParity,
//            &aErrorNormalize,
//            &aLearningSpeed,
//            &aWakeupWeight
//        ]
//        ( Neuron* neuron ) -> bool
//        {
//            if( neuron -> getLoopParityError() != aLoopParity )
//            {
//                /* Calc neuron */
//                neuron -> learning
//                (
//                    aLoopParity,
//                    aErrorNormalize,
//                    aLearningSpeed,
//                    aWakeupWeight
//                );
//                /* Accumulate layer finish value */
//                calculatedErrorFinish
//                = calculatedErrorFinish
//                && ( neuron -> getLoopParityError() == aLoopParity );
//            }
//            return false;
//        }
//    );
//
//    /* Fixing the loop parity for layer */
//    if( calculatedErrorFinish )
//    {
//        loopParityError = aLoopParity;
//    }

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
    auto currentSize = neurons -> getCount();
    auto newSize = a.x * a.y * a.z;
    if( newSize != currentSize )
    {
        /* The size was changed */
        getLog()
        -> begin( "Layer resize" )
        -> prm( "id", getNameOrId() )
        -> prm( "from", currentSize )
        -> prm( "to", newSize )
        -> lineEnd();

        if( newSize > currentSize )
        {
            /* Size increase */
            neurons -> resize( newSize );
            /* Neurons create */
            for( int i = currentSize; i < newSize; i++ )
            {
                /*
                    Create neuron.
                    It must be the once point for this operation
                */
                Neuron::create( this, i )
                -> setValue( 0.0 );
            }
        }
        else
        {
            if( newSize < currentSize )
            {
                /* Size decrease */
                for( int i = newSize; i < currentSize; i++ )
                {
                    auto n = neurons -> getByIndex( i );
                    if( !n -> isNull())
                    {
                        delete n;
                    }
                }
                neurons -> resize( newSize );
            }
        }

        size = a;

        /* Recalculate neuron points */
        neuronPointsCalc( true );

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
    }
    return this;
}



/*
    Return neuron list
*/
NeuronList* Layer::getNeurons()
{
    return neurons;
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
        pointsRecalc = false;
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
    drawSize = a;
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
    Set loop parity for error and value
*/
Layer* Layer::setLoopParity
(
    bool a
)
{
    loopParityValue = a;
    loopParityError = a;

    return this;
}



/*
    Return parity of loop for current layer
*/
bool Layer::getLoopParityValue()
{
    return loopParityValue;
}



/*
    Return parity of loop for current layer
*/
bool Layer::getLoopParityError()
{
    return loopParityError;
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
    NeuronList* buffer = NeuronList::create();
    buffer -> resize( neurons -> getCount() );

    int iBuffer = 0;

    neurons -> loop
    (
        [ buffer, &aTopLeft, &aBottomRight, &iBuffer ]
        (
            Neuron* neuron
        ) -> bool
        {
            if( neuron -> getScreenPoint().testRectXY( aTopLeft, aBottomRight ))
            {
                buffer -> setByIndex( iBuffer, neuron );
                iBuffer++;
            }
            return false;
        }
    );

    buffer -> resize( iBuffer );
    aList -> add( buffer );
    buffer -> destroy();

    return this;
}



/*
    Return list of neurons around  the screen position
*/
Layer* Layer::getNeuronsByScreenPos
(
    NeuronList* aList,
    const Point3d& aPosition
)
{
    NeuronList* buffer = NeuronList::create();
    buffer -> resize( neurons -> getCount() );

    int iBuffer = 0;

    neurons -> loop
    (
        [ buffer, &aPosition, &iBuffer ]
        (
            Neuron* neuron
        ) -> bool
        {
            if( neuron -> getScreenPoint().distXY( aPosition ) < 10 )
            {
                buffer -> setByIndex( iBuffer, neuron );
                iBuffer++;
            }
            return false;
        }
    );

    buffer -> resize( iBuffer );
    aList -> add( buffer );
    buffer -> destroy();

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
    return neurons -> getCount();
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

    neurons -> loop
    (
        [ &result ]
        (
            Neuron* neuron
        ) -> bool
        {
            result += abs( neuron -> getError() );
            return false;
        }
    );

    return result;
}



/*
    Return sum of value
*/
double Layer::getValue()
{
    double result = 0.0;

    neurons -> loop
    (
        [ &result ]
        (
            Neuron* neuron
        ) -> bool
        {
            result += neuron -> getValue();
            return false;
        }
    );

    return result;
}



/*
    Set source filename
*/
Layer* Layer::setSourcePath
(
    string a
)
{
    sourcePath = a;
    return this;
}



/*
    Return the source filename
*/
string Layer::getSourcePath()
{
    return sourcePath;
}



/*
    Set bitmap to neurons
*/
Layer* Layer::setBitmap
(
    Bitmap* a
)
{
    auto w = a -> getWidth();
    auto h = a -> getHeight();

    if( w > 0 && h > 0 && size.x > 0 && size.y > 0 )
    {
        double kx = (double) size.x / (double) w;
        double ky = (double) size.y / (double) h;

        double scale = min( kx, ky );

        a -> scale( ( int ) ( w * scale ), (int) ( h * scale ) );
    }

    w = a -> getWidth();
    h = a -> getHeight();

    /* Shift */
    int sx = ( size.x - w ) / 2;
    int sy = ( size.y - h ) / 2;

    Rgba rgba;

    for( int z = 0; z < size.z; z++ )
    {
        for( int y = 0; y < size.y; y++ )
        {
            for( int x = 0; x < size.x; x++ )
            {
                a -> getRgba( x, y, rgba );
                auto n = neuronByPos( Point3i( x + sx , y + sy, z ));
                if( n != NULL)
                {
                    n -> setValue( rgba.getGray() );
                }
            }
        }
    }

    return this;
}



/*
    Load bitmap from source file name and set it
*/
Layer* Layer::loadSource()
{
    auto fileName = getSourceFilename();
    auto fileExt = getFileExt( fileName );

    if( fileExt == "jpg" )
    {
        /* Load from bitmap */
        auto bitmap = Bitmap::create() -> load( fileName );

        if( bitmap -> isOk() )
        {
            getLog()
            -> trace( "Load image" )
            -> prm( "Layer", getNameOrId() )
            -> prm( "File", getSourceFilename() )
            -> prm( "width", bitmap -> getWidth() )
            -> prm( "height", bitmap -> getHeight() )
            -> lineEnd();
            setBitmap( bitmap );
        }
        else
        {
            getLog() -> warning( bitmap -> getMessage() );
        }
        bitmap -> destroy();
    }
    else
    {
        if( fileExt == "uuid" )
        {
            /* Load uuid from file */
            Hid h;
//            h.loadUuid( fileName );
//            applyUuid( h );
        }
        else
        {
            if( fileExt == "json" )
            {
                /* Load from json file */
            }
            else
            {
                /* Reset layer */
                clearValues();
            }
        }
    }

    return this;
}



Layer* Layer::applyUuid
(
    Hid a
)
{
    for( int i = 0; i < neurons -> getCount(); i ++ )
    {
        neurons -> getByIndex( i ) -> setValue( a.getBit( i ) ? 1.0 : 0.0 );
    }
    return this;
}


/*
    Apply bitmap from source file name and set it
*/
Layer* Layer::applyImage()
{
    auto update = fileLastUpdate( getSourceFilename() );
    if( update != lastSourceUpdate )
    {
        loadSource();
        lastSourceUpdate = update;
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
string Layer::getSourceFilename()
{
    string result
    = sourcePath == ""
    ? getLayerPath() + "/source/"
    : sourcePath;

    vector <string> exts = { "uuid", "jpg", "json" };

    auto list = getFileList( result );

    for( int i; i < list.size(); i++ )
    {
        auto ext = getFileExt( list[ i ] );

        if( find( exts.begin(), exts.end(), ext ) != exts.end() )
        {
            result = result + list[ i ];
            break;
        }
    }
    return result;
}





/*
    Return outer box size
*/
Point3d& Layer::getOuterBox()
{
    return outerBox;
}



/*
    Load value and error from storage file
*/
Layer* Layer::loadValue()
{
    if(  neurons -> isOk() )
    {
        auto valueFile = getStorageValueName();
        if( !neurons -> loadFromStorage( valueFile ) -> isOk())
        {
            getLog() -> warning( neurons -> getCode() )-> prm( "layer", getNameOrId() );
            neurons -> setOk();
        }
    }
    return this;
}



/*
    Save value and error to storage file
*/
Layer* Layer::saveValue()
{
    if(  neurons -> isOk() )
    {
        auto file = getStorageValueName();
        if( !neurons -> saveToStorage( file ) -> isOk())
        {
            getLog() -> warning( neurons -> getCode() )-> prm( "layer", getNameOrId() );
            neurons -> setOk();
        }
    }
    return this;
}



/*
    Send layer to server
*/
Layer* Layer::writeToServer
(
    string aHost,               /* server ip address */
    int aPort,                  /* server port number */
    int aProcessorNumber,       /* Processor Number */
    int aProcessorCount,        /* Processor count */
    string aIdClient,           /* Client ID*/
    CalcDirection aDirection    /* Direction of calculation */
)
{
    if( neurons -> isOk() )
    {
        auto neuronFrom = calcNeuronFrom( aProcessorNumber, aProcessorCount );
        auto neuronTo = calcNeuronTo( aProcessorNumber, aProcessorCount );

        char* buffer = NULL;
        size_t size = 0;

        getNeurons() -> saveToBuffer
        (
            neuronFrom,
            neuronTo,
            buffer,
            size
        );


        if( buffer != NULL )
        {
            auto rpc = RpcClient::create( getLog(), aHost, aPort );
            rpc -> getRequest()
            -> setString( "idLayer", this -> getId() )
            -> setString( "idClient", aIdClient )
            -> setInt( "direction", aDirection )
            -> setInt( "from", neuronFrom )
            -> setInt( "to", neuronTo )
            -> setData( "data", buffer, size );

            rpc -> call( CMD_WRITE_LAYER );
            rpc -> destroy();

            /* Delete buffer */
            delete [] buffer;
        }
    }
    return this;
}



/*
    Request layer from server
*/
Layer* Layer::readFromServer
(
    string aHost,       /* server ip address */
    int aPort           /* server port number */
)
{
    if( neurons -> isOk() )
    {
        auto rpc = RpcClient::create( getLog(), aHost, aPort );
        rpc
        -> getRequest()
        -> setString( "idLayer", this -> getId() );

        if( rpc -> call( CMD_READ_LAYER ) -> isOk() )
        {
            char* buffer = NULL;
            size_t size = 0;
            rpc -> getAnswer() -> getData( "data", buffer, size );
            if( buffer != NULL && size > 0 )
            {
                getNeurons() -> loadFromBuffer( buffer, size );
            }
        }
        rpc -> destroy();
    }

    return this;
}



/*
    Return read role
*/
bool Layer::getRead()
{
    return read;
}



/*
    Set read role
*/
Layer* Layer::setRead
(
    bool a /* value */
)
{
    read = a;
    return this;
}



/*
    Return write role
*/
bool Layer::getWrite()
{
    return write;
}



/*
    Set write role
*/
Layer* Layer::setWrite
(
    bool a /* value */
)
{
    write = a;
    return this;
}



/*
    Return calc role
*/
bool Layer::getCalc()
{
    return calc;
}



/*
    Return calc role
*/
Layer* Layer::setCalc
(
    bool a /* value */
)
{
    calc = a;
    return this;
}



/*
    Return send role
*/
bool Layer::getFromServer()
{
    return fromServer;
}



/*
    Return send role
*/
Layer* Layer::setFromServer
(
    bool a /* value */
)
{
    fromServer = a;
    return this;
}



/*
    Return send role
*/
bool Layer::getToServer()
{
    return toServer;
}



/*
    Return send role
*/
Layer* Layer::setToServer
(
    bool a /* value */
)
{
    toServer = a;
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
    return floor((double) (getNeurons() -> getCount()) * (double)aNumber / (double)aCount);
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


/********************************************************************
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




