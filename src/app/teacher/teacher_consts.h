/*
    Shoggoth Teacher specific constants
*/

#pragma once


/*
    List with type of teachers tasks
*/
enum TeacherTask
{
    TEACHER_CMD_UNKNOWN,
    TEACHER_CMD_VALUE_TO_LAYER,
    TEACHER_CMD_VALUES_TO_LAYER,
    TEACHER_CMD_IMAGE_TO_LAYER,
    TEACHER_CMD_FOLDER_TO_LAYER,
    TEACHER_CMD_GUID_TO_LAYER,
    TEACHER_CMD_HID_TO_LAYER
};



/*
    List of teaching strtegy
*/
enum Strategy
{
    TEACHER_STRATEGY_UNKNOWN,
    TEACHER_STRATEGY_LOOP,
    TEACHER_STRATEGY_RANDOM,
    TEACHER_STRATEGY_ORDER,
    TEACHER_STRATEGY_PER
};



/*
    Convert command to string
*/
inline std::string teacherTaskToString
(
    TeacherTask a
)
{
    switch( a )
    {
        case TEACHER_CMD_UNKNOWN            : return "TEACHER_CMD_UNKNOWN";
        case TEACHER_CMD_VALUE_TO_LAYER     : return "VALUE_TO_LAYER";
        case TEACHER_CMD_VALUES_TO_LAYER    : return "VALUES_TO_LAYER";
        case TEACHER_CMD_IMAGE_TO_LAYER     : return "IMAGE_TO_LAYER";
        case TEACHER_CMD_FOLDER_TO_LAYER    : return "FOLDER_TO_LAYER";
        case TEACHER_CMD_GUID_TO_LAYER      : return "GUID_TO_LAYER";
        case TEACHER_CMD_HID_TO_LAYER       : return "HID_TO_LAYER";
    }
    return "TEACHER_CMD_UNKNOWN";
}



/*
    Convert string to teacher task
*/
inline TeacherTask stringToTeacherTask
(
    std::string a
)
{
    if( a == "VALUE_TO_LAYER" )     return TEACHER_CMD_VALUE_TO_LAYER;
    if( a == "VALUES_TO_LAYER" )    return TEACHER_CMD_VALUES_TO_LAYER;
    if( a == "IMAGE_TO_LAYER" )     return TEACHER_CMD_IMAGE_TO_LAYER;
    if( a == "FOLDER_TO_LAYER" )    return TEACHER_CMD_FOLDER_TO_LAYER;
    if( a == "GUID_TO_LAYER" )      return TEACHER_CMD_GUID_TO_LAYER;
    if( a == "HID_TO_LAYER" )       return TEACHER_CMD_HID_TO_LAYER;
    return TEACHER_CMD_UNKNOWN;
}



/*
    Convert strategy to string
*/
inline std::string teacherStrategyToString
(
    Strategy a
)
{
    switch( a )
    {
        case TEACHER_STRATEGY_UNKNOWN : return "UNKNOWN";
        case TEACHER_STRATEGY_LOOP    : return "LOOP";
        case TEACHER_STRATEGY_RANDOM  : return "RANDOM";
        case TEACHER_STRATEGY_ORDER   : return "ORDER";
        case TEACHER_STRATEGY_PER     : return "PER";
    }
    return "UNKNOWN";
}



/*
    Convert string to strategy
*/
inline Strategy stringToTeacherStrategy
(
    std::string a
)
{
    if( a == "LOOP" )   return TEACHER_STRATEGY_LOOP;
    if( a == "RANDOM" ) return TEACHER_STRATEGY_RANDOM;
    if( a == "ORDER" )  return TEACHER_STRATEGY_ORDER;
    if( a == "PER" )    return TEACHER_STRATEGY_PER;
    return TEACHER_STRATEGY_UNKNOWN;
}
