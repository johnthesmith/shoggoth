#include <string>
#include "shoggoth_consts.h"



/*
    Convert command to string
*/
std::string actionToString
(
    Action a
)
{
    switch( a )
    {
        default:
        case ACTION_UNKNOWN     : return "ACTION_UNKNOWN";
        case READ_VALUES        : return "READ_VALUES";
        case WRITE_VALUES       : return "WRITE_VALUES";
        case READ_ERRORS        : return "READ_ERRORS";
        case WRITE_ERRORS       : return "WRITE_ERRORS";
        case SYNC_RESET         : return "SYNC_RESET";
        case READ_STAT_ERROR    : return "READ_STAT_ERROR";
        case READ_STAT_VALUE    : return "READ_STAT_VALUE";
        case READ_STAT_TICK     : return "READ_STAT_TICK";
    }
}



/*
    Return task of participant string by task enum
*/
std::string taskToString
(
    Task a /* Event enum */
)
{
    switch( a )
    {
        default:
        case TASK_UNKNOWN   : return "UNKNOWN";
        case TASK_UI        : return "UI";
        case TASK_PROC      : return "PROC";
        case TASK_TEACHER   : return "TEACHER";
        case TASK_EVOLUTION : return "EVOLUTION";
    }
}



/*
    Convert command to string
*/
std::string commandToString
(
    Command a
)
{
    switch( a )
    {
        default:
        case CMD_UNKNOWN        : return "UNKNOWN";
        case CMD_READ_NET       : return "READ_NET";
        case CMD_CLONE_NET      : return "CLONE_NET";
        case CMD_SWITCH_NET     : return "SWITCH_NET";
        case CMD_WRITE_LAYERS   : return "WRITE_LAYERS";
        case CMD_READ_LAYERS    : return "READ_LAYERS";
        case CMD_REQUEST_WEIGHTS: return "REQUEST_WEIGHTS";
        case CMD_WRITE_WEIGHTS  : return "WRITE_WEIGHTS";
        case CMD_READ_WEIGHTS   : return "READ_WEIGHTS";
        case CMD_DROP_LAYER_TICK: return "DROP_LAYER_TICK";
        case CMD_READ_LAYER_STAT: return "READ_LAYER_STAT";
    }
}



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
    string a
)
{
    if( a == "ONE_TO_ONE" ) return ONE_TO_ONE;
    if( a == "ALL_TO_ALL" ) return ALL_TO_ALL;
    return ALL_TO_ALL;
};



string nerveTypeToString
(
    NerveType a
)
{
    switch( a )
    {
        default:
        case ALL_TO_ALL : return "ALL_TO_ALL";
        case ONE_TO_ONE : return "ONE_TO_ONE";
    }
}




/*
    Converts string to bind type
*/
BindType bindTypeFromString
(
    std::string a
)
{
    if( a == "MUL" ) return BT_MUL;
    if( a == "ADD" ) return BT_ADD;
    return BT_ADD;
}



/*
    Converts bind type to string
*/
std::string bindTypeToString
(
    BindType a
)
{
    switch( a )
    {
        default:
        case BT_ADD : return "ADD";
        case BT_MUL : return "MUL";
    };
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
    string a
)
{
    if( a == "WEIGHTS" ) return DATA_WEIGHTS;
    if( a == "VALUES" ) return DATA_VALUES;
    if( a == "ERRORS" ) return DATA_ERRORS;
    return DATA_UNKNOWN;
}



/*
    Convert datatype to string
*/
string dataToString
(
    Data a
)
{
    switch( a )
    {
        default:
        case DATA_UNKNOWN   : return "UNKNOWN";
        case DATA_VALUES    : return "VALUES";
        case DATA_ERRORS    : return "ERRORS";
        case DATA_WEIGHTS   : return "WEIGHTS";
    };
}



/*
    Convert direction from string
*/
Direction directionFromString
(
    string a
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
string directionToString
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
    /* String argument for conversion */
    string aValue,
    /* Defaule value */
    Dataview aDefault
)
{
    if( aValue == "UNKNOWN" )   return DATAVIEW_UNKNOWN;
    if( aValue == "GRAPH" )     return DATAVIEW_GRAPH;
    if( aValue == "DIGITS" )    return DATAVIEW_DIGITS;
    return aDefault;
}



/*
    Convert Dataview to string
*/
string dataviewToString
(
    Dataview a
)
{
    switch( a )
    {
        default:
        case DATAVIEW_UNKNOWN   : return "UNKNOWN";
        case DATAVIEW_GRAPH     : return "GRAPH";
        case DATAVIEW_DIGITS    : return "DIGITS";
    };
}
