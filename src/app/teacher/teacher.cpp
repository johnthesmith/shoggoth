/*
    Teacher main file for Shoggoth neural network
*/

#include "../shoggoth_application.h"
#include "teacher_payload.h"

extern "C" Payload* create_payload
(
    Application* app,
    std::string aPayloadId
)
{
    return TeacherPayload::create
    (
        ( ShoggothApplication* ) app,
        aPayloadId
    );
}
