/*
    Shoggoth constants and conversion methods
*/

#pragma once


/*
    List with type of teachers tasks
*/
enum TeacherTask
{
    TEACHER_TASK_UNKNOWN,
    TEACHER_TASK_VALUE,
    TEACHER_TASK_IMAGE,
    TEACHER_TASK_GUID,
    TEACHER_TASK_HID,
    TEACHER_TASK_NOISE
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
