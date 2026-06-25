#include <fstream>
#include <unistd.h> // для close()
#include <sys/file.h>

#include "limb.h"
#include "../../../../lib/core/math.h"
#include "../../../../lib/core/str.h"
#include "../../../../lib/graph/param_point.h"


/*
    Constructor
*/
Limb::Limb
(
    LogManager* aLogManager,
    std::string aPayloadId,
    string aVersion
)
:
    logManager ( aLogManager ),
    payloadId ( aPayloadId ),
    version( aVersion )
{
    rnd = Rnd::create();
    /* Create layers and nerves structures */
    layers = LayerList::create( this );
    nerves = NerveList::create( aLogManager );
}



/*
    Destructor
*/
Limb::~Limb()
{
    getLog() -> begin( "Limb destroing" ) -> prm( "payload id", getPayloadId() );
    nerves -> clear() -> destroy();
    layers -> clear() -> destroy();
    /* Destroy rnd object */
    rnd -> destroy();
    getLog() -> end( "Limb destroed" ) -> prm( "payload id", getPayloadId() );
}



/*
    Apply config
*/
Limb* Limb::applyConfig
(
    /* Application config  */
    ParamList*  aAppConfig,
    /* Net config */
    ParamList*  aNetConfig
)
{
    lock();

    auto configLayers = aNetConfig -> getObject( Path{ "layers" });

    if( configLayers != NULL )
    {
        /* Separate using layers */
        collectLayersUsing( aAppConfig );

        /* Remove layers absents in the use list */
        purgeLayers( configLayers );

        /* Set net version from config */
        setVersion( aNetConfig -> getString( Path{ "version", "current" } ));

        /* Set rnd seed version from config */
        getRnd() -> setSeed( aNetConfig -> getUInt( Path{ "seed" } ));

        /* Load limb elements */
        loadLayers( configLayers );
        loadNerves( aNetConfig );
    }

    /* Update last update net moment */
    setLastUpdate( aNetConfig -> getInt( Path{ "lastUpdate" }, 0 ));

    /* Drop tick */
    setLearningTick( 0 );

    getLayerList() -> dump();
    getNerveList() -> dump();

    /* Clear tick stat for each layer */
    getLayerList() -> loop
    (
        []
        ( void* aLayer )
        {
            auto layer = ( Layer* ) aLayer;
            layer -> getChartTick() -> clear();
            return false;
        }
    );

    return this;
}



/*
    Load nerves from config
*/
Limb* Limb::loadLayers
(
    ParamList* aConfigLayers
)
{
    /* Create layers */
    getLog() -> begin( "Layers load for task" );
    aConfigLayers -> objectsLoop
    (
        [ this ]
        (
            ParamList* iParam,
            string layerId
        )
        {
            getLog() -> begin( "Layer loading" ) -> prm( "id", layerId );

            /* Layer creates */
            if( allLayers.find( layerId ) != allLayers.end() )
            {
                /* Add new layer in to limb */
                auto layer = addLayer( layerId );
                /* Load arguments */
                loadLayer( layer, iParam );
            }
            else
            {
                getLog()
                -> trace( "Layer skiped" )
                -> prm( "id", layerId )
                ;
            }

            getLog() -> end();
            return false;
        }
    );
    /* End of layers load */
    getLog() -> end( "" );
    return this;
}



/*
    Load layer structure from param list
    Layer may be resized.
*/
Limb* Limb::loadLayer
(
    /* Layer object */
    Layer*      aLayer,
    /* Layer configuration */
    ParamList*  aParams
)
{
    if( this -> isOk() )
    {
        /* Set ID from params */
        if
        (
            aLayer-> getId()
            != aParams -> getString( Path{ "id" }, aLayer -> getId() )
        )
        {
            setCode( "InvalidLayerID" );
        }
        else
        {
            /* Apply neuron functions for layer */
            aLayer
            -> setFrontFunc
            (
                strToFunc
                (
                    aParams -> getString( Path{ "functionFront" }, "NULL" )
                )
            )
            -> setBackFunc
            (
                strToFunc
                (
                    aParams -> getString( Path{ "functionBack" }, "NULL" )
                )
            )
            -> setBackFuncOut
            (
                strToFunc( aParams -> getString( Path{ "functionBackOut" }, "NULL" ))
            )
            -> setErrorCalc
            (
                errorCalcFromString( aParams -> getString( Path{ "errorCalc" }, "NONE" ))
            )
            -> setWeightCalc
            (
                weightCalcFromString( aParams -> getString( Path{ "weightCalc" }, "NONE" ))
            );

            /* Set Size from params */
            auto size = ParamPoint::point3i( aParams -> getObject( Path{ "size" } ));

            /* Remove nerves for size changed layer */
            if( size.mulComponents() != aLayer -> getCount() )
            {
                getNerveList() -> removeByLayer( aLayer );
            }

            /* Update layer */
            aLayer -> setSize( size );

            /* Apply default values */
            auto values = aParams -> getObject( Path{ "values" } );
            if( values != nullptr )
            {
                aLayer -> fillValue( values );
            }

            calcLayerValuesHash( aLayer );
        }
    }
    return this;
}



