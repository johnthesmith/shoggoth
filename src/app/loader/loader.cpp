/*
    Loader main file for Shoggoth neural network
*/

#include "../shoggoth_application.h"
#include "loader_payload.h"

extern "C" Payload* create_payload
(
    Application* app,
    std::string aPayloadId
)
{
    return LoaderPayload::create
    (
        ( ShoggothApplication* ) app,
        aPayloadId
    );
}
