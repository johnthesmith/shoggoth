/*
    Shoggoth net object on limb based
    Its works like synchronizer between other limbs.
*/
#pragma once



#include <string>

#include "../../../../lib/core/rnd.h"
#include "../../../../lib/sock/sock_manager.h"


#include "limb.h"
#include "shoggoth_db.h"
#include "net_config.h"
#include "weights_exchange.h"
#include "../app/shoggoth_application.h"




class Net: public Limb
{
    private:

        /* Objects */

        /* The application object */
        ShoggothApplication*    application     = NULL;

        /* Socket manager object */
        SockManager*    sockManager     = NULL;

        /* Last net config after IO */
        ParamList*      config          = NULL;

        /* Monitoring object */
        Mon*            mon             = NULL;

        /* Rnd object */
        Rnd*            rnd             = NULL;

        /* Net database */
        ShoggothDb*     db              = nullptr;

        /* Lists of layers id after update by swap operations */
        vector<string>  changedValues;
        vector<string>  changedErrors;

        /* Weights request */
        WeightsExchange* weightsExchange = NULL;

        /* Net task */
        Task task                       = TASK_UNKNOWN;

        /* Synchronization states */
        /* Random vesion after load */
        int             randVersion     = 0;

        /* Protects net weights from writing */
        bool            weightWriteLock = false;
        /* Net id */
        string          id              = "";
        /* Net version, for switching at next turn of calculation */
        string          nextVersion     = "";
        /* Tick of the net. Settings by processor */
        unsigned long long tick         = 0;

        Net* loadNerves
        (
            /* Load nerves for applyNet */
            ParamList*,
            /* Task name */
            string
        );


    public:

        /*
            Constructor
        */
        Net
        (
            /* Application object */
            ShoggothApplication*,
            /* Socket manager */
            SockManager*,
            /* The net id */
            std::string,
            /* The net version */
            std::string,
            /* Task of the current net */
            Task
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
            /* Application object */
            ShoggothApplication*    aApplication,
            /* Socket manager object */
            SockManager*    aSockManager,
            /* The net id */
            std::string          aId,
            /* The net version */
            std::string          aVersion,
            /* The net version */
            Task            aTask
        )
        {
            return new Net
            (
                aApplication,
                aSockManager,
                aId,
                aVersion,
                aTask
            );
        }



        /*
            Destructor
        */
        inline void destroy()
        {
            delete ( this );
        }




        /*
            Read the nerve weights array from io
        */
        Net* readWeights
        (
            Nerve*,
            /* Connection */
            std::string = "default"
        );



        /*
            Write weights array to the io
        */
        Net* writeWeights
        (
            Nerve*,
            /* Connection */
            std::string = "default"
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
            Return application object
        */
        inline ShoggothApplication* getApplication()
        {
            return application;
        }



        /*
            Return log object
        */
        inline Log* getLog()
        {
            return application -> getLog();
        }



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
            /* Subpath */
            string = ""
        );



        /*
            Return net version path
        */
        string getNetVersionPath
        (
            /* Subpath */
            string = "",
            /* Specific version */
            string = ""
        );



        /*
            Return net config
        */
        string getNetConfigFile
        (
            /* Specific version */
            string = ""
        );



        /*
            Return log path
        */
        string getLogPath
        (
            /* Subpath */
            string = "",
            /* Specific version */
            string = ""
        );



        /*
            Return mon path
        */
        string getMonPath
        (
            /* Subpath */
            string = "",
            /* Specific version */
            string = ""
        );



        /*
            Return dump path
        */
        string getDumpPath
        (
            /* Subpath */
            string = "",
            /* Specific version */
            string = ""
        );



        /*
            Return path for nerves weights
        */
        string getNervesPath
        (
            /* Subpath */
            string = "",
            /* Specific version */
            string = ""
        );



        /*
            Return weights dump path
        */
        string getWeightsPath
        (
            string = "",
            /* Specific version */
            string = ""
        );



        /*
            Return net monitoring file
        */
        string getMonFile
        (
            /* Specific version */
            string = ""
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
            /* Answer */
            ParamList*,
            /* Connetion */
            std::string = "default"
        );



        bool readNetFromFile
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
            real,           /* survivalErrorAvg */
            Rnd*            /* rnd stream object for mutation */
        );



        Net* mutateChangeParam
        (
            ParamList*,
            ParamList*,
            /* rnd stream object for mutation */
            Rnd*
        );



        Net* applyNet
        (
            /* Config */
            ParamList*
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
            bool,        /* Skip action for locked */
            bool&,
            bool&
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
        Net* syncWithServer
        (
            /* Connection config */
            std::string = "default"
        );



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
            std::string,
            /* Current version of the net */
            std::string,
            /* Parent succes */
            bool
        );



        /*
            Calculate statistics for layers of the net
        */
        Net* stat();


        /*
            Request layers statistics
        */
        Net* requestStat
        (
            /* Layer list for stat request */
            vector<string>,
            /* Layer list for stat request */
            vector<string>,
            /* Layer list for stat request */
            vector<string>,
            /* Layer list for stat request */
            vector<string>,
            /* Connection config */
            std::string aConnection
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




        bool getWeightWriteLock()
        {
            return weightWriteLock;
        }



        Net* setWeightWriteLock
        (
            bool a
        )
        {
            weightWriteLock = a;
            return this;
        }



        ShoggothDb* getDb()
        {
            return db;
        }
};
