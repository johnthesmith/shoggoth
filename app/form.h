#pragma once

/* Local libraries */
#include "../lib/scene_payload.h"


using namespace std;


class Form : public ScenePayload
{
    public:
        /*
            Constructor
        */
        Form
        (
            Log& aLog
        )
        : ScenePayload( aLog ) /* Call parent constructor */
        {};

        virtual void draw
        (
            Scene& aScene   /* Scene object */
        ) override;
};
