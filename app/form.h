#pragma once

/* Local libraries */
#include "../lib/scene_payload.h"
#include "../lib/camera.h"



using namespace std;



class Form : public ScenePayload
{
    private:

        Camera camera = Camera();   /* Default camera */

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



        virtual void onKeyUp
        (
            Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        );
};
