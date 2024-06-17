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
    SYNC_RESET      = 17,
    READ_STAT       = 18
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
    TASK_TEACHER,
    TASK_EVOLUTION
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
    CMD_CLONE_NET,      /* Server clone net */
    CMD_SWITCH_NET,     /* Server switch to specified net */
    CMD_WRITE_LAYERS,   /* Server send layers with errors and values plan */
    CMD_READ_LAYERS,    /* Server receive layers with errors and values plan */
    CMD_REQUEST_WEIGHTS,/* Request weights for one neuron */

    CMD_DROP_LAYER_TICK,/* Drop layer tick counter */
    CMD_READ_LAYER_STAT, /* Server return the layer statistics */

    CMD_WRITE_WEIGHTS,  /* Server receive  */
    CMD_READ_WEIGHTS    /* Server return the layer  */
};



enum BindType
{
    BT_ALL,             /* All binds */
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



/*
    Convert string to neuron bind type
*/
BindType bindTypeFromString
(
    string
);



/*
    Convert neuron bind type to string
*/
string bindTypeToString
(
    BindType
);

