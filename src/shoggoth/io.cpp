#include "io.h"

using namespace std;



/*
    Constructor
*/
Io::Io
(
    Net* aNet,          /* Net object */
    ParamList* aAnswer  /* Answer object */
)
{
    net = aNet;
    request = ParamList::create();
    if( aAnswer == NULL )
    {
        answer = ParamList::create();
        answerOwner = true;
    }
    else
    {
        answer = aAnswer;
    }
}



/*
    Destructor
*/
Io::~Io()
{
    request -> destroy();
    if( answerOwner )
    {
        answer -> destroy();
    }
}



/*
    Create the Io
*/
Io* Io::create
(
    Net* aNet,   /* Application object */
    ParamList* aAnswer  /* Answer object */
)
{
    auto result = new Io( aNet, aAnswer );
    return result;
}



/*
    Self destroy the Io
*/
void Io::destroy()
{
    delete this;
}



/*
    Call input output operation
*/
Io* Io::call
(
    Command aCommand    /* Command */
)
{
    if( this -> isOk() )
    {
        auto config = getApplication()
        -> getConfig()
        -> selectObject( Path { "io" });
        /* Read configuration */
        if( config -> getString( Path{ "source" }, "LOCAL" ) == "LOCAL" )
        {
            /* File operation */
            switch( aCommand )
            {
                case CMD_READ_NET       : fileReadNet(); break;
                case CMD_WRITE_LAYERS   : // fileWriteValues(); break;
                case CMD_READ_LAYERS    : // fileReadErrors(); break;
                case CMD_READ_WEIGHTS   : fileReadWeights(); break;
                case CMD_WRITE_WEIGHTS  : fileWriteWeights(); break;
                default                 : setCode( "UnknownCommand" ); break;
            }
        }
        else
        {
            auto  host = config -> getString( Path{ "host" });
            auto  port = config -> getInt( Path{ "port" });
            auto  readWaitingTimeoutMcs = config -> getInt( Path{ "readWaitingTimeoutMcs" });

            /* Server operation */
            auto client = RpcClient::create
            (
                getApplication() -> getLogManager(),
                net -> getSockManager(),
                host,
                port
            )
            -> setOnBeforeCall( [ this ]( RpcClient* a ){ handleBeforeCall( a ); } )
            -> setOnAfterCall( [ this ]( RpcClient* a ){ handleAfterCall( a ); } )
            ;

            client -> setReadWaitingTimeoutMcs( readWaitingTimeoutMcs );
            client
            -> setRequest( request )
            -> setAnswer( answer )
            -> call( commandToString( aCommand ))
            -> resultTo( this )
            -> destroy();
        }

        /*
            Return code
        */
        if( !isOk() )
        {
            getLog()
            -> warning( "Shoggot IO error" )
            -> prm( "command", commandToString( aCommand ))
            -> prm( "code", getCode() )
            -> dump( getDetails(), "error details" )
            -> dump( request, "request" )
            -> lineEnd();
        }
    }

    return this;
}



/*
    Call input output operation
*/
Io* Io::disconnect()
{
    auto config = getApplication()
    -> getConfig()
    -> selectObject( Path { "io" });

    /* Read configuration */
    if( config -> getString( Path{ "source" }, "LOCAL" ) != "LOCAL" )
    {
        /* Server operation */
        RpcClient::create
        (
            getApplication() -> getLogManager(),
            net -> getSockManager(),
            config -> getString( Path{ "host" }),
            config -> getInt( Path{ "port" })
        )
        -> disconnect()
        -> destroy();
    }
    return this;
}



/******************************************************************************
    Setters and getters
*/


/*
    Return the Application object
*/
Application* Io::getApplication()
{
    return net -> getApplication();
}



/*
    Return log
*/
Log* Io::getLog()
{
    return net -> getLog();
}



/*
    Return answer parametes
*/
ParamList* Io::getAnswer()
{
    return answer;
}



/*
    Return request parametes
*/
ParamList* Io::getRequest()
{
    return request;
}



/******************************************************************************
    File operation section
*/



/*
    Read net from file
*/
Io* Io::fileReadNet()
{
    net -> readNetFromFile( answer );
    return this;
}





