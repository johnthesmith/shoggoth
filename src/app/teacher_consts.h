/*
    Shoggoth constants and conversion methods
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
    Convert Teachers task to string
*/
std::string teacherTaskToString
(
    TeacherTask
);



/*
    Convert string to teacher task
*/
TeacherTask stringToTeacherTask
(
    std::string
);
