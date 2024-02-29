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
        case TASK_UNKNOWN   : return "TASK_UNKNOWN";
        case TASK_UI        : return "TASK_UI";
        case TASK_PROC      : return "TASK_PROC";
        case TASK_TEACHER   : return "TASK_TEACHER";
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
        case CMD_WRITE_LAYERS   : return "WRITE_LAYERS";
        case CMD_READ_LAYERS    : return "READ_LAYERS";
        case CMD_WRITE_WEIGHTS  : return "WRITE_WEIGHTS";
        case CMD_READ_WEIGHTS   : return "READ_WEIGHTS";
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
