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
        case READ_VALUES    : return "READ_VALUES";
        case WRITE_VALUES   : return "WRITE_VALUES";
        case READ_ERRORS    : return "READ_ERRORS";
        case WRITE_ERRORS   : return "WRITE_ERRORS";
        case SYNC_RESET     : return "SYNC_RESET";
    }
    return "ACTION_UNKNOWN";
}



/*
    Return Action const by string value
*/
Action stringToAction
(
    std::string a
)
{
    if( a == "READ_VALUES" )   return READ_VALUES;
    if( a == "WRITE_VALUES" )  return WRITE_VALUES;
    if( a == "READ_ERRORS" )   return READ_ERRORS;
    if( a == "WRITE_ERRORS" )  return WRITE_ERRORS;
    if( a == "SYNC_RESET" )    return SYNC_RESET;
    return ACTION_UNKNOWN;
}




/*
    Return event string by Event
*/
std::string eventToString
(
    Event a /* Event enum */
)
{
    switch( a )
    {
        case LOOP_BEGIN     : return "LOOP_BEGIN";
        case LOOP_END       : return "LOOP_END";
        case READ_NET       : return "READ_NET";
        case TEACHER_BEGIN  : return "TEACHER_BEGIN";
        case TEACHER_END    : return "TEACHER_END";
    }
    return "EVENT_UNKNOWN";
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
        case TASK_UI        : return "TASK_UI";
        case TASK_PROC      : return "TASK_PROC";
        case TASK_TEACHER   : return "TASK_TEACHER";
    }
    return "TASK_UNKNOWN";
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
        case CMD_READ_NET       : return "READ_NET";
        case CMD_WRITE_LAYERS   : return "WRITE_LAYERS";
        case CMD_READ_LAYERS    : return "READ_LAYERS";
        case CMD_WRITE_ERRORS   : return "WRITE_ERRORS";
        case CMD_READ_ERRORS    : return "READ_ERRORS";
        case CMD_WRITE_WEIGHTS  : return "WRITE_WEIGHTS";
        case CMD_READ_WEIGHTS   : return "READ_WEIGHTS";
    }
    return "UNKNOWN";
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
        case CALC_UNKNOWN   : return "CALC_UNKNOWN";
        case CALC_NOT_START : return "CALC_NOT_START";
        case CALC_START     : return "CALC_START";
        case CALC_COMPLETE  : return "CALC_COMPLETE";
    }
    return "CALC_UNKNOWN";
}
