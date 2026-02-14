#include <fstream>
#include <unistd.h> // для close()
#include <sys/file.h>

#include "limb.h"
#include "../../../../lib/core/math.h"
#include "../../../../lib/core/str.h"


/*
    Constructor
*/
Limb::Limb
(
    LogManager* aLogManager,
    string aVersion
)
{
    /* Set properties */
    logManager = aLogManager;

    version = aVersion;

    /* Create layers and nerves structures */
    layers = LayerList::create( this );
    nerves = NerveList::create( logManager );
}



/*
    Destructor
*/
Limb::~Limb()
{
    nerves -> clear() -> destroy();
    layers -> clear() -> destroy();
}



/*
    Create
*/
Limb* Limb::create
(
    /* Log object*/
    LogManager* aLogManager,
    string aVersion
)
{
    return new Limb( aLogManager, aVersion );
}



/*
    Destroy
*/
void Limb::destroy()
{
    delete this;
}



/*
    Return log object
*/
Log* Limb::getLog()
{
    return logManager -> getLog();
}



/*
    Return log object
*/
LogManager* Limb::getLogManager()
{
    return logManager;
}



/*
    Create new layer
*/
Layer* Limb::createLayer
(
    string aId /* Id of layer */
)
{
    Layer* result = NULL;

    lock();

    auto layers = getLayerList();
    int layerIndex = layers -> getIndexById( aId );

    if( layerIndex > -1 )
    {
        /* Return exists layer object */
        result = layers -> getByIndex( layerIndex );
    }
    else
    {
        /* Create new layer object */
        result = Layer::create( this, aId );
        layers -> push( result );

        lastChangeStructure = now();
    }

    unlock();

    return result;
}



/*
    Delete layer by Id
*/
Limb* Limb::deleteLayer
(
    string a /* Id of layer */
)
{
    auto layers = getLayerList();
    auto nerves = getNerveList();
    int layerIndex = layers -> getIndexById( a );
    if( layerIndex > -1 )
    {
        /* Define layer for remove */
        Layer* layer = layers -> getByIndex( layerIndex );

        /* Destroy nerves for layer */
        nerves -> removeByLayer( layer );

        /* Remove layer from layer list */
        layers -> remove( layerIndex );

        /* Destroy layer */
        layer -> destroy();

        lastChangeStructure = now();
    }
    return this;
}



/*
    Create new nerve
*/
Nerve* Limb::createNerve
(
    /* Layer source */
    Layer*      aLayerFrom,
    /* Layer destination */
    Layer*      aLayerTo,
    /* Type of nerve */
    NerveType   aNerveType,
    /* Type of bind */
    BindType    aBindType,
    /* Window size for some-to-some */
    Point3i     aSize
)
{
    getLog() -> begin( "Nerve create" );
    auto result = Nerve::create
    (
        logManager,
        aLayerFrom,
        aLayerTo,
        aNerveType,
        aBindType,
        aSize
    );
    nerves -> push( result );
    lastChangeStructure = now();
    getLog() -> end();
    return result;
}



/*
    Delete layer by Id
*/
Limb* Limb::deleteNerve
(
    Nerve* aNerve /* Nerve opbject */
)
{
    if( aNerve != NULL )
    {
        /* Remove nerve from net list */
        nerves -> remove( aNerve );

        /* Destroy the nerve */
        aNerve -> destroy();

        lastChangeStructure = now();
    }
    return this;
}




/**********************************************************************
    Limb synchronization
*/

bool Limb::copyTo
(
    Limb* aLimb,
    bool aStrictSync,
    bool aSkipLockThis,
    bool aSkipLockLimb
)
{
    bool result = false;

    if( aLimb != this )
    {
        if( lock( /* aSkipLockThis */ ))
        {
            if( aLimb -> lock( /* aSkipLockLimb */ ))
            {
                auto layersIsEqual = layers -> compare( aLimb -> getLayerList() );
                auto nervesIsEqual = layersIsEqual && nerves -> compare( aLimb -> getNerveList() );

                if( aStrictSync )
                {
                    if( !layersIsEqual )
                    {
                        /* Copy structure of layers */
                        aLimb -> copyStructureFrom( this -> getLayerList() );
                        layersIsEqual = true;
                        result = true;
                    }
                    if( !nervesIsEqual )
                    {
                        /* Copy structure of nerves */
                        aLimb
                        -> getNerveList()
                        -> copyStructureFrom( this -> getNerveList(), aLimb -> getLayerList() );
                        nervesIsEqual = true;
                        result = true;
                    }
                }

                if( layersIsEqual && nervesIsEqual )
                {
                    aLimb
                    -> getLayerList()
                    -> copyValuesFrom( this -> getLayerList() )
                    -> copyErrorsFrom( this -> getLayerList() )
                    ;
                }

                /* Data sinchronization */
                aLimb -> unlock();
            }
            unlock();
        }
    }
    else
    {
        getLog() -> warning( "UnableLimbItselfCopyTo" );
    }

    return result;
}



