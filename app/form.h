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



        virtual void onKeyUp
        (
            Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        );
};
