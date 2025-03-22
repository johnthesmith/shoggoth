#include <string>
#include <unordered_map>

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
        case READ_STAT_ERRORS_BEFORE_CHANGE    : return "READ_STAT_ERRORS_BEFORE_CHANGE";
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
        case CMD_COMMIT_NET     : return "COMMIT_NET";
        case CMD_READ_NET       : return "READ_NET";
        case CMD_READ_NET_INFO  : return "READ_NET_INFO";
        case CMD_CLONE_NET      : return "CLONE_NET";
        case CMD_SWITCH_NET     : return "SWITCH_NET";
        case CMD_WRITE_LAYERS   : return "WRITE_LAYERS";
        case CMD_READ_LAYERS    : return "READ_LAYERS";
        case CMD_REQUEST_WEIGHTS: return "REQUEST_WEIGHTS";
        case CMD_WRITE_WEIGHTS  : return "WRITE_WEIGHTS";
        case CMD_READ_WEIGHTS   : return "READ_WEIGHTS";
        case CMD_DROP_LAYER_TICK: return "DROP_LAYER_TICK";
        case CMD_READ_LAYER_STAT: return "READ_LAYER_STAT";
        case CMD_GET_NET_MODE   : return "GET_NET_MODE";
        case CMD_SET_NET_MODE   : return "SET_NET_MODE";
    }
}



/*
    Convert calculation stage from string
*/
Command commandFromString
(
    std::string a
)
{
    if( a == "READ_NET" )            return CMD_READ_NET;
    if( a == "READ_NET_INFO" )       return CMD_READ_NET_INFO;
    if( a == "COMMIT_NET" )          return CMD_COMMIT_NET;
    if( a == "CLONE_NET" )           return CMD_CLONE_NET;
    if( a == "SWITCH_NET" )          return CMD_SWITCH_NET;
    if( a == "WRITE_LAYERS" )        return CMD_WRITE_LAYERS;
    if( a == "READ_LAYERS" )         return CMD_READ_LAYERS;
    if( a == "REQUEST_WEIGHTS" )     return CMD_REQUEST_WEIGHTS;
    if( a == "WRITE_WEIGHTS" )       return CMD_WRITE_WEIGHTS;
    if( a == "READ_WEIGHTS" )        return CMD_READ_WEIGHTS;
    if( a == "DROP_LAYER_TICK" )     return CMD_DROP_LAYER_TICK;
    if( a == "READ_LAYER_STAT" )     return CMD_READ_LAYER_STAT;
    if( a == "GET_NET_MODE" )        return CMD_GET_NET_MODE;
    if( a == "SET_NET_MODE" )        return CMD_SET_NET_MODE;

    return CMD_UNKNOWN;
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
    const string aValue,
    Dataview aDefault
)
{
    static const unordered_map<string, Dataview> lookup =
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
    string aValue,
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



/*
    Convert Dataview to string
*/
string netModeToString
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
