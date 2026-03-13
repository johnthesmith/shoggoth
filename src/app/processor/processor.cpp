/*
    Processor main file for Shoggoth neural network
*/

#include "../shoggoth_application.h"
#include "processor_payload.h"

extern "C" Payload* create_payload
(
    Application* app,
    std::string aPayloadId
)
{
    return ProcessorPayload::create
    (
        ( ShoggothApplication* ) app,
        aPayloadId
    );
}
