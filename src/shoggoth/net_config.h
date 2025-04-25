/*
    Network configuration
*/


#pragma once

#include <string>

#include "../../../../lib/json/param_list.h"    /* For read size from params */

#include "nerve.h"
#include "shoggoth_consts.h"



using namespace std;



class NetConfig : public ParamList
{
    private:
    public:

        /*
            Creator
        */
        static NetConfig* create();



        /*
            Loop for each layer
        */
        template <typename Func>
        NetConfig* layerLoop
        (
            /* Lambda function applied to each item */
            Func callback
        )
        {
            auto layers = getObject( Path{ "layers" });
            if( layers != NULL )
            {
                layers -> objectsLoop
                (
                    [ &callback ]
                    ( ParamList* item )
                    {
                        return callback( item );
                    }
                );

            }
            return this;
        };



        /*
            Loop for each nerve
        */
        template <typename Func>
        NetConfig* nerveLoop
        (
            /* Lambda function applied to each item */
            Func callback
        )
        {
            auto nerves = getObject( Path{ "nerves" });
            if( nerves != NULL )
            {
                nerves -> objectsLoop
                (
                    [ &callback ]
                    ( ParamList* item, string name )
                    {
                        return callback( item );
                    }
                );

            }
            return this;
        };



        /*
            Nerve delete by parent and child
        */
        NetConfig* nerveDelete
        (
            /* Parent id*/
            string,
            /* Child id */
            string
        );



        ParamList* getNerveByMutation
        (
            /* Mutation */
            ParamList*,
            /* Rnd */
            Rnd* aRnd
        );



        /*
            Create new layer between two other layers
        */
        ParamList* createLayer
        (
            /* New layer id*/
            string,
            /* Parent layer id*/
            string,
            /* Child layer id */
            string,
             /* Type pointer */
             ParamList* pointer,
            /* Interpolation */
            double
        );



        ParamList* createNerve
        (
            /* Parent layer id */
            string,
            /* Children layer id */
            string,
            /* Pattern */
            ParamList*
        );



        NetConfig* deleteNerve
        (
            /* Parent layer id */
            string,
            /* Children layer id */
            string
        );




        NetConfig* mutateInsertLayer
        (
            /* Mutation */
            ParamList*,
            /* rnd stream object for mutation */
            Rnd*
        );



        /*
            Return layer Params by id
        */
        ParamList* getLayerById
        (
            /* Layer id */
            string
        );



        /*
            Return layer type by layer id
        */
        string getLayerTypeById
        (
            /* Layer id */
            string
        );

};