/**********************************************************************
    Setters and getters
*/


/**********************************************************************
    Current age of the limbs config
*/

/*
    Increamet age of limb
*/
Limb* Limb::setLastUpdate
(
    long long aValue
)
{
    lastUpdate = aValue;
    return this;
}


long long Limb::getLastUpdate()
{
    return lastUpdate;
}



/*
    Copy list of layers
*/
Limb* Limb::copyStructureFrom
(
    LayerList* aSource
)
{
    nerves -> clear();
    layers -> clear();

    aSource -> loop
    (
        [ this ]
        ( void* p )
        {
            /* Create new layer object and push it to this*/
            auto iLayer = (Layer*) p;
            auto nLayer = copyLayerFrom( iLayer );
            nLayer -> setName( iLayer -> getName());
            layers -> push( nLayer );
            return false;
        }
    );
    return this;
}



/*
    Create new layer for this limb and copy parameters from source layer.
    This method have to overriden at children Limbs.
*/
Layer* Limb::copyLayerFrom
(
    Layer* aLayerFrom
)
{
    return
    Layer::create( this, aLayerFrom -> getId() )
    -> setErrorCalc( aLayerFrom -> getErrorCalc() )
    -> setWeightCalc( aLayerFrom -> getWeightCalc() )
    -> setFrontFunc( aLayerFrom -> getFrontFunc() )
    -> setBackFunc( aLayerFrom -> getBackFunc() )
    -> setBackFuncOut( aLayerFrom -> getBackFuncOut() )
    -> setSize( aLayerFrom -> getSize() );
}




/*
    Configuration postprocessing
*/
void Limb::onAfterReconfig
(
    ParamList* aConfig
)
{
}



/*
    Check layer parents existing by type
*/
bool Limb::layerParentsExists
(
    Layer* aLayer,
    BindType aBindType
)
{
    bool result = false;
    nerves -> loop
    (
        [ &aLayer, &aBindType, &result ]
        ( void* item )
        {
            auto iNerve = ( Nerve* )item;
            result
            = iNerve -> getChild() == aLayer
            && iNerve -> getBindType() == aBindType;
            return result;
        }
    );
    return result;
}



string Limb::getDumpFile
(
    /* File name masque цшер %% */
    string          aFile,
    /* Number of tick */
    long long int   aTick,
    /* Stage */
    CalcStage       aStage,
    /* The layer */
    Layer*          aLayerFrom,
    /* Neuron Index in the layer */
    Layer*          aLayerTo,
    /* Neuron Index in the layer */
    Point3i         aNeuronPos,
    /* Type parent or child */
    Direction       aDirection,
    /* Data type */
    Data            aData,
    /* Data view*/
    Dataview        aDataview
)
{
    /* Replace file name */
    return replace
    (
        aFile,
        vector <string>
        {
            "%tick%",
            "%stage%",
            "%layer-from%",
            "%layer-to%",
            "%direction%",
            "%data%",
            "%neuron%"
        },
        vector <string>
        {
            toString( aTick ),
            calcStageToString( aStage ),
            aLayerFrom -> getId(),
            aLayerTo -> getId(),
            directionToString( aDirection ),
            dataToString( aData ),
            aNeuronPos.toString()
        }
    );
}



/*
    On cahnge event for limb
*/
Limb* Limb::onChangeValues()
{
    lastChangeValues = now();
    return this;
}



