#include <string>
#include <unordered_map>

#include "shoggoth_consts.h"



/*
    Convert calculation stage to string
*/
std::string calcStageToString
(
    CalcStage a
)
{
    switch( a )
    {
        default:
        case CALC_STAGE_UNKNOWN         : return "UNKNOWN";
        case CALC_STAGE_ALL             : return "ALL";
        case CALC_STAGE_START           : return "START";
        case CALC_STAGE_AFTER_FRONT     : return "AFTER_FRONT";
        case CALC_STAGE_AFTER_BACK      : return "AFTER_BACK";
        case CALC_STAGE_AFTER_LEARNING  : return "AFTER_LEARNING";
    }
}



/*
    Convert calculation stage from string
*/
CalcStage calcStageToString
(
    std::string a
)
{
    if( a == "ALL" )            return CALC_STAGE_ALL;
    if( a == "START" )          return CALC_STAGE_START;
    if( a == "AFTER_FRONT" )    return CALC_STAGE_AFTER_FRONT;
    if( a == "AFTER_BACK" )     return CALC_STAGE_AFTER_BACK;
    if( a == "AFTER_LEARNING" ) return CALC_STAGE_AFTER_LEARNING;
    return CALC_STAGE_UNKNOWN;
}



NerveType nerveTypeFromString
(
    std::string a
)
{
    if( a == "ONE_TO_ONE" ) return ONE_TO_ONE;
    if( a == "ALL_TO_ALL" ) return ALL_TO_ALL;
    if( a == "SOME_TO_SOME" ) return SOME_TO_SOME;
    return ALL_TO_ALL;
};



std::string nerveTypeToString
(
    NerveType a
)
{
    switch( a )
    {
        default:
        case ALL_TO_ALL : return "ALL_TO_ALL";
        case ONE_TO_ONE : return "ONE_TO_ONE";
        case SOME_TO_SOME : return "SOME_TO_SOME";
    }
}



/*
    Converts string to error calculation
*/
ErrorCalc errorCalcFromString
(
    std::string a
)
{
    if( a == "LEARNING" ) return EC_LEARNING;
    if( a == "VALUE" ) return EC_VALUE;
    if( a == "NONE" ) return EC_NONE;
    return EC_NONE;
}



/*
    Converts error calculation to string
*/
std::string errorCalcToString
(
    ErrorCalc a
)
{
    switch( a )
    {
        default:
        case EC_NONE            : return "NONE";
        case EC_LEARNING        : return "LEARNING";
        case EC_VALUE           : return "VALUE";
    };
}



/*
    Converts string to weight calculation
*/
WeightCalc weightCalcFromString
(
    std::string a
)
{
    if( a == "CALC" ) return WC_CALC;
    if( a == "NONE" ) return WC_NONE;
    return WC_NONE;
}



/*
    Converts error calculation to string
*/
std::string weightCalcToString
(
    WeightCalc a
)
{
    switch( a )
    {
        default:
        case WC_NONE    : return "NONE";
        case WC_CALC    : return "CALC";
    };
}



/*
    Convert datatype from string
*/
Data dataFromString
(
    std::string a
)
{
    if( a == "WEIGHTS" ) return DATA_WEIGHTS;
    if( a == "VALUES" ) return DATA_VALUES;
    if( a == "ERRORS" ) return DATA_ERRORS;
    if( a == "INDEX_WEIGHTS" ) return DATA_INDEX_WEIGHTS;
    return DATA_UNKNOWN;
}



/*
    Convert datatype to string
*/
std::string dataToString
(
    Data a
)
{
    switch( a )
    {
        default:
        case DATA_UNKNOWN       : return "UNKNOWN";
        case DATA_VALUES        : return "VALUES";
        case DATA_ERRORS        : return "ERRORS";
        case DATA_WEIGHTS       : return "WEIGHTS";
        case DATA_INDEX_WEIGHTS : return "INDEX_WEIGHTS";
    };
}



/*
    Convert direction from string
*/
Direction directionFromString
(
    std::string a
)
{
    if( a == "NONE" )   return DIRECTION_NONE;
    if( a == "PARENT" ) return DIRECTION_PARENT;
    if( a == "CHILD" )  return DIRECTION_CHILD;
    return DIRECTION_UNKNOWN;
}



/*
    Convert direction to string
*/
std::string directionToString
(
    Direction a
)
{
    switch( a )
    {
        default:
        case DIRECTION_UNKNOWN  : return "UNKNOWN";
        case DIRECTION_NONE     : return "NONE";
        case DIRECTION_PARENT   : return "PARENT";
        case DIRECTION_CHILD    : return "CHILD";
    };
}



/*
    Convert Dataview from string
*/
Dataview dataviewFromString
(
    const std::string aValue,
    Dataview aDefault
)
{
    static const std::unordered_map<std::string, Dataview> lookup =
    {
        { "UNKNOWN", DATAVIEW_UNKNOWN },
        { "GRAPH",   DATAVIEW_GRAPH },
        { "CHAR",    DATAVIEW_CHAR },
        { "FLOAT",   DATAVIEW_FLOAT }
    };

    auto it = lookup.find( aValue );
    return (it != lookup.end()) ? it -> second : aDefault;
}




/*
    Convert Dataview to string
*/
std::string dataviewToString
(
    Dataview a
)
{
    switch( a )
    {
        default:
        case DATAVIEW_UNKNOWN   : return "UNKNOWN";
        case DATAVIEW_GRAPH     : return "GRAPH";
        case DATAVIEW_CHAR      : return "CHAR";
        case DATAVIEW_FLOAT     : return "FLOAT";
    };
}



/*
    Convert Dataview from string
*/
NetMode netModeFromString
(
    /* String argument for conversion */
    std::string aValue,
    /* Defaule value */
    NetMode aDefault
)
{
    if( aValue == "MODE_UNKNOWN" )  return NET_MODE_UNKNOWN;
    if( aValue == "MODE_LEARN" )    return NET_MODE_LEARN;
    if( aValue == "MODE_TEST" )     return NET_MODE_TEST;
    if( aValue == "MODE_WORK" )     return NET_MODE_WORK;

    return aDefault;
}


