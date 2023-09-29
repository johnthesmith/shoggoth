#include <iostream>

/* User libraries */
#include "teacher.h"
#include "../lib/math.h"
#include "../lib/rnd.h"
#include "../json/param_list_log.h"



using namespace std;



/*
    Constructor
*/
Teacher::Teacher
(
    Net* aNet
)
: Payload( aNet -> getApplication() ) /* Call parent constructor */
{
    aNet -> getLog() -> trace( "Create teacher" );
    batches = ParamList::create();
    net = aNet;
}



/*
    Destructor
*/
Teacher::~Teacher()
{
    batches -> destroy();
    getLog() -> trace( "Destroy teacher" );
}



/*
    Creator
*/
Teacher* Teacher::create
(
    Net* a
)
{
    return new Teacher( a );
}



/*
    Destructor
*/
void Teacher::destroy()
{
    delete this;
}



/*
    Return application
*/
ShoggothApplication* Teacher::getApplication()
{
    return (ShoggothApplication*) Payload::getApplication();
}



/******************************************************************************
    Methods
*/



Teacher* Teacher::task()
{
    getLog() -> begin( "Check" );

    auto error = net -> getLayers() -> getById( idErrorLayer );

    if( error != NULL )
    {
        if( error -> getValue() <= errorLimit )
        {
            getLog() -> begin( "New batch" );
            auto item = batches -> getRnd();
            if( item != NULL && item -> isObject() )
            {
                auto batch = item -> getObject();
                ParamListLog::dump( getLog(), batch );
                batch -> loop
                (
                    [ this ]
                    ( Param* aParam )
                    {
                        auto layerId = aParam -> getName();
                        auto jobs = aParam -> getObject();
                        auto layer = net -> getLayers() -> getById( layerId );
                        if( layer != NULL )
                        {
                            auto job = jobs -> getRnd();
                            if( job != NULL )
                            {
                                char* buffer = NULL;
                                size_t size = 0;
//                                buildValueVuffer( job, buffer, size );
                                if( size != 0 )
                                {
                                    layer -> valuesFromBuffer( buffer, size );
                                }
                            }
                        }
                        return false;
                    }
                );

//            auto a = Rnd::get( 0, 10 );
//            Rnd::storeSeed( a );

//            auto retina = net -> getLayers() -> getById( "retina" );
//            auto sample = net -> getLayers() -> getById( "sample" );
//
//    if( retina != NULL && sample != NULL )
//    {
//        /* Set retina */
////        retina -> neurons -> loop
////        (
////            []( Neuron* neuron )
////            {
////                neuron -> setValue( Rnd::get( 0.0, 1.0 ) );
////                return false;
////            }
////        );
////        retina -> saveValue();
////
////        /* Set sample */
////        auto hid = Hid().setString( to_string( a ));
////        for( int i = 0; i < sample -> neurons -> getCount(); i ++ )
////        {
////            sample -> neurons
////            -> getByIndex( i )
////            -> setValue
////            (
////                hid.getBit( i ) && net -> getLearningMode() ? 1.0 : 0.0
////            );
////            sample -> saveValue();
////        }
//    }
//    else
//    {
//        getLog() -> warning( "NoLayerRetinaOrSampleInNet" );
//    }
//
//            Rnd::restoreSeed();
            }
            getLog() -> end();
        }
    }
    getLog() -> end();


    return this;
}



/******************************************************************************
    Setters and getters
*/



Teacher* Teacher::setErrorLimit
(
    double a
)
{
    errorLimit = a;
    return this;
}



double Teacher::getErrorLimit()
{
    return errorLimit;
}



Teacher* Teacher::setIdErrorLayer
(
    string a
)
{
    idErrorLayer = a;
    return this;
}



string Teacher::getIdErrorLayer()
{
    return idErrorLayer;
}



ParamList* Teacher::getBatches()
{
    return batches;
}
