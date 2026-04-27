/*
    Shoggoth constants and conversion methods
*/

#pragma once


#include <vector>



/*
    Type of nerves
*/
enum NerveType
{
    ALL_TO_ALL,
    ONE_TO_ONE,
    SOME_TO_SOME
};



/*
    Type of neuron direction
    Uses for neuronet monitoring
*/
enum Direction
{
    /* Unknown direction */
    DIRECTION_UNKNOWN,
    /* Direction not specified */
    DIRECTION_NONE,
    /* Direction from the current layer to the parent layer */
    DIRECTION_PARENT,
    /* Direction from the current layer to the child layer */
    DIRECTION_CHILD
};



/*
    Type for array of data
    Uses for neuronet monitoring.
*/
enum Data
{
    /* Unknown data */
    DATA_UNKNOWN,
    /* Nerve weights */
    DATA_WEIGHTS,
    /* Neuron values */
    DATA_VALUES,
    /* Neuron errors */
    DATA_ERRORS,
    /* Weights index */
    DATA_INDEX_WEIGHTS
};



/*
    Data view type
*/
enum Dataview
{
    /* Unknown out type */
    DATAVIEW_UNKNOWN,
    /* Digits like 0.01321323 */
    DATAVIEW_FLOAT,
    //
    DATAVIEW_CHAR,
    DATAVIEW_GRAPH
};



/*
    Dump types
*/
enum Dumptype
{
    /* Unknown out type */
    DUMP_TYPE_UNKNOWN,
    DUMP_TYPE_LAYER,
    DUMP_TYPE_NERVE,
    DUMP_TYPE_NEURON
};




/*
    Shoggoth layers calculation stage
    The calculation stages works in LimbProcessor::calc.
*/
enum CalcStage
{
    /* Unknown stage for configuration errors detecting */
    CALC_STAGE_UNKNOWN,
    /* Works for all stages */
    CALC_STAGE_ALL,
    /* On calculation start */
    CALC_STAGE_START,
    CALC_STAGE_AFTER_FRONT,
    CALC_STAGE_AFTER_BACK,
    CALC_STAGE_AFTER_LEARNING
};



/*
    Net mode
    using at command
        CMD_GET_NET_MODE
        CMD_SET_NET_MODE
*/
enum NetMode
{
    /* Unknown mode */
    NET_MODE_UNKNOWN,
    /* Learning */
    NET_MODE_LEARN,
    /* Test */
    NET_MODE_TEST,
    /* Work (calculate) */
    NET_MODE_WORK
};



/*
    Neuron layer error calculation types
*/
enum ErrorCalc
{
    /* Error not changeing */
    EC_NONE,
    /* Calculating error for learning */
    EC_LEARNING,
    /* Put value in to the error */
    EC_VALUE
};



/*
    Neuron layer value calculation types
*/
enum ValueCalc
{
    /* Value not calculating */
    VC_NONE,
    /* Value calculating */
    VC_CALC
};



/*
    Weight layer error calculation types
*/
enum WeightCalc
{
    /* Weight not changeing */
    WC_NONE,
    /* Calculating weight for learning */
    WC_CALC
};



enum BindType
{
    /* All binds */
    BT_ALL,
    /* Additive bind*/
    BT_ADD,
    /* Multiplexor bind */
    BT_MUL,
    /*
        Maximizator bind
        hi priority befor BT_ALL BT_MUL in the calculation
    */
    BT_MAX
};



/*
    Convert calculation stage to string
*/
std::string calcStageToString
(
    CalcStage
);



/*
    Convert calculation stage from string
*/
CalcStage calcStageFromString
(
    std::string
);




/*
    Converts string to error calculation
*/
ErrorCalc errorCalcFromString
(
    std::string
);


/*
    Converts error calculation to string
*/
std::string errorCalcToString
(
    ErrorCalc
);



/*
    Converts string to weight calculation
*/
WeightCalc weightCalcFromString
(
    std::string a
);



/*
    Converts error calculation to string
*/
std::string weightCalcToString
(
    WeightCalc
);



NerveType nerveTypeFromString
(
    std::string
);



std::string nerveTypeToString
(
    NerveType
);



/*
    Convert datatype from string
*/
Data dataFromString
(
    std::string
);



/*
    Convert datatype to string
*/
std::string dataToString
(
    Data
);



/*
    Convert direction from string
*/
Direction directionFromString
(
    std::string
);



/*
    Convert direction to string
*/
std::string directionToString
(
    Direction
);



/*
    Convert dataview from string
*/
Dataview dataviewFromString
(
    /* String argument for conversion */
    const std::string,
    /* Defaule value */
    Dataview = DATAVIEW_UNKNOWN
);



/*
    Convert dataview to string
*/
std::string dataviewToString
(
    Dataview
);



/*
    Convert Dataview from string
*/
NetMode netModeFromString
(
    /* String argument for conversion */
    std::string,
    /* Defaule value */
    NetMode = NET_MODE_UNKNOWN
);



/*
    Convert Dataview to string
*/
inline std::string netModeToString
(
    NetMode a
)
{
    switch( a )
    {
        default:
        case NET_MODE_UNKNOWN   : return "MODE_UNKNOWN";
        case NET_MODE_LEARN     : return "MODE_LEARN";
        case NET_MODE_TEST      : return "MODE_TEST";
        case NET_MODE_WORK      : return "MODE_WORK";
    };
}



/*
    Converts string to bind type
*/
inline BindType bindTypeFromString
(
    std::string a
)
{
    if( a == "MUL" ) return BT_MUL;
    if( a == "ADD" ) return BT_ADD;
    if( a == "MAX" ) return BT_MAX;
    return BT_ADD;
}



/*
    Converts bind type to string
*/
inline std::string bindTypeToString
(
    BindType a
)
{
    switch( a )
    {
        default:
        case BT_ADD : return "ADD";
        case BT_MUL : return "MUL";
        case BT_MAX : return "MAX";
    };
}



/*
    Convert type of dump from string
*/
inline Dumptype dumptypeFromString
(
    std::string a
)
{
    if( a == "NERVE" ) return DUMP_TYPE_NERVE;
    if( a == "LAYER" ) return DUMP_TYPE_LAYER;
    if( a == "NEURON" ) return DUMP_TYPE_NEURON;
    return DUMP_TYPE_UNKNOWN;
}



/*
    Convert type of dump from string
*/
inline std::string dumptypeToString
(
    Dumptype a
)
{
    switch( a )
    {
        default:
        case DUMP_TYPE_NERVE   : return "NERVE";
        case DUMP_TYPE_LAYER   : return "LAYER";
        case DUMP_TYPE_NEURON  : return "NEURON";
        case DUMP_TYPE_UNKNOWN : return "UNKNOWN";
    };
}

