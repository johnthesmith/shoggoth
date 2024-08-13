#pragma once


#include <string>
#include "../../../../../lib/json/param_list.h"



/**********************************************************************
    Weights exchange buffer

    [
        {
            string  clientId
            string  layerId
            int     neuronIndex
            nerves  nerves
            [
                string layerParentId
                string layerChildId
                string bindType
                data   data
            ]
        }
    ]
*/



class WeightsExchange: public ParamList
{
    private:

        /* Client id */
        string  clientId = "";
        /*
            Requested weights clear signal
            true - the clear signal already sended to server
            false - need to sent the cliear signal
        */
        bool    clear       = true;



        /*
            Find request by arguments
        */
        Param* find
        (
            string,         /* Layer id */
            int,            /* neuron */
            string = ""     /* client id */
        );



    public:


        /*
            Constructor
        */
        WeightsExchange();



        /*
            Destructor
        */
        ~WeightsExchange();



        /*
            Create of the object
        */
        static WeightsExchange* create();



        /*
            Destroy of the object
        */
        void destroy();




        bool needRequest();



        WeightsExchange* prepareRequest
        (
            ParamList*
        );



        /*
            Prepare answer
        */
        WeightsExchange* prepareAnswer
        (
            string,     /* clienId */
            ParamList*  /* Neurons */
        );




        /*
        */
        WeightsExchange* synchNeuronsByClient
        (
            string,     /* clienId */
            ParamList*  /* Neurons */
        );


        /*
            Set clear flag to true
        */
        WeightsExchange* checkClearSignal();



        /**********************************************************************
            Client and server methods
        */
        WeightsExchange* add
        (
            string, /* Layer id */
            int,    /* neuron */
            string = ""
        );



        /*
            Remove all records by client id
        */
        WeightsExchange* purgeByClientId
        (
            string = ""     /* Client id */
        );


        /*
            Client method
        */
        WeightsExchange* readAnswer
        (
            ParamList* /* Readed answer */
        );
};
