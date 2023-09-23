#include "string"
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
        case READ_WEIGHTS   : return "READ_WEIGHTS";
        case WRITE_WEIGHTS  : return "WRITE_WEIGHTS";
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
    if( a == "READ_WEIGHTS" )  return READ_WEIGHTS;
    if( a == "WRITE_WEIGHTS" ) return WRITE_WEIGHTS;
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
        case THINKING_BEGIN : return "THINKING_BEGIN";
        case THINKING_END   : return "THINKING_END";
        case LEARNING_END   : return "LEARNING_END";
        case READ_NET       : return "READ_NET";
        case TICHING_END    : return "TICHING_END";
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
        case TASK_SERVER    : return "TASK_SERVER";
    }
    return "TASK_UNKNOWN";
}
