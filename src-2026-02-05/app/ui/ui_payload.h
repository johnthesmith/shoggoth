#pragma once

/* Local libraries */
#include "../../../../../lib/graph/scene_payload.h"
#include "../../../../../lib/graph/camera.h"
#include "../../shoggoth/limb/net.h"

/* Local libraries */
#include "./limb_ui.h"



using namespace std;



class UiPayload : public ScenePayload
{
    private:

        /* Shoggoth net */
        Net*        net                 = NULL;
        LimbUi*     limb                = NULL;

        /* Ui specific objects */
        Camera*     camera              = NULL;   /* Default camera */

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
        UiPayload
        (
            UiApplication*,
            /* Payload id */
            string,
            /* Net id */
            string,
            /* Net version */
            string
        );



        /*
            Destructor
        */
        ~UiPayload();



        /*
            Creator
        */
        static UiPayload* create
        (
            /* Application object */
            UiApplication*,
            /* Payload id */
            string,
            /* Net id */
            string,
            /* Net version */
            string
        );



        /*
            Destructor
        */
        void destroy() override;


        /*
            Return application object
        */
        UiApplication* getApplication() override;



        /*
            Help method
        */
        UiPayload* help();


        /******************************************************************************
            Payload emethods
        */


        /*
            Full screen
        */
        UiPayload* fillScreen
        (
            int
        );



        /******************************************************************************
            Payload events
        */

        virtual void onActivate
        (
            Scene&  /* Scene object */
        ) override;



        virtual void onCalc
        (
            Scene*  /* Scene calculate */
        ) override;



        virtual void onDraw
        (
            Scene*  /* Scene object */
        ) override;



        /*
            Event on key up
        */
        virtual void onKeyUp
        (
            Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        ) override;



        virtual void onKeyDown
        (
            Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        ) override;



        /*
            Custom on mouse move event
        */
        virtual void onMouseMove
        (
            Scene&,      /* Scene object */
            const Point3d&
        ) override;




        /*
            Mouse left drag begin
            Set left-top corner of selection box
        */
        virtual bool onLeftDragBegin
        (
            Scene&,             /* Scene object */
            const Point3d&
        ) override;



        /*
            Mouse left drag end
            End selection box
        */
        virtual void onLeftDragEnd
        (
            Scene&,             /* Scene object */
            const Point3d&
        ) override;



        /*
            Mouse left drag
        */
        virtual void onLeftDrag
        (
            Scene&,             /* Scene object */
            const Point3d&
        ) override;



        /*
            Mouse right drag
        */
        virtual void onRightDrag
        (
            Scene&,             /* Scene object */
            const Point3d&
        ) override;



        /*
            Mouse wheel event
        */
        virtual void onMouseWheel
        (
            Scene&, /* Scene object */
            const Point3d&
        ) override;




        /*
            On mouse left click event
        */
        virtual void onLeftClick
        (
            Scene&,         /* Scene object */
            const Point3d&, /* Mouse position */
            const int       /* Key mode */
        ) override;



        /*
            On mouse left click event
        */
        virtual void onLeftDblClick
        (
            Scene&,         /* Scene object */
            const Point3d&, /* Mouse position */
            const int       /* Key mode */
        ) override;



        /*
            Run ui loop
        */
        virtual void onLoop() override;


// TODO что это и зачем....

        virtual void onLoopBefore() override;



        /**********************************************************************
            Setters and getters
        */

        ShoggothRoleApplication* getApplication() override;
};
