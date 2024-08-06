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
    Convert command to string
*/
std::string calcStageToString
(
    CalcStage a
)
{
    switch( a )
    {
        default:
        case CALC_UNKNOWN   : return "CALC_UNKNOWN";
        case CALC_NOT_START : return "CALC_NOT_START";
        case CALC_START     : return "CALC_START";
        case CALC_COMPLETE  : return "CALC_COMPLETE";
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
        case BT_ADD             : return "ADD";
        case BT_MUL             : return "MUL";
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
    if( a == "CURRENT_VALUE" ) return EC_CURRENT_VALUE;
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
        case EC_CURRENT_VALUE   : return "CURRENT_VALUE";
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
