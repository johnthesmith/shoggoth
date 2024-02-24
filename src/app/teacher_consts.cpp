#include <string>
#include "teacher_consts.h"



/*
    Convert command to string
*/
std::string teacherTaskToString
(
    TeacherTask a
)
{
    switch( a )
    {
        case TEACHER_CMD_UNKNOWN            : return "TEACHER_CMD_UNKNOWN";
        case TEACHER_CMD_VALUE_TO_LAYER     : return "VALUE_TO_LAYER";
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
TeacherTask stringToTeacherTask
(
    std::string a
)
{
    if( a == "VALUE_TO_LAYER" )     return TEACHER_CMD_VALUE_TO_LAYER;
    if( a == "IMAGE_TO_LAYER" )     return TEACHER_CMD_IMAGE_TO_LAYER;
    if( a == "FOLDER_TO_LAYER" )    return TEACHER_CMD_FOLDER_TO_LAYER;
    if( a == "GUID_TO_LAYER" )      return TEACHER_CMD_GUID_TO_LAYER;
    if( a == "HID_TO_LAYER" )       return TEACHER_CMD_HID_TO_LAYER;
    return TEACHER_CMD_UNKNOWN;
}