/*
    Write layer to file
*/
Io* Io::fileWriteValues()
{
//    if( aFilePath != "" )
//    {
//        if( !checkStorage( aFilePath ) )
//        {
//            createStorage( aFilePath );
//        }
//
//        if( checkStorage( aFilePath ) )
//        {
//            if( aBuffer != NULL )
//            {
//                auto h = fopen( aFilePath.c_str(), "w" );
//                if( h )
//                {
//                    if( fwrite( &aBuffer, aSize, 1, h ) == 0 )
//                    {
//                        setResult( "WriteToStorageError" );
//                    }
//                    fclose( h );
//                }
//                else
//                {
//                    setResult( "SaveOpenStorageError" );
//                }
//            }
//        }
//        else
//        {
//            setResult( "SaveCheckStorageError" );
//        }
//    }
    return this;
}



Io* Io::fileReadValues()
{
//    if( aFilePath != "" )
//    {
//        if( !checkStorage( aFilePath ) )
//        {
//            setResult( "LoadCheckStorageError" );
//        }
//        else
//        {
//            auto h = fopen( aFilePath.c_str(), "r" );
//            if( !h )
//            {
//                setResult( "LoadOpenStorageError" );
//            }
//            else
//            {
//                loop
//                (
//                    [ this, &h  ]
//                    (
//                        Neuron* iNeuron
//                    )
//                    {
//                        StorageNeuron part;
//                        auto readed = fread( &part, sizeof( part ), 1, h );
//                        iNeuron -> setValue( part.value );
//                        iNeuron -> setError( part.error );
//                        auto stop = readed != 1;
//                        if( stop )
//                        {
//                            setResult( "FileReadError" );
//                        }
//                        return stop;
//                    }
//                );
//                fclose( h );
//            }
//        }
//    }
    return this;
}



/*
    Write layers errors to file
*/
Io* Io::fileWriteErrors()
{
    return this;
}



/*
    Read layer errors from file
*/
Io* Io::fileReadErrors()
{
    return this;
}



/*
    Write nerve weights to file
*/
Io* Io::fileWriteWeights()
{
    return this;
}



/*
    Read nerve weights from file
*/
Io* Io::fileReadWeights()
{
    return this;
}



/******************************************************************************
    Shoggoth Rpc Client methods
*/



/*
    Clone net from id and version
*/
Io* Io::cloneNet
(
    string aNetId,
    string aNetVersion,
    int aGeneration,
    bool aMutation
)
{
    if( isOk() )
    {
        request
        -> clear()
        -> setString( "parentNetId", aNetId )
        -> setString( "parentNetVersion", aNetVersion )
        -> setInt( "parentGeneration", aGeneration )
        -> setBool( "mutation", aMutation );

        call( CMD_CLONE_NET );
    }
    return this;
}



/*
    Switch net from id and version
*/
Io* Io::switchNet
(
    string aId,
    string aVersion
)
{
    if( this -> isOk() )
    {
        request
        -> clear()
        -> setString( "id", aId )
        -> setString( "version", aVersion );

        call( CMD_SWITCH_NET );
    }
    return this;
}



/*
    Return server net mode
*/
Io* Io::getNetMode
(
    NetMode &a
)
{
    if( this -> isOk() )
    {
        call( CMD_GET_NET_MODE );
        a = netModeFromString( answer -> getString( Path{ "mode" }));
    }
    return this;
}



/*
    Set server net mode
*/
Io* Io::setNetMode
(
    NetMode aMode,
    ParamList* aReason
)
{
    if( this -> isOk() )
    {
        request
        -> setString( "mode", netModeToString( aMode ))
        -> copyFrom( "reason", aReason );

        call( CMD_SET_NET_MODE );
    }
    return this;
}


/*
    Event handlers
*/


void Io::handleBeforeCall
(
    RpcClient* a
)
{

//    if
//    (
//        getApplication()
//        -> getConfig()
//        -> valueExists
//        (
//            Path{ "io", "methodDump" },
//            a -> getRequest() -> getString( Path{ "method" })
//        )
//    )
    {
        getLog()
        -> begin( "Call RPC" ) /* End of log in the Io::handleAfterCall */
        -> dump( a -> getRequest(), "Request" )
        ;
    }
}



void Io::handleAfterCall
(
    RpcClient* a
)
{
//    if
//    (
//        getApplication()
//        -> getConfig()
//        -> valueExists
//        (
//            Path{ "io", "methodDump" },
//            a -> getRequest() -> getString( Path{ "method" })
//        )
//    )
    {
        getLog()
        -> dump( a -> getAnswer(), "Response" )
        -> end();
    }
}



