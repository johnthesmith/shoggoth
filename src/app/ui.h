#pragma once

/* Local libraries */
 #include "../../../../lib/graph/scene_payload.h"
#include "../../../../lib/graph/camera.h"
#include "../shoggoth/layer.h"
#include "../shoggoth/limb/limb_ui.h"

#include "shoggoth_application.h"



using namespace std;



class Ui : public ScenePayload
{
    private:

        Camera*     camera              = NULL;   /* Default camera */
        LimbUi*     limb                = NULL;

        Point3d     selectTopLeft       = POINT_3D_0;
        Point3d     selectBottomRight   = POINT_3D_0;

        Rgba        interfaceColor      = Rgba( 0.4, 1.0, 0.7, 0.7 );
        Rgba        interfaceColorDark  = Rgba( 0.1, 0.4, 0.2, 0.7 );

        long int    lastConfigCheck     = 0;

        string      selectedLayerId     = "";

    public:

        /*
            Constructor
        */
        Ui
        (
            Net*
        );



        /*
            Destructor
        */
        ~Ui();



        /*
            Creator
        */
        static Ui* create
        (
            Net*
        );



        /*
            Destructor
        */
        void destroy();



        ShoggothApplication* getApplication() override;



        Ui* help();



        /*
        */
        Ui* fillScreen
        (
            int
        );



        /**********************************************************************
            Events
        */


        virtual void onActivate
        (
            Scene&  /* Scene object */
        );



        virtual void onCalc
        (
            Scene*  /* Scene calculate */
        );



        virtual void onDraw
        (
            Scene*  /* Scene object */
        );



        /*
            Event on key up
        */
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
        virtual bool onLeftDragBegin
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




        /*
            On mouse left click event
        */
        virtual void onLeftClick
        (
            Scene&,         /* Scene object */
            const Point3d&, /* Mouse position */
            const int       /* Key mode */
        );



        /*
            On mouse left click event
        */
        virtual void onLeftDblClick
        (
            Scene&,         /* Scene object */
            const Point3d&, /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Run ui loop
        */
        virtual void onLoop();


        virtual void onLoopBefore();

};
