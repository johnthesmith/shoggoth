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
    READ_STAT_VALUE = 18,
    READ_STAT_ERROR = 19,
    READ_STAT_TICK  = 20
};



/*
    List of actions
*/
typedef vector <Action> Actions;




/*
    Type of neuron direction
    Uses for neuronet monitoring
*/
enum Direction
{
    /* Unknown direction */
    DIRECTION_UNKNOWN,
    /* Direction not specified */
    DIRECTION_NONE,
    /* Direction from the current layer to the parent layer */
    DIRECTION_PARENT,
    /* Direction from the current layer to the child layer */
    DIRECTION_CHILD
};



/*
    Type for array of data
    Uses for neuronet monitoring.
*/
enum Data
{
    /* Unknown data */
    DATA_UNKNOWN,
    /* Nerve weights */
    DATA_WEIGHTS,
    /* Neuron values */
    DATA_VALUES,
    /* Neuron errors */
    DATA_ERRORS
};



/*
    Data view type
*/
enum Dataview
{
    /* Unknown out type */
    DATAVIEW_UNKNOWN,
    /* Digits like 0.01321323 */
    DATAVIEW_DIGITS,
    /* Graph like:
        " " <= EPSILON;
        "░" <= 0.25;
        "▒" <= 0.5;
        "▓" <= 0.75;
        "█" <= 1.0
    */
    DATAVIEW_GRAPH
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
    TASK_EVOLUTION
};



/*
    Shoggoth layers calculation stage
    The calculation stages works in LimbProcessor::calc.
*/
enum CalcStage
{
    /* Unknown stage for configuration errors detecting */
    CALC_STAGE_UNKNOWN,
    /* Works for all stages */
    CALC_STAGE_ALL,
    /* On calculation start */
    CALC_STAGE_START,
    CALC_STAGE_AFTER_FRONT,
    CALC_STAGE_AFTER_BACK,
    CALC_STAGE_AFTER_LEARNING
};



/*
    Shoggoth protocol commands
*/
enum Command
{
    /* Unknown command */
    CMD_UNKNOWN,
    /* Server return full Net configuration */
    CMD_READ_NET,
    /* Server clone net */
    CMD_CLONE_NET,
    /* Server switch to specified net */
    CMD_SWITCH_NET,
    /* Server send layers with errors and values plan */
    CMD_WRITE_LAYERS,
    /* Server receive layers with errors and values plan */
    CMD_READ_LAYERS,
    /* Request weights for one neuron */
    CMD_REQUEST_WEIGHTS,
    /* Drop layer tick counter */
    CMD_DROP_LAYER_TICK,
    /* Server return the layer statistics */
    CMD_READ_LAYER_STAT,
    /* Server receive  */
    CMD_WRITE_WEIGHTS,
    /* Server return the layer  */
    CMD_READ_WEIGHTS
};



/*
    Neuron layer error calculation types
*/
enum ErrorCalc
{
    /* Error not changeing */
    EC_NONE,
    /* Calculating error for learning */
    EC_LEARNING,
    /* Put value in to the error */
    EC_VALUE
};



/*
    Neuron layer value calculation types
*/
enum ValueCalc
{
    /* Value not calculating */
    VC_NONE,
    /* Value calculating */
    VC_CALC
};



/*
    Weight layer error calculation types
*/
enum WeightCalc
{
    /* Weight not changeing */
    WC_NONE,
    /* Calculating weight for learning */
    WC_CALC
};



enum BindType
{
    /* All binds */
    BT_ALL,
    /* Additive bind*/
    BT_ADD,
    /* Multiplexor bind */
    BT_MUL
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
    Convert calculation stage to string
*/
std::string calcStageToString
(
    CalcStage
);



/*
    Convert calculation stage from string
*/
CalcStage calcStageFromString
(
    string
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



/*
    Converts string to error calculation
*/
ErrorCalc errorCalcFromString
(
    string
);


/*
    Converts error calculation to string
*/
string errorCalcToString
(
    ErrorCalc
);



/*
    Converts string to weight calculation
*/
WeightCalc weightCalcFromString
(
    string a
);



/*
    Converts error calculation to string
*/
string weightCalcToString
(
    WeightCalc
);



NerveType nerveTypeFromString
(
    string
);



string nerveTypeToString
(
    NerveType
);



/*
    Convert datatype from string
*/
Data dataFromString
(
    string
);



/*
    Convert datatype to string
*/
string dataToString
(
    Data
);



/*
    Convert direction from string
*/
Direction directionFromString
(
    string
);



/*
    Convert direction to string
*/
string directionToString
(
    Direction
);



/*
    Convert dataview from string
*/
Dataview dataviewFromString
(
    /* String argument for conversion */
    string,
    /* Defaule value */
    Dataview = DATAVIEW_UNKNOWN
);



/*
    Convert dataview to string
*/
string dataviewToString
(
    Dataview
);
