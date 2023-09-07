#include "io.h"



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
    if( net -> getHost() == "" )
    {
        /* File operation */
        switch( aCommand )
        {
            case CMD_READ_NERVE     : fileReadNerve(); break;
            case CMD_WRITE_NERVE    : fileWriteNerve(); break;
            case CMD_READ_LAYER     : fileReadLayer(); break;
            case CMD_WRITE_LAYER    : fileWriteLayer(); break;
        }
    }
    else
    {
        /* Server operation */
        auto rpc = RpcClient::create
        (
            net -> getLog(),
            net -> getHost(),
            net -> getPort()
        )
        -> setRequest( request )
        -> setAnswer( answer )
        -> call( aCommand );

        rpc -> resultTo( this );
        rpc -> destroy();
    }
    return this;
}



/******************************************************************************
    Setters and getters
*/



/*
    Return the Net object
*/
Net* Io::getNet()
{
    return net;
}



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




Io* Io::fileReadNerve()
{
    return this;
}



Io* Io::fileWriteNerve()
{
    return this;
}



/*
    Write layer to file
*/
Io* Io::fileWriteLayer()
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



Io* Io::fileReadLayer()
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







