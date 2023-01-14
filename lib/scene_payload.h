#pragma once

/* Local libraries */
#include "result.h"
#include "log.h"
//#include "scene.h"



using namespace std;



class Scene;

class ScenePayload : public Result
{
    private:

        Log& log;
        Scene* scene    = NULL;   /* Current scene for this payload */

    public:
        /*
            Constructor
        */
        ScenePayload
        (
            Log&
        );

        ~ScenePayload();

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
            Get scene value
        */
        Log& getLog();

        /*
            Draw method
        */
        virtual void draw
        (
            Scene& aScene   /* Scene object */
        ) = 0;
};
