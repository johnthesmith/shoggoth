/*
    Shoggoth net object on limb based
    Its works like synchronizer between other limbs.
*/





#pragma once



#include <string>

#include "../../../../../lib/core/application.h"
#include "../../../../../lib/sock/sock_manager.h"

#include "../limb.h"

#include "weights_exchange.h"



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

        /* Weights request */
        WeightsExchange* weightsExchange = NULL;


        /* Events */
        ParamList* tasks           = NULL;     /* List of participants tasks */

        /* Synchronization states */
        int             randVersion     = 0;        /* Random vesion after load */

        /*
            Storage settings
            The net contains at storage
                storate - value from net storagePath
                    id - value from net id
                        net.json - net configuration
                        versions - version of net
                            log - log files
                            mon - mon files
                            weights - weighs data
        */

        string          storagePath     = "net";    /* Path for storage */
        string          id              = "";       /* Net id */
        /* Current net version */
        string          version         = "";
        /* Net version, for switching at next turn of calculation */
        string          nextVersion   = "";

    public:

        /*
            Constructor
        */
        Net
        (
            Application*,   /* Application object */
            SockManager*,   /* Socket manager */
            string,         /* The net id */
            string          /* The net version */
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
            Application*,   /* Application object */
            SockManager*,   /* Socket manager */
            string,         /* The net id */
            string          /* The net version */
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


        /*
            Set storage path
        */
        Net* setStoragePath
        (
            const string
        );



        /*
            Get storage path
        */
        string getStoragePath();



        /*
            Return net id
        */
        string getId();



        /*
            Return net id
        */
        Net* setId
        (
            string
        );



        /*
            Return net version
        */
        string getVersion();



        /*
            Set next wersion for switching
        */
        Net* setNextVersion
        (
            string
        );


        /******************************************************************************
            Pathes
        */



        /*
            Return net path
        */
        string getNetPath
        (
            string = "", /* Subpath */
            string = ""  /* Net id */
        );



        /*
            Return net version path
        */
        string getNetVersionPath
        (
            string = "",    /* Subpath */
            string = "",    /* Specific version */
            string = ""     /* Specific net id */
        );



        /*
            Return net config
        */
        string getNetConfigFile
        (
            string = "",    /* Specific version */
            string = ""     /* Specific net id */
        );



        /*
            Return log path
        */
        string getLogPath
        (
            string = "",    /* Subpath */
            string = "",    /* Specific version */
            string = ""     /* Specific net id */
        );



        /*
            Return mon path
        */
        string getMonPath
        (
            string = "",    /* Subpath */
            string = "",    /* Specific version */
            string = ""     /* Specific net id */
        );



        /*
            Return dump path
        */
        string getDumpPath
        (
            string = "",    /* Subpath */
            string = "",    /* Specific version */
            string = ""     /* Specific net id */
        );



        /*
            Return path for nerves weights
        */
        string getNervesPath
        (
            string = "",    /* Subpath */
            string = "",    /* Specific version */
            string = ""     /* Specific net id */
        );



        /*
            Return weights dump path
        */
        string getWeightsPath
        (
            string = "",
            string = "",    /* Specific version */
            string = ""     /* Specific net id */
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
        Net* readNet
        (
            ParamList*  /* Answer */
        );



        bool isConfigUpdate
        (
            ParamList*  /* Config */
        );



        /*
            Clone net form parent to child
        */
        Net* clone
        (
            string,         /* Parent Net Id */
            string,         /* Parent Net Version */
            string&,        /* result child version */
            bool = false    /* true for mutation */
        );



        Net* applyNet
        (
            ParamList* /* Config */
        );



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
            Load selected weights to this net from the limb argument
        */
        Net* loadWeightsFrom
        (
            Limb*   /* Sorce */
        );


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



        /*
            Create roles strung of the process
        */
        Net* buildTasks();



        /*
            Add role for this net
        */
        Net* addTask
        (
            Task    /* Adding task */
        );



        WeightsExchange* getWeightsExchange();



        /*
            Request weights for neurons
        */
        Net* requestWeights();



        /*
            Return true when nextVersion not equal verson
        */
        bool isVersionChanged();



        /*
            Return the next version of the net
        */
        string getNextVersion();



        /*
            Return parent net version
        */
        string getParentVersion();


        /*
            Calculate statistics for layers of the net
        */
        Net* stat();


        Net* requestStat
        (
            LayerList*,
            LayerList*,
            LayerList*
        );

};