/*
    Dump errors or values of the layer in to the file
*/
Limb* Limb::dumpLayer
(
    /* Store path */
    string          aPath,
    /* The layer */
    Layer*          aLayer,
    /* Data type */
    Data            aData,
    /* Data view*/
    Dataview        aDataview,
    /* Tick number */
    int             aTick,
    /* Colored */
    bool            aColored
)
{
    aPath = replace( aPath, "%layer%", aLayer -> getId() );
    if( checkPath( getPath( aPath )))
    {
        /* Open file stream */
        ofstream f;
        f.open( aPath );
        if( f.is_open() )
        {
            f << getFile( aPath ) << " tick: " <<  aTick << endl;

            auto c = aLayer -> getCount();
            auto size = aLayer -> getSize();

            for( int i = 0; i < c; i++ )
            {
                real val = 0.0;

                switch( aData )
                {
                    default:
                        val = 0;
                    break;
                    case DATA_VALUES:
                        val = aLayer -> getNeuronValue( i );
                    break;
                    case DATA_ERRORS:
                        val = aLayer -> getNeuronError( i );
                    break;
                }

                string delimiter = "";

                switch( aDataview )
                {
                    default:
                    case DATAVIEW_FLOAT:
                        f << toString( val, 4, DF_FIXED, aColored );
                        delimiter = "|";
                    break;
                    case DATAVIEW_CHAR:
                        f << valueToChar( val, aColored, VALUE_CHAR );
                        delimiter = "";
                    break;
                    case DATAVIEW_GRAPH:
                        f << valueToChar( val, aColored, VALUE_GRAPH );
                        delimiter = "";
                    break;
                }

                /* Check new line or delimiter */
                auto p = Point3i::byIndex( i, size );
                f << (( p.x == size.x - 1 ) ? "\n" : delimiter );
            }
            f.close();
        }
    }

    return this;
}



/*
    Dump weights of bind or errors | values of target neurons
    from the neuron in to the file
*/
Limb* Limb::dumpNeuron
(
    /* File masque */
    string          aFile,
    /* The layer */
    Layer*          aLayer,
    /* Neuron Index in the layer */
    Point3i         aNeuronPos,
    /* Data type */
    Data            aData,
    /* Data view*/
    Dataview        aDataview
)
{
    aFile = replace( aFile, "%neuron%", aNeuronPos.toString() );

    /* Parent direction */
    Layer* lastParentLayer = NULL;

    auto neuronIndex = aLayer -> indexByPos( aNeuronPos );

    auto file = replace
    (
        aFile,
        vector<string>{ "%layer%" },
        vector<string>{ aLayer -> getId() }
    );

    if( checkPath( getPath( file )))
    {
        /* Open file stream */
        ofstream f;
        f.open( file );
        if( f.is_open() )
        {
            /* Old neuron position */
            auto old = Point3i( -1, -1, -1 );
            auto size = POINT_3I_0;

            f << getFile( file ) << endl;

            parentsLoop
            (
                aLayer,
                neuronIndex,
                BT_ALL,
                [
                    &f,
                    aData,
                    &size,
                    &lastParentLayer,
                    &aLayer,
                    &aNeuronPos,
                    &old
                ]
                (
                    Layer*  aParentLayer,
                    int     aParentNeuronIndex,
                    Nerve*  aNerve,
                    real    aWeight,
                    int     aWeightIndex
                )
                {
                    if( aParentLayer != lastParentLayer )
                    {
                        /* Skip first enter */
                        if( lastParentLayer != NULL ) f << endl;

                        /* Start new layer table */
                        f
                        << INK_DEFAULT
                        << "Parent "
                        << dataToString( aData )
                        << " : "
                        << INK_YELLOW
                        << aParentLayer -> getId()
                        << INK_DEFAULT
                        << " ->  "
                        << INK_YELLOW
                        << aLayer -> getId()
                        << INK_DEFAULT
                        << aNeuronPos.toString( aLayer -> getSize())
                        ;

                        lastParentLayer = aParentLayer;
                        size = aParentLayer -> getSize();
                    }
                    auto p = Point3i::byIndex( aParentNeuronIndex, size );
                    /* Check new line or delimiter */
                    f << (( p.y == old.y ) ? " | " : "\n" );

                    old = p;
                    real val = 0.0;

                    switch( aData )
                    {
                        default:
                        case DATA_INDEX_WEIGHTS:
                        break;
                        case DATA_WEIGHTS:
                            val = aWeight;
                        break;
                        case DATA_VALUES:
                            val = aParentLayer -> getNeuronValue( aParentNeuronIndex );
                        break;
                        case DATA_ERRORS:
                            val = aParentLayer -> getNeuronError( aParentNeuronIndex );
                        break;
                    }

                    f
                    << p.toString( size )
                    << INK_GREY
                    << "="
                    << INK_DEFAULT
                    << toString( val, 4, DF_FIXED, true );

                    return false;
                }
            );

            f << endl;


            /* Child direction */
            Layer* lastChildLayer = NULL;
            size = POINT_3I_0;
            old = Point3i( -1, -1, -1 );

            childrenLoop
            (
                aLayer,
                neuronIndex,
                BT_ALL,
                [
                    &f,
                    &aData,
                    &size,
                    &lastChildLayer,
                    &aLayer,
                    &aNeuronPos,
                    &old
                ]
                (
                    Layer*  aChildLayer,
                    int     aChildNeuronIndex,
                    Nerve*  aNerve,
                    real    aWeight,
                    int     aWeightIndex
                )
                {
                    if( aChildLayer != lastChildLayer )
                    {
                        /* Skip first enter */
                        if( lastChildLayer != NULL ) f << endl;

                        /* Start new layer table */
                        f
                        << INK_DEFAULT
                        << "Child "
                        << dataToString( aData )
                        << " : "
                        << INK_YELLOW
                        << aLayer -> getId()
                        << aNeuronPos.toString( aLayer -> getSize())
                        << " ->  "
                        << INK_YELLOW
                        << aChildLayer -> getId()
                        << INK_DEFAULT
                        ;

                        lastChildLayer = aChildLayer;
                        size = aChildLayer -> getSize();
                    }
                    auto p = Point3i::byIndex( aChildNeuronIndex, size );

                    /* Check new line or delimiter */
                    f << (( p.y == old.y ) ? " | " : "\n" );

                    old = p;
                    real val = 0.0;

                    switch( aData )
                    {
                        default:
                        case DATA_INDEX_WEIGHTS:
                        break;
                        case DATA_WEIGHTS:
                            val = aWeight;
                        break;
                        case DATA_VALUES:
                            val = aChildLayer -> getNeuronValue( aChildNeuronIndex );
                        break;
                        case DATA_ERRORS:
                            val = aChildLayer -> getNeuronError( aChildNeuronIndex );
                        break;
                    }

                    f
                    << p.toString( size )
                    << INK_GREY
                    << "="
                    << INK_DEFAULT
                    << toString( val, 4, DF_FIXED, true )
                    ;

                    return false;
                }
            );

            f << endl;
            f.close();
        } /* File open */
    } /* Path exists */

    return this;
}



