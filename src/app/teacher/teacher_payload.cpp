/*
    TODO переделать на использование тещего модуля
*/

#include <iostream>
#include <cmath>

/* User libraries */
#include "teacher_application.h"
#include "teacher_payload.h"
#include "../../shoggoth/io.h"
#include "../../../../../lib/core/str.h"



using namespace std;



/*
    Constructor
*/
TeacherPayload::TeacherPayload
(
    TeacherApplication* a,
    string aNetId,
    string aNetVersion
)
: Payload( a ) /* Call parent constructor */
{
    net = Net::create( a, a -> getSockManager(), aNetId, aNetVersion );
    net -> addTask( TASK_TEACHER );
}



/*
    Destructor
*/
TeacherPayload::~TeacherPayload()
{
    net -> destroy();
}



/*
    Creator
*/
TeacherPayload* TeacherPayload::create
(
    TeacherApplication* a,
    string aNetId,
    string aNetVersion
)
{
    return new TeacherPayload( a, aNetId, aNetVersion );
}



/*
    Destructor
*/
void TeacherPayload::destroy()
{
    delete this;
}



/*
    Return application object
*/
TeacherApplication* TeacherPayload::getApplication()
{
    return ( TeacherApplication* ) Payload::getApplication();
}




/******************************************************************************
    Payload events
*/



/*
    Main loop event
*/
void TeacherPayload::onLoop()
{
    getLog() -> trapOn() -> begin( "Loop" );

    getMon()
    -> startTimer( Path{ "loop", "moment" })
    -> setString( Path{ "net", "id" }, net -> getId() )
    -> setString( Path{ "net", "version" }, net -> getVersion() )
    ;

    if( getApplication() -> getCli() -> getString( "config" ) == "" )
    {
        getLog() -> error( "Need the --config cli argument." );
        terminate();
    }
    else
    {
        /* Check local application config */
        getApplication() -> checkConfigUpdate();

        if( getApplication() -> getConfigUpdated() )
        {
            auto cfg = getApplication() -> getConfig();

            if( cfg != NULL )
            {
                setLoopTimeoutMcs( cfg -> getDouble( "loopSleepMcs", 100 ));
            }
        }

        /* Check server net config */
        auto netConfig = ParamList::create();

        /* Read net config from server */
        net -> readNet( netConfig );

        if
        (
            net -> isConfigUpdate( netConfig ) ||
            net -> isVersionChanged()
        )
        {
            getLog()
            -> begin( "Config updated" )
            -> prm( "File", getApplication() -> getConfigFileName() )
            -> dump( netConfig, "Net config" )
            -> lineEnd();

            net -> applyNet( netConfig );

            getLog() -> end();
        }

        netConfig -> destroy();

        /* Synchronize net from the server */
        net -> syncWithServer();

        /* Processing Teacher */
        processing();
    }

    getMon() -> flush();
    getLog() -> end( getCode() ) -> trapOff();
}



/******************************************************************************
    Private methods
*/



/*
    Processing of the loop
*/
TeacherPayload* TeacherPayload::processing()
{
    getLog() -> begin( "Processing" );
    getLog() -> end();
    return this;
}
