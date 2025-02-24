/*
    Shoggoth net object on limb based
    Its works like synchronizer between other limbs.
*/
#pragma once



#include <string>

#include "../../../../../lib/core/application.h"
#include "../../../../../lib/core/rnd.h"
#include "../../../../../lib/sock/sock_manager.h"

#include "../limb.h"

#include "weights_exchange.h"



class Net: public Limb
{
    private:

        /* Objects */

        /* The application object */
        Application*    application     = NULL;

        /* Socket manager object */
        SockManager*    sockManager     = NULL;

        /* Last net config after IO */
        ParamList*      config          = NULL;

        /* Monitoring object */
        Mon*            mon             = NULL;

        /* Rnd object */
        Rnd*            rnd             = NULL;

        /* Lists of layers id after update by swap operations */
        vector<string>  changedValues;
        vector<string>  changedErrors;

        /* Weights request */
        WeightsExchange* weightsExchange = NULL;

        /* Net task */
        Task task                       = TASK_UNKNOWN;

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
        string          nextVersion     = "";
        /* Tick of the net. Settings by processor */
        unsigned long long tick         = 0;
    public:

        /*
            Constructor
        */
        Net
        (
            Application*,   /* Application object */
            SockManager*,   /* Socket manager */
            string,         /* The net id */
            string,         /* The net version */
            Task            /* Task of the current net */
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
            string,         /* The net version */
            Task            /* Task of the current net */
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
            vector<string>, /* List of layers id for the values write */
            vector<string>  /* List of layers id for the errors write */
        );



        /*
            Read layers from io
        */
        Net* readLayers
        (
            vector<string>, /* List of layers id for the values write */
            vector<string>  /* List of layers id for the errors write */
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



        /*
            Return net monitoring file
        */
        string getMonFile
        (
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



        Net* readNetFromFile
        (
            ParamList* /* Answer */
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
            string,         /* New net version */
            double,         /* survivalErrorAvg */
            Rnd*            /* rnd stream object for mutation */
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
            Actions,    /* Action list for participant */
            Limb*,      /* Participant */
            bool        /* Skip action for locked */
        );



        /*
            Send changes from net in to other limb
        */
        bool syncToLimb
        (
            Limb*   /* targetLimb */,
            bool    /* Skip synchronization for Net is lock */
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
            Return next version
        */
        string generateVersion
        (
            /* Id of the net */
            string,
            /* Current version of the net */
            string,
            /* Parent succes */
            bool
        );



        /*
            Calculate statistics for layers of the net
        */
        Net* stat();


        Net* requestStat
        (
            vector<string>,
            vector<string>,
            vector<string>,
            vector<string>
        );



        /*
            Reurn parent net version
        */
        string getParentVersion
        (
            /* Net ID (not used) */
            string,
            /* Net version */
            string,
            /* Parent generation (0 - current net) */
            int = 0
        );



        /*
            Return true value if layer contains action for current net task
        */
        bool checkLayerAction
        (
            Layer*,
            Action
        );



        /*
            Return the tick of the net
        */
        long long int getTick();



        /*
            Return the tick of the net
        */
        Net* setTick
        (
            /* Tick number */
            long long int
        );



        /*
            Tick increment
        */
        Net* incTick();



        /*
            Return net mon object
        */
        Mon* getMon();



        Rnd* getRnd();



        Net* setRndSeedFromConfig();
};