/*
    Dump nerve information in to the file
*/
Limb* Limb::dumpNerve
(
    /* Store path */
    string          aPath,
    /* The layer */
    Nerve*          aNerve,
    /* Data type */
    Data            aData,
    /* Data view*/
    Dataview        aDataview,
    /* Colored */
    bool            aColored
)
{
    if( checkPath( getPath( aPath )))
    {
        int fd = open( aPath.c_str(), O_RDWR | O_CREAT, 0666 );
        if (fd != -1)
        {
            if( flock( fd, LOCK_EX ) == 0 )
            {
                /* Open file stream */
                ofstream f( aPath );

                Layer* from = aNerve -> getParent();
                Layer* to = aNerve -> getChild();
                int pc = from -> getCount();
                int cc = to -> getCount();

                f << getFile( aPath ) << endl;

                for( int c = 0; c < cc; c++ )
                {
                    vector <string> line = {};
                    for( int p = 0; p < pc; p++ )
                    {
                        auto index = aNerve -> getWeightIndex( p, c );

                        switch( aData )
                        {
                            case DATA_INDEX_WEIGHTS:
                                line.push_back
                                (
                                    toString( index, aColored, 2 )
                                );
                            break;
                            case DATA_WEIGHTS:
                                line.push_back
                                (
                                    index >= 0 ?
                                    toString
                                    (
                                        aNerve -> getWeight( index ),
                                        4,
                                        DF_FIXED,
                                        aColored
                                    )
                                    : "......"
                                );
                            break;
                            default:
                                line.push_back( "?" );
                            break;
                        }
                    }

                    f
                    << toString( c, false, length( cc ))
                    << " / "
                    << implode( line, "|" )
                    << endl;
                }
                f.close();
            }
            flock( fd, LOCK_UN );
        }
        close(fd);
    }
    return this;
}
