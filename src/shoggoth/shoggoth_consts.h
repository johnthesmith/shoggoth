/*
    Shoggoth constants and conversion methods
*/

#pragma once


/*
    List of events for neral net
*/
enum Event
{
    EVENT_UNKNOWN,
    LOOP_BEGIN,         /* Event for each calculation loop */
    THINKING_BEGIN,     /* Event for begin of the value calculations */
    THINKING_END,       /* Event for end of the value calculation */
    LEARNING_END,       /**/
    READ_NET,           /**/
    TEACHING_END
};



/*
    List of actions like resolving
    actions = f( event, active modules )
*/
enum Action
{
    ACTION_UNKNOWN,
    READ_VALUES,
    WRITE_VALUES,
    READ_ERRORS,
    WRITE_ERRORS,
    READ_WEIGHTS,
    WRITE_WEIGHTS,
    SYNC_RESET
};



/*
    List of participants task
*/
enum Task
{
    TASK_UNKNOWN,
    TASK_UI,
    TASK_PROC,
    TASK_TEACHER,
    TASK_SERVER
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
*/
enum Command
{
    CMD_UNKNWON,        /* Unknown command */
    CMD_READ_NET,       /* Server return full Net configuration */
    CMD_WRITE_VALUES,   /* Server receive layer values */
    CMD_READ_VALUES,    /* Server return the layer values data */
    CMD_WRITE_ERRORS,   /* Server receive layer errors */
    CMD_READ_ERRORS,    /* Server return the layer errors data */
    CMD_WRITE_WEIGHTS,  /* Server receive the nerve weights data */
    CMD_READ_WEIGHTS    /* Server return the nerve weights data */
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
