#include <iostream>

/* User libraries */
#include "teacher.h"
#include "../lib/math.h"
#include "../lib/rnd.h"



using namespace std;



/*
    Constructor
*/
Teacher::Teacher
(
    ShoggothApplication* a
)
: Payload( a ) /* Call parent constructor */
{
    batch = Json::create();
    net = Net::create( a );
}



/*
    Destructor
*/
Teacher::~Teacher()
{
    batch -> destroy();
    net -> destroy();
}



/*
    Creator
*/
Teacher* Teacher::create
(
    ShoggothApplication* a
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



ShoggothApplication* Teacher::getApplication()
{
    return (ShoggothApplication*) Payload::getApplication();
}



/******************************************************************************
    Methods
*/



/*
    Run payload
*/
void Teacher::onLoop
(
    bool&           aTreminated,
    bool&           aIdling,
    unsigned int&   sleep,
    bool&           aReconfig
)
{
    /* Check file update */
    getApplication() -> checkConfigUpdate();

    if( getApplication() -> getConfigUpdated() )
    {
        getLog()
        -> trace( "Teacher updated" )
        -> prm( "File", getApplication()
        -> getConfigFileName() );
    }

    auto config = getApplication() -> getConfig();

    if( config -> isOk() )
    {
        auto errorId = config -> getString( "error" );
        net -> setStoragePath( config -> getString( "storagePath", net -> getStoragePath() ));
//        net -> use();

        /* Create error layer in net */
//        auto errorLayer = net -> getLayers() -> getById( errorId );
//        if( errorLayer != NULL )
//        {
//            errorLayer -> loadValue();
//            if( errorLayer -> isOk())
//            {
//                getLog()
//                -> trace( "value" )
//                -> prm( "value", errorLayer -> getValue() );
//
//                if( errorLayer -> getValue() < 0.001 )
//                {
//                    task();
//                }
//            }
//            else
//            {
//                getLog() -> warning( errorLayer -> getCode() );
//            }
//        }
    }
}



Teacher* Teacher::task()
{
    getLog() -> begin( "new task" );

    auto a = Rnd::get( 0, 10 );
    Rnd::storeSeed( a );

    auto retina = net -> getLayers() -> getById( "retina" );
    auto sample = net -> getLayers() -> getById( "sample" );

    if( retina != NULL && sample != NULL )
    {
        /* Set retina */
//        retina -> neurons -> loop
//        (
//            []( Neuron* neuron )
//            {
//                neuron -> setValue( Rnd::get( 0.0, 1.0 ) );
//                return false;
//            }
//        );
//        retina -> saveValue();
//
//        /* Set sample */
//        auto hid = Hid().setString( to_string( a ));
//        for( int i = 0; i < sample -> neurons -> getCount(); i ++ )
//        {
//            sample -> neurons
//            -> getByIndex( i )
//            -> setValue
//            (
//                hid.getBit( i ) && net -> getLearningMode() ? 1.0 : 0.0
//            );
//            sample -> saveValue();
//        }
    }
    else
    {
        getLog() -> warning( "NoLayerRetinaOrSampleInNet" );
    }

    getLog() -> end();

    Rnd::restoreSeed();

    return this;
}
