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
    TICHING_END
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
