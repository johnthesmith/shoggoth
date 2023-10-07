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
        case TEACHER_TASK_VALUE   : return "TEACHER_TASK_VALUE,";
        case TEACHER_TASK_IMAGE   : return "TEACHER_TASK_IMAGE";
        case TEACHER_TASK_GUID    : return "TEACHER_TASK_GUID";
        case TEACHER_TASK_HID     : return "TEACHER_TASK_HID";
        case TEACHER_TASK_NOISE   : return "TEACHER_TASK_NOISE";
    }
    return "TEACHER_TASK_UNKNOWN";
}



/*
    Convert string to teacher task
*/
TeacherTask stringToTeacherTask
(
    std::string a
)
{
    if( a == "TEACHER_TASK_VALUE" ) return TEACHER_TASK_VALUE;
    if( a == "TEACHER_TASK_IMAGE" ) return TEACHER_TASK_IMAGE;
    if( a == "TEACHER_TASK_GUID" )  return TEACHER_TASK_GUID;
    if( a == "TEACHER_TASK_HID" )   return TEACHER_TASK_HID;
    if( a == "TEACHER_TASK_NOISE" )  return TEACHER_TASK_NOISE;
    return TEACHER_TASK_UNKNOWN;
}
