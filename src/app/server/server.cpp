/*
    Processor main file for Shoggoth neural network
*/
#include "../shoggoth_application.h"
#include "server_payload.h"

extern "C" Payload* create_payload
(
    Application* app,
    std::string aPayloadId
)
{
    return ServerPayload::create
    (
        ( ShoggothApplication* ) app,
        aPayloadId
    );
}
