/*
    Shoggoth net object on limb based
    Its works like synchronizer between other limbs.
*/
#pragma once



#include <string>

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

        /* Net database */
        ShoggothDb*     db              = nullptr;

        /* Lists of layers id after update by swap operations */
        vector<string>  changedValues;
        vector<string>  changedErrors;

        /* Weights request */
        WeightsExchange* weightsExchange = NULL;

        /* Synchronization states */
        /* Random vesion after load */
        int             randVersion     = 0;

        /* Protects net weights from writing */
        bool            weightWriteLock = false;

        /* Net id */
        string          id              = "";
        /* Net version, for switching at next turn of calculation */
        string          nextVersion     = "";

    public:

        /*
            Constructor
        */
        Net
        (
            /* Application object */
            ShoggothApplication*,
            /* The net id */
            std::string,
            /* The net version */
            std::string
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
            /* The net id */
            std::string          aId,
            /* The net version */
            std::string          aVersion
        )
        {
            return new Net
            (
                aApplication,
                aId,
                aVersion
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
            string aVersion = ""
        )
        {
            return getNetVersionPath( "net.json", aVersion );
        }



        /*
            Return net weight file
        */
        string getNetWeightFile
        (
            /* Specific version */
            string aVersion = ""
        )
        {
            return getNetVersionPath( "weights.bin", aVersion );
        }



        /*
            Return log path
        */
        string getLogPath
        (
            /* Subpath */
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




        /******************************************************************************
            Layers
        */

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
            real            /* survivalErrorAvg */
        );



        Net* mutateChangeParam
        (
            ParamList*,
            ParamList*
        );



        Net* setCalcDebug
        (
            bool
        );


        bool getCalcDebug();



        int getCalcLayerIndex();



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
        bool valuesAndErrorsToLimb
        (
            /* Participant limb object */
            Limb*,
            /* List of layers for reading from net */
            vector<string>,
            /* Skip action for locked */
            bool
        );



        /*
            Swap layers between net and other participants for actions
        */
        bool valuesAndErrorsFromLimb
        (
            /* Participant limb object */
            Limb*,
            /* List of layers for writing to net */
            vector<string>,
            /* Skip action for locked */
            bool
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
            Return next version
        */
        string generateVersion
        (
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
            /* Net version */
            string,
            /* Parent generation (0 - current net) */
            int = 0
        );



        bool getWeightWriteLock()
        {
            return weightWriteLock;
        }



        ShoggothDb* getDb()
        {
            return db;
        }



        /*
            Fill weight for net uses config
        */
        Net* fillWeights()
        {
            lock();
            getNerveList() -> loop
            (
                [ this ]
                ( void* item )
                {
                    auto nerve = ( Nerve* )item;
                    nerve -> fill( getRnd() );
                    return false;
                }
            );
            unlock();
            return this;
        }

};

//
//1. выносим cfg в app
//2. переносим applycfg в limb
//3. добавляем в апп аплай
//4. удаляем синктулимб и делаем аплай
//5. делаем проверяем размножение текущей версии между лимбаим и цфг.
//6. в процессоре и в мемори делаем нету выделение памяти под вэйты
