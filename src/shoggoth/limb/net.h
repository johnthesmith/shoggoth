/*
    Shoggoth net object on limb based
    Its works like synchronizer between other limbs
*/

#pragma once



#include <string>

#include "../../../../../lib/core/application.h"
#include "../../../../../lib/sock/sock_manager.h"

#include "../limb.h"



class Net: public Limb
{
    private:

        /* Objects */

        Application*    application     = NULL;     /* The application object */
        SockManager*    sockManager     = NULL;     /* Socket manager object */

        /* Events */
        string          ptu             = "***";    /* Roles of the Net in PTU */
        ParamListFile*  actions         = NULL;     /* Structure for resolve [actions] = f( ptu, event ) */
        ParamList*      tasks           = NULL;     /* List of participants tasks */

        /* Synchronization states */
        int             randVersion     = 0;        /* Random vesion after load */

        /* Settings */
        string          id              = "";       /* Net id */
        string          storagePath     = "net";

    public:

        /*
            Constructor
        */
        Net
        (
            Application*,   /* Application object */
            SockManager*
        );



        /*
            Destructor
        */
        ~Net();



        /*
            Static constructor for parent net
        */
        static Net* create
        (
            Application*,
            SockManager*
        );



        /*
            Destructor
        */
        void destroy();



        /*
            Read the nerve weights array from io
        */
        Net* readWeights
        (
            Nerve*
        );



        /*
            Write weights array to the io
        */
        Net* writeWeights
        (
            Nerve*
        );



        /*
            Write value to io
        */
        Net* writeValues
        (
            Layer*
        );



        /*
            Read values from io
        */
        Net* readValues
        (
            Layer*
        );



        /*
            Write errors to io
        */
        Net* writeErrors
        (
            Layer* aLayer
        );



        /*
            Read value from io
        */
        Net* readErrors
        (
            Layer* aLayer
        );



        /*
            Read the layers values from io
        */
        Net* readValuesList
        (
            ParamList* = NULL
        );



        /*
            Write the layers values to the io
        */
        Net* writeValuesList
        (
            ParamList* = NULL
        );



        /*
            Read the layers errors from io
        */
        Net* readErrorsList
        (
            ParamList* = NULL
        );



        /*
            Write the layers errors to the io
        */
        Net* writeErrorsList
        (
            ParamList* = NULL
        );




        Net* readWeightsList();



        Net* writeWeightsList();




        /*
            Return layer by Id
        */
        Layer* getLayerById
        (
            string /* Id of layer */
        );



        /*
            Request actual data layer form server
        */
        Net* writeLayersToServer();



        /*
            Return application object
        */
        Application* getApplication();




        /*
            Return log object
        */
        Log* getLog();



        Net* setStoragePath
        (
            const string
        );



        string getStoragePath();



        Net* setId
        (
            string
        );


        /******************************************************************************
            Layers
        */



        /*
            Load layer structure from param list
            Layer may be resized.
        */
        Net* loadLayer
        (
            Layer*,
            ParamList*
        );



        /*
            Load layer from net.layers section
        */
        Layer* loadLayerFromConfig
        (
            string,
            ParamList*
        );



        /*
            Remove layers absent in the list
        */
        Net* purgeLayers
        (
            ParamList*
        );



        /*
            Read net configuration and reallocate net objects
        */
        Net* readNet();



        /*
            Main event handler
        */
        Net* event
        (
            Event
        );



        /*
            Create roles strung of the process in ptu format
            P - process uses as processor
            T - process uses as teacher
            U - process has a UI interface
        */
        Net* buildPtuAndTasks();



// TODO for remove        bool isNextLoop();


        Net* setCalcDebug
        (
            bool
        );


        bool getCalcDebug();


        int getCalcLayerIndex();


        SockManager* getSockManager();
};

