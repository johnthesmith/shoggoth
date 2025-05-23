#include <fstream>

#include "limb.h"
#include "../../../../lib/core/math.h"



/*
    Constructor
*/
Limb::Limb
(
    LogManager* aLogManager
)
{
    /* Set properties */
    logManager = aLogManager;

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
    LogManager* aLogManager       /* Log object*/
)
{
    return new Limb( aLogManager );
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
    Layer*      aLayerFrom,     /* Layer source */
    Layer*      aLayerTo,       /* Layer destination */
    NerveType   aNerveType,
    BindType    aBindType
)
{
    Nerve* result = Nerve::create
    (
        logManager,
        aLayerFrom,
        aLayerTo,
        aNerveType,
        aBindType
    );
    nerves -> push( result );
    lastChangeStructure = now();
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

Limb* Limb::copyTo
(
    Limb* aLimb,
    bool aStrictSync,
    bool aSkipLockThis,
    bool aSkipLockLimb
)
{
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
                    }
                    if( !nervesIsEqual )
                    {
                        /* Copy structure of nerves */
                        aLimb
                        -> getNerveList()
                        -> copyStructureFrom( this -> getNerveList(), aLimb -> getLayerList() );
                        nervesIsEqual = true;
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

    return this;
}



/**********************************************************************
    Setters and getters
*/



/*
    Loop for each parents neuron of this neuron
*/
//Limb* Limb::parentsLoop
//(
//    Layer*          aLayer,     /* Layer */
//    int             aIndex,     /* Neuron index */
//    BindType        aBindType,
//    Func            aCallback
////    parentsLambda   aCallback   /* Callback method */
//)
//{
//    /* Loop by nerves */
//    getNerveList() -> loop
//    (
//        [ &aLayer, &aCallback, &aIndex, &aBindType ]
//        ( void* aNerve )
//        {
//            auto iNerve = ( Nerve* ) aNerve;
//
//            if
//            (
//                iNerve -> getChild() == aLayer &&
//                (
//                    aBindType == BT_ALL ||
//                    iNerve -> getBindType() == aBindType
//                )
//            )
//            {
//                int from = 0;
//                int to = 0;
//                iNerve -> getWeightsRangeByChildIndex
//                (
//                    aIndex, from, to
//                );
//                /* Loop by weights */
//                for( int i = from; i < to;  i++ )
//                {
//                    aCallback
//                    (
//                        iNerve -> getParent(),
//                        iNerve -> getParentByWeightIndex( i ),
//                        iNerve,
//                        iNerve -> getWeight( i ),
//                        i
//                    );
//                }
//            }
//            return false;
//        }
//    );
//    return this;
//}



/*
    Loop for each child of i neuron for Layer
*/
//Limb* Limb::childrenLoop
//(
//    Layer*          aLayer,
//    int             aIndex,
//    BindType        aBindType,
//    Func            aCallback
//)
//{
//    /* Loop by nerves */
//    getNerveList() -> loop
//    (
//        [ &aLayer, &aCallback, &aIndex, &aBindType ]
//        ( void* aNerve )
//        {
//            auto iNerve = ( Nerve* ) aNerve;
//            if
//            (
//                iNerve -> getParent() == aLayer &&
//                (
//                    aBindType == BT_ALL ||
//                    iNerve -> getBindType() == aBindType
//                )
//            )
//            {
//                int from = 0;
//                int to = 0;
//                int step = 0;
//
//                iNerve -> getWeightsRangeByParentIndex
//                (
//                    aIndex, from, to, step
//                );
//
//                /* Loop by weights */
//                for( int i = from; i < to;  i += step )
//                {
//                    aCallback
//                    (
//                        iNerve -> getChild(),
//                        iNerve -> getChildByWeightIndex( i ),
//                        iNerve,
//                        iNerve -> getWeight( i ),
//                        i
//                    );
//                }
//            }
//            return false;
//        }
//    );
//    return this;
//}



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



/*
    Damp data definition function
    Uses in Limb::dump method
*/
Limb* Limb::dumpValue
(
    ofstream&   f,
    Data&       data,
    double      weight,
    Layer*      layerLink,
    int         layerLinkIndex
)
{
    double val = 0.0;

    switch( data )
    {
        default:
        case DATA_WEIGHTS:
            val = weight;
        break;
        case DATA_VALUES:
            val = layerLink -> getNeuronValue( layerLinkIndex );
        break;
        case DATA_ERRORS:
            val = layerLink -> getNeuronError( layerLinkIndex );
        break;
    }
    f << toString( val, 12, DF_FIXED, true );

    return this;
};



/*
    Dump weights errors values from neuron of the layer in to the file
*/
Limb* Limb::dump
(
    /* Store path */
    string          aPath,
    /* The layer */
    Layer*          aLayer,
    /* Neuron Index in the layer */
    Point3i         aNeuronPos,
    /* Type parent or child */
    Direction       aDirection,
    /* Data type */
    Data            aData,
    /* Data view*/
    Dataview        aDataview,
    /* Number of tick */
    long long int   aTick
)
{
    if( checkPath( getPath( aPath )))
    {
        /* Open file stream */
        ofstream f;
        f.open( aPath );
        auto neuronIndex = aLayer -> indexByPos( aNeuronPos );
        if( f.is_open() )
        {
            f << "tick: " << aTick << endl;

            switch( aDirection )
            {
                default:
                    f << "Unknown direction" << "\n";
                break;
                case DIRECTION_PARENT:
                {
                    Layer* lastParentLayer = NULL;
                    auto size = POINT_3I_0;

                    parentsLoop
                    (
                        aLayer,
                        neuronIndex,
                        BT_ALL,
                        [
                            this,
                            &f,
                            &aData,
                            &size,
                            &lastParentLayer,
                            &aLayer,
                            &aNeuronPos,
                            &aDirection
                        ]
                        (
                            Layer*  aParentLayer,
                            int     aParentNeuronIndex,
                            Nerve*  aNerve,
                            double  aWeight,
                            int     aWeightIndex
                        )
                        {
                            if( aParentLayer != lastParentLayer )
                            {
                                /* Start new layer table */
                                f
                                << aLayer -> getId()
                                << aNeuronPos.toString()
                                << " "
                                << directionToString( aDirection )
                                << " "
                                << dataToString( aData )
                                << " "
                                << aParentLayer -> getId()
                                << endl;

                                lastParentLayer = aParentLayer;
                                size = aParentLayer -> getSize();
                            }

                            auto p = Point3i::byIndex( size, aParentNeuronIndex );

                            /* Dump val */
                            dumpValue
                            (
                                f,
                                aData,
                                aWeight,
                                aParentLayer,
                                aParentNeuronIndex
                            );

                            /* Check new line or delimiter */
                            f << (( p.x == size.x - 1 ) ? "\n" : " | ");

                            return false;
                        }
                    );
                }
                break;
                case DIRECTION_CHILD:
                {
                    Layer* lastChildLayer = NULL;
                    auto size = POINT_3I_0;

                    childrenLoop
                    (
                        aLayer,
                        neuronIndex,
                        BT_ALL,
                        [ this, &f, &aData, &size, &lastChildLayer, &aLayer, &aNeuronPos, &aDirection ]
                        (
                            Layer*  aChildLayer,
                            int     aChildNeuronIndex,
                            Nerve*  aNerve,
                            double  aWeight,
                            int     aWeightIndex
                        )
                        {
                            if( aChildLayer != lastChildLayer )
                            {
                                /* Start new layer table */
                                f
                                << aLayer -> getId()
                                << aNeuronPos.toString()
                                << " "
                                << directionToString( aDirection )
                                << " "
                                << dataToString( aData )
                                << " "
                                << aChildLayer -> getId()
                                << endl;

                                lastChildLayer = aChildLayer;
                                size = aChildLayer -> getSize();
                            }

                            auto p = Point3i::byIndex( size, aChildNeuronIndex );

                            /* Dump val */
                            dumpValue
                            (
                                f,
                                aData,
                                aWeight,
                                aChildLayer,
                                aChildNeuronIndex
                            );

                            /* Check new line or delimiter */
                            f << (( p.x == size.x - 1 ) ? "\n" : " | ");

                            return false;
                        }
                    );
                }
                break;
            }
            f.close();
        }
    }

    return this;
}



/*
    Dump errors or values for the layer in to the file
*/
Limb* Limb::dump
(
    /* Store path */
    string          aPath,
    /* Calculataion stage */
    CalcStage       aStage,
    /* The layer */
    Layer*          aLayer,
    /* Data type */
    Data            aData,
    /* Data view*/
    Dataview        aDataview,
    /* */
    long long int   aTick,
    /* Colored */
    bool            aColored
)
{
    if( checkPath( getPath( aPath )))
    {
        /* Open file stream */
        ofstream f;
        f.open( aPath );
        if( f.is_open() )
        {
            f
            << aLayer -> getId()
            << " ("
            << dataToString( aData )
            << ") "
            << neuronFuncToStr( aLayer -> getFrontFunc())
            << "/"
            << neuronFuncToStr( aLayer -> getBackFunc())
            << "/"
            << neuronFuncToStr( aLayer -> getBackFuncOut())
            << endl
            << calcStageToString( aStage )
            << " #"
            << aTick
            << endl
            ;

            auto c = aLayer -> getCount();
            auto size = aLayer -> getSize();

            for( int i = 0; i < c; i++ )
            {
                double val = 0.0;

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
                        f << toString( val, 12, DF_FIXED, aColored );
                        delimiter = " | ";
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
                auto p = Point3i::byIndex( size, i );
                f << (( p.x == size.x - 1 ) ? "\n" : delimiter );
            }
            f.close();
        }
    }

    return this;
}



/*
    On cahnge event for limb
*/
Limb* Limb::onChangeValues()
{
    lastChangeValues = now();
    return this;
}
