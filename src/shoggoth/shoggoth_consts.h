/*
    Shoggoth constants and conversion methods
*/

#pragma once


#include <vector>



using namespace std;



/*
    Type of nerves
*/
enum NerveType
{
    ALL_TO_ALL,
    ONE_TO_ONE
};



/*
    List of events for neral net
*/
enum Event
{
    EVENT_UNKNOWN,
    LOOP_BEGIN,         /* Event for each main loop begin */
    LOOP_END,           /* Event for each main loop end */
    READ_NET,           /**/
    TEACHER_BEGIN,      /* Begin of teacher loop */
    TEACHER_END         /* End o teeacher loop */
};



/*
    List of actions like resolving
    actions = f( event, active modules )
*/
enum Action
{
    ACTION_UNKNOWN  = 10,
    READ_VALUES     = 11,
    WRITE_VALUES    = 12,
    READ_ERRORS     = 13,
    WRITE_ERRORS    = 14,
    SYNC_RESET      = 17
};



/*
    List of actions
*/
typedef vector <Action> Actions;



/*
    List of participants task
*/
enum Task
{
    TASK_UNKNOWN,
    TASK_UI,
    TASK_PROC,
    TASK_TEACHER
};




/*
    Shoggoth RPC protocol constants
*/

enum CalcDirection
{
    CALC_ALL,
    CALC_FORWARD,     /* Calculation forward */
    CALC_BACKWARD     /* Calculation backweard */
};



/*
    Shoggoth layers calculation stage
*/
enum CalcStage
{
    CALC_UNKNOWN,
    CALC_NOT_START,
    CALC_START,
    CALC_COMPLETE
};



/*
    Shoggoth protocol commands
*/
enum Command
{
    CMD_UNKNWON,        /* Unknown command */
    CMD_READ_NET,       /* Server return full Net configuration */
    CMD_WRITE_LAYERS,   /* Server send layers with errors and values plan */
    CMD_READ_LAYERS,    /* Server receive layers with errors and values plan */
    CMD_WRITE_ERRORS,   /* Server receive layer errors */
    CMD_READ_ERRORS,    /* Server return the layer errors data */
    CMD_WRITE_WEIGHTS,  /* Server receive  */
    CMD_READ_WEIGHTS    /* Server return the layer  */
};



enum BindType
{
    BT_VALUE,           /* Spred value */
    BT_SAMPLE,          /**/
    BT_COMMAND,
    BT_ERROR_TO_VALUE
};



/*
    Convert command to string
*/
std::string actionToString
(
    Action
);



/*
    Return Action const by string value
*/
Action stringToAction
(
    std::string
);



/*
    Return event string by Event
*/
std::string eventToString
(
    Event /* Event enum */
);



/*
    Return task of participant string by task enum
*/
std::string taskToString
(
    Task aValue /* Event enum */
);



/*
    Convert command to string
*/
std::string commandToString
(
    Command
);



/*
    Convert calculation stage to to string
*/
std::string calcStageToString
(
    CalcStage
);
