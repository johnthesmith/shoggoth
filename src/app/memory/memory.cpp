/*
    Client main file for Shoggoth memory
*/

#include "../shoggoth_application.h"
#include "memory_payload.h"

extern "C" Payload* create_payload
(
    Application* app,
    std::string aPayloadId
)
{
    return MemoryPayload::create
    (
        ( ShoggothApplication* ) app,
        aPayloadId
    );
}
