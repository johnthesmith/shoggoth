/*
    Client main file for Shoggoth neural network
*/

#include "../shoggoth_application.h"
#include "client_payload.h"

extern "C" Payload* create_payload
(
    Application* app,
    std::string aPayloadId
)
{
    return ClientPayload::create
    (
        ( ShoggothApplication* ) app,
        aPayloadId
    );
}