/*
    Load nerves from config
*/
Limb* Limb::loadNerves
(
    ParamList* aConfig
)
{
    /* Nerves */
    auto jsonNerves = aConfig -> getObject( Path{ "nerves" });
    if( jsonNerves != NULL )
    {
        auto layers = getLayerList();
        auto nerves = getNerveList();

        getLog() -> begin( "Nerves load" );

        jsonNerves -> loop
        (
            [ this, &layers, &nerves, &aConfig ]
            ( Param* aItem )
            {
                /* Check the json layer */
                if( aItem -> isObject() )
                {
                    auto jsonNerve = aItem -> getObject();

                    auto fromList = jsonNerve -> getStringVector( Path{ "idFrom" });
                    auto toList = jsonNerve -> getStringVector( Path{ "idTo" });

                    auto bindType = bindTypeFromString
                    (
                        jsonNerve -> getString( Path{ "bindType" } )
                    );
                    auto nerveType = nerveTypeFromString
                    (
                        jsonNerve -> getString( Path{ "nerveType" } )
                    );
                    auto nerveDelete = jsonNerve -> getBool( Path{ "delete" });
                    auto windowSize = ParamPoint::point3i
                    (
                        jsonNerve -> getObject( Path{ "windowSize" } )
                    );

                    /* Cartesian product for form and to */
                    for( auto& idFrom:fromList )
                    {
                        for( auto& idTo:toList )
                        {
                            /* Find the layers */
                            auto from = layers -> getById( idFrom );
                            auto to = layers -> getById( idTo );

                            if( from != NULL && to != NULL )
                            {
                                auto nerve = nerves -> find
                                (
                                    idFrom,
                                    idTo,
                                    bindType
                                );

                                if( nerve != NULL )
                                {
                                    if
                                    (
                                        nerve -> getParent() != from ||
                                        nerve -> getChild() != to ||
                                        nerve -> getBindType() != bindType ||
                                        nerve -> getNerveType() != nerveType ||
                                        nerveDelete
                                    )
                                    {
                                        deleteNerve( nerve );
                                        nerve = NULL;
                                    }
                                    else
                                    {
                                        getLog()
                                        -> trace( "Nerve exists" )
                                        -> prm( "idFrom", idFrom )
                                        -> prm( "idTo", idTo )
                                        -> lineEnd()
                                        ;
                                    }
                                }

                                if( nerve == NULL && !nerveDelete )
                                {
                                    auto minW = jsonNerve
                                    -> getDouble( Path{ "minWeight" } , 0 );
                                    auto maxW = jsonNerve
                                    -> getDouble( Path{ "maxWeight" }, 0 );
                                    auto mulW = aConfig
                                    -> getDouble( Path{ "weightMul" }, 1 );
                                    nerve = createNerve
                                    (
                                        from,
                                        to,
                                        nerveType,
                                        bindType,
                                        windowSize
                                    )
                                    -> setMinWeight
                                    (
                                        minW * ( minW == maxW ? 1 : mulW )
                                    )
                                    -> setMaxWeight
                                    (
                                        maxW * ( minW == maxW ? 1 : mulW )
                                    )
                                    ;
                                    if( !nerve -> isOk() )
                                    {
                                        getLog()
                                        -> warning( "Nerve error" )
                                        -> prm( "code", nerve -> getCode() )
                                        -> lineEnd();
                                    }
                                }
                            }
                            else
                            {
                                getLog()
                                -> info( "Nerve skiped" )
                                -> prm( "idFrom", idFrom )
                                -> prm( "idTo", idTo )
                                -> lineEnd()
                                ;
                            }
                        }
                    }
                }
                return false;
            }
        );

        getLog()
        -> end();

    } /* End of nerves load */
    return this;
}






