#pragma once

/* Local libraries */
#include "../lib/scene_payload.h"
#include "../lib/camera.h"
#include "../lib/neuron/layer.h"



using namespace std;



class Form : public ScenePayload
{
    private:

        Camera camera = Camera();   /* Default camera */

        Layer* layer1  = NULL;
        Layer* layer2  = NULL;
        Layer* layer3  = NULL;
        Layer* layer4  = NULL;
        Layer* layer5  = NULL;
        Layer* layer6  = NULL;

        Point3d selectTopLeft = POINT_3D_0;
        Point3d selectBottomRight = POINT_3D_0;

        Rgba interfaceColor = Rgba( 0.5, 1.0, 0.5, 0.7 );
        Rgba interfaceColorDark = Rgba( 0.1, 0.4, 0.1, 0.7 );

    public:

        /*
            Constructor
        */
        Form
        (
            Log& /* Log */
        );



        /*
            Destructor
        */
        ~Form();



        /*
            Creator
        */
        static Form* create
        (
            Log& aLog
        );



        /*
            Destructor
        */
        void destroy();



        /******************************************************************************
        */


        virtual void onActivate
        (
            Scene&  /* Scene object */
        );



        virtual void onDraw
        (
            Scene&  /* Scene object */
        );



        virtual void onKeyUp
        (
            Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        );




        virtual void onKeyDown
        (
            Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        );



        /*
            Custom on mouse move event
        */
        virtual void onMouseMove
        (
            Scene&,      /* Scene object */
            const Point3d&
        );




        /*
            Mouse left drag begin
            Set left-top corner of selection box
        */
        virtual void onLeftDragBegin
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Mouse left drag end
            End selection box
        */
        virtual void onLeftDragEnd
        (
            Scene&,             /* Scene object */
            const Point3d&
        );





        /*
            Mouse left drag
        */
        virtual void onLeftDrag
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Mouse right drag
        */
        virtual void onRightDrag
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Mouse wheel event
        */
        virtual void onMouseWheel
        (
            Scene&, /* Scene object */
            const Point3d&
        );
};
