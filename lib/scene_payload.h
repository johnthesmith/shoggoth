#pragma once

/* Local libraries */
#include "payload.h"
//#include "log.h"
//#include "scene.h"



using namespace std;



class Scene;

class ScenePayload : public Payload
{
    private:

        Scene* scene    = NULL;   /* Current scene for this payload */

    public:

        /*
            Transparent constructor
        */
        ScenePayload
        (
            Log& aLog   /* Log */
        )
        : Payload( aLog ) /* Call parent constructor */
        {};


        /*
            Set scene value
        */
        ScenePayload& setScene
        (
            Scene&
        );

        /*
            Get scene value
        */
        Scene& getScene();


        /*
            Draw method
        */
        virtual void draw
        (
            Scene& aScene   /* Scene object */
        );
};
