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
        case ACTION_UNKNOWN : return "ACTION_UNKNOWN";
        case READ_VALUES    : return "READ_VALUES";
        case WRITE_VALUES   : return "WRITE_VALUES";
        case READ_ERRORS    : return "READ_ERRORS";
        case WRITE_ERRORS   : return "WRITE_ERRORS";
        case SYNC_RESET     : return "SYNC_RESET";
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
        case CMD_UNKNWON        : return "UNKNWON";
        case CMD_READ_NET       : return "READ_NET";
        case CMD_CLONE_NET      : return "CLONE_NET";
        case CMD_SWITCH_NET     : return "SWITCH_NET";
        case CMD_WRITE_LAYERS   : return "WRITE_LAYERS";
        case CMD_READ_LAYERS    : return "READ_LAYERS";
        case CMD_REQUEST_WEIGHTS: return "REQUEST_WEIGHTS";
        case CMD_WRITE_WEIGHTS  : return "WRITE_WEIGHTS";
        case CMD_READ_WEIGHTS   : return "READ_WEIGHTS";
        case CMD_DROP_LAYER_TICK: return "CMD_DROP_LAYER_TICK";
        case CMD_READ_LAYER_STAT: return "CMD_READ_LAYER_STAT";
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




BindType bindTypeFromString
(
    std::string a
)
{
    if( a == "VALUE" )              return BT_VALUE;
    if( a == "SAMPLE" )             return BT_SAMPLE;
    if( a == "COMMAND" )            return BT_COMMAND;
    if( a == "ERROR_TO_VALUE" )     return BT_ERROR_TO_VALUE;
    return BT_VALUE;
}



std::string bindTypeToString
(
    BindType a
)
{
    switch( a )
    {
        default:
        case BT_VALUE           : return "VALUE";
        case BT_SAMPLE          : return "SAMPLE";
        case BT_COMMAND         : return "COMMAND";
        case BT_ERROR_TO_VALUE  : return"ERROR_TO_VALUE";
    };
}