/*
    Create new layer
*/
Layer* Limb::addLayer
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
        result = createLayer( aId );
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
        getLogManager(),
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
    Limb* aLimb
)
{
    bool result = false;

    if( aLimb != this )
    {
        if( lock())
        {
            if( aLimb -> lock())
            {
                result =
                layers -> compare( aLimb -> getLayerList() )
                &&
                nerves -> compare( aLimb -> getNerveList() );

                if( result )
                {
                    aLimb
                    -> getLayerList()
                    -> copyValuesFrom( this -> getLayerList() )
                    -> copyErrorsFrom( this -> getLayerList() )
                    ;
                }

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
    Current age of the limbs config
*/



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
    Data            aData
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
    Data            aData
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
                    Nerve*,
                    real    aWeight,
                    int
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
                    Nerve*,
                    real    aWeight,
                    int
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
                                    toString( (long long int)index, aColored, 2 )
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
                    << toString( (long long int)c, false, length( cc ))
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




/*
    Load selected weights to this limb from the limb argument
*/
Limb* Limb::weightsFrom
(
    /* Sorce limb */
    Limb* aFrom
)
{
    lock();
    aFrom -> lock();

    auto fromNerveList = aFrom -> getNerveList();
    auto c = fromNerveList -> getCount();

    if( c == getNerveList() -> getCount() )
    {
        auto ok = true;

        for( int i = 0; i < c; i ++ )
        {
            auto nerveFrom = (Nerve*) fromNerveList -> getByIndex( i );
            auto nerveTo = (Nerve*) getNerveList() -> getByIndex( i );
            ok = ok && nerveFrom -> getId() == nerveTo -> getId();
        }

        if( ok )
        {
            for( int i = 0; i < c; i ++ )
            {
                auto nerveFrom = (Nerve*) fromNerveList -> getByIndex( i );
                auto nerveTo = (Nerve*) getNerveList() -> getByIndex( i );
                nerveTo -> copyWeightsFrom( nerveFrom );
            }
        }
        else
        {
            /* Nerves have differnese */
        }
    }
    else
    {
        /* Nerves count for limbs not equal */
    }

    aFrom -> unlock();
    unlock();

    return this;
}



/*
    Remove layers absent in the list
*/
Limb* Limb::purgeLayers
(
    ParamList* aLayers  /* List from config */
)
{
    lock();
    /* Build pure list */
    vector <string> purgeList = {};
    getLayerList() -> loop
    (
        [ &purgeList, &aLayers ]
        ( void* iLayer )
        {
            auto layerId = (( Layer* ) iLayer ) -> getId();
            if( aLayers -> getObject( Path{ layerId }) == NULL )
            {
                /* Layer is absent in the config and must be delete */
                purgeList.push_back( layerId );
            }
            return false;
        }
    );

    /* Delete layers */
    auto c = purgeList.size();
    for( long unsigned int i = 0; i<c; i++ )
    {
        deleteLayer( purgeList[ i ] );
    }

    unlock();
    return this;
}



Limb* Limb::collectLayersUsing
(
    ParamList* aConfig
)
{
    /* Lock */
    allLayers.clear();
    readValues.clear();
    writeValues.clear();
    readErrors.clear();
    writeErrors.clear();

    auto payloads = aConfig -> getObject
    (
        Path
        {
            "engine",
            "payloads"
        }
    );

    if( payloads != nullptr )
    {
        payloads -> loop
        (
            [ this ]
            ( Param* item )
            {
                if
                (
                    item -> isObject() &&
                    (
                        item -> getName() == payloadId
                        ||
                        payloadId == ""
                    )
                )
                {
                    auto layers = item -> getObject() -> getObject
                    (
                        Path{ "config", "layers" }
                    );
                    if( layers != nullptr )
                    {
                        layers -> loop
                        (
                            [ this ]
                            ( Param* list )
                            {
                                auto layersId = list -> getObject();
                                if( layersId != nullptr )
                                {
                                    auto key = list -> getName();
                                    layersId -> loop
                                    (
                                        [ &key, this ]
                                        ( Param* item )
                                        {
                                            auto val = item -> getString();
                                            /* Add to all layers list */
                                            allLayers.insert( val );
                                            /* Add for operatibale lists */
                                            if( key== "read-values" )
                                                readValues.insert( val );
                                            else if( key== "write-values" )
                                                readValues.insert( val );
                                            else if( key== "read-errors" )
                                                readErrors.insert( val );
                                            else if( key== "write-errors" )
                                                writeErrors.insert( val );
                                            return false;
                                        }
                                    );
                                }
                                return false;
                            }
                        );
                    }
                }
                return false;
            }
        );
    }
    return this;
}




/**********************************************************************
    Setters and getters
*/




/*
    Dump layers
*/
Limb* Limb::dump()
{
    return this;
}

