#include "io.h"
#include "../../../../lib/json/param_list_log.h"
#include "../../../../lib/sock/rpc_client.h"


using namespace std;



/*
    Constructor
*/
Io::Io
(
    Net* aNet   /* Net object */
)
{
    net = aNet;
    request = ParamList::create();
    answer = ParamList::create();
}



/*
    Destructor
*/
Io::~Io()
{
    request -> destroy();
    answer -> destroy();
}



/*
    Create the Io
*/
Io* Io::create
(
    Net* aNet   /* Application object */
)
{
    auto result = new Io( aNet );
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
        if( config -> getString( "source", "LOCAL" ) == "LOCAL" )
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
            string  host = config -> getString( "host" );
            int     port = config -> getInt( "port" );

            /* Server operation */
            RpcClient::create
            (
                getApplication() -> getLogManager(),
                net -> getSockManager(),
                host,
                port
            )
            -> setRequest( request )
            -> setAnswer( answer )
            -> call( aCommand )
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
            -> prm( "code", getCode() );

            ParamListLog::dump( getLog(), request );
        }
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
    string net = getApplication()
    -> getConfig()
    -> getString( Path { "io", "net" });

    getLog() -> trace( "Net config read" ) -> prm( "file", net );

    Json::create()
    -> fromFile( net )
    -> copyTo( answer )
    -> resultTo( this )
    -> destroy();

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
