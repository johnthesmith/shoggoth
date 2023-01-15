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
            Log& aLog   /* Log */
        )
        : ScenePayload( aLog ) /* Call parent constructor */
        {};


        virtual void onDraw
        (
            Scene& aScene   /* Scene object */
        );
};
