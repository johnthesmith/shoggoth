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
        ParamList*      config          = NULL;     /* Last net config after IO */

        /* Lists of layers id after update by swap operations */
        vector<string>  changedValues;
        vector<string>  changedErrors;

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
            Write layers to io
        */
        Net* writeLayers
        (
            LayerList* aValues,
            LayerList* aErrors
        );



        /*
            Read layers from io
        */
        Net* readLayers
        (
            LayerList* aValues,
            LayerList* aErrors
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
            Create roles strung of the process in ptu format
            P - process uses as processor
            T - process uses as teacher
            U - process has a UI interface
        */
        Net* buildPtuAndTasks();



        Net* setCalcDebug
        (
            bool
        );


        bool getCalcDebug();


        int getCalcLayerIndex();



        /*
            Return socket manager object
        */
        SockManager* getSockManager();



        /*
            Return config of the net
        */
        ParamList* getConfig();



        /*
            Swap layers between net and other participants for actions
        */
        Net* swapValuesAndErrors
        (
            Actions, /* Action list for participant */
            Task,    /* Task (role) of participant */
            Limb*    /* Participant */
        );



        Net* syncToLimb
        (
            Limb* targetLimb
        );




        /*
            Synchronaize with server
            For the modified layer, write to the server
            else read from the server.
        */
        Net* syncWithServer();



        /*
            Add layer to changed values list
        */
        Net* addChangedValues
        (
            Layer*
        );



        /*
            Add layer to changed values list
        */
        Net* addChangedErrors
        (
            Layer*
        );
};

