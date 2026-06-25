/*
    This file of the Shoggoth memory

    Authors:
        still@itserv.ru
        igorptx@gmail.com
*/
#pragma once


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Local libraries */
#include "../../../../../lib/core/payload_engine.h"

#include "../../shoggoth/net.h"

#include "../shoggoth_application.h"



using namespace std;



/*
    Structure of memory file header

    {
        tick,
        nerveCount,
        [
            {
                nerveIdSize
                nerveDataSize
            },
            ...
        ],
        [
            nerveIdData,
            ...
        ],
        [
            nerveData,
            ...
        ]
*/
struct MemoryHeader
{
    unsigned long long int tick = 0;
    unsigned long long int nerveCount = 0;
};



struct MemoryNerveSize
{
    unsigned long long int nerveIdSize = 0;
    unsigned long long int nerveDataSize = 0;
};





class MemoryPayload : public PayloadEngine
{
    private:
        /* Neural net object */
        Mon*            mon                 = NULL;
        /* Neural net object */
        Net* net = NULL;
        /* Resolve hash by layer id for values*/
        std::unordered_map<std::string, uint64_t> oldValuesHashes;

    public:

        /*
            Constructor
        */
        MemoryPayload
        (
            /* The application object */
            ShoggothApplication* aApp,
            /* Payload id */
            std::string aId
        )
        /* Call parent constructor */
        : PayloadEngine(( Application* ) aApp, aId )
        {
            /* Define mon file */
            auto monFile = aApp -> getMonPath( aId + ".json" );

            /* Begin log */
            aApp -> getLog() -> trace( "Memory creating" )
            -> prm( "id", aId )
            -> prm( "log", monFile )
            ;

            /* Create teacher monitor */
            mon = Mon::create( monFile )
            -> setString( Path{ "start", "source" }, aId )
            -> startTimer( Path{ "start", "moment" })
            -> flush()
            ;

            net = aApp -> getNet();
        }



        /*
            Destructor
        */
        ~MemoryPayload()
        {
            waitStop();
            /* Destroy Processor monitor */
            mon -> destroy();
            /* Log report */
            getLog() -> trace( "Memory destroyd" );
        }




        /*
            Creator
        */
        static MemoryPayload* create
        (
            /* The application object */
            ShoggothApplication* aApp,
            /* Payload id */
            std::string aId
        )
        {
            return new MemoryPayload( aApp, aId );
        }



        /*
            Destructor
        */
        void destroy() override
        {
            delete this;
        }



        /*
            Return application object
        */
        ShoggothApplication* getApplication() override
        {
            return ( ShoggothApplication* ) PayloadEngine::getApplication();
        }



        /******************************************************************************
            Payloads methods
        */

        /*
            Read and return header
        */
        MemoryHeader readHeader()
        {
            MemoryHeader result = { 0, 0 };
            auto fd = open( net -> getNetWeightFile().c_str(), O_RDONLY );
            if( fd != -1 )
            {
                ssize_t n = read( fd, &result, sizeof( MemoryHeader ));
                close( fd );
                if( n != sizeof( MemoryHeader ))
                {
                    result = { 0, 0 };
                }
            }
            return result;
        }



        /*
            Write nerve weight in to file
        */
        MemoryPayload* dataFromNetToFile()
        {
            auto fileName = net -> getNetWeightFile();

            if( checkPath( getPath( fileName )))
            {
                auto fd = open
                (
                    fileName.c_str(),
                    O_WRONLY | O_CREAT | O_TRUNC,
                    0644
                );

                if( fd != -1 )
                {
                    net -> lock();
                    /* Write header */
                    MemoryHeader header =
                    {
                        net -> getLearningTick(),
                        (unsigned long long int) net -> getNerveList() -> getCount()
                    };
                    write( fd, &header, sizeof( MemoryHeader ));

                    /* Write size of data frames */
                    net -> getNerveList() -> loop
                    (
                        [ &fd ]
                        ( void *item )
                        {
                            auto nerve = (Nerve*) item;
                            auto nerveId = nerve -> getId();
                            MemoryNerveSize nerveSize =
                            {
                                (unsigned long long int) nerveId.length(),
                                nerve -> calcWeightsBufferSize()
                            };
                            write( fd, &nerveSize, sizeof( MemoryNerveSize ));

                            return false;
                        }
                    );

                    /* Write nerves id */
                    net -> getNerveList() -> loop
                    (
                        [ &fd ]
                        ( void *item )
                        {
                            /* Write nerve id */
                            auto nerve = (Nerve*) item;
                            auto nerveId = nerve -> getId();
                            write( fd, nerveId.c_str(), nerveId.length() );
                            return false;
                        }
                    );

                    /* Write data */
                    net -> getNerveList() -> loop
                    (
                        [ &fd ]
                        ( void *item )
                        {
                            /* Write weights uffer */
                            auto nerve = (Nerve*) item;
                            auto buffer = nerve -> getWeights();
                            auto size = nerve -> calcWeightsBufferSize();
                            write( fd, buffer,size );

                            return false;
                        }
                    );

                    close( fd );

                    net -> unlock();
                }
            }
            else
            {
                setResult( "path-not-exists" );
            }

            return this;
        }



        /*
            Read and fill net nerve weights
        */
        MemoryPayload* dataFromFileToNet()
        {
            /* Open file for reading */
            int fd = open( net -> getNetWeightFile().c_str(), O_RDONLY );

            /* If file is not open then return */
            if (fd != -1)
            {
                /* Read entire file into buffer */
                lseek(fd, 0, SEEK_SET);

                off_t fileSize = lseek(fd, 0, SEEK_END);
                lseek(fd, 0, SEEK_SET);

                std::vector<uint8_t> fileBuffer( fileSize );
                ssize_t n = read( fd, fileBuffer.data(), fileSize);
                close(fd);

                if( n == fileSize )
                {
                    /* Shift to after headers */

                    /* Parse buffer */
                    uint8_t* ptr = fileBuffer.data();
                    MemoryHeader* header = ( MemoryHeader* )ptr;
                    ptr += sizeof( MemoryHeader );

                    /* Lock the net */
                    net -> lock();

                    /* Read structure of nerves */
                    std::vector <MemoryNerveSize> vec( header -> nerveCount );

                    memcpy
                    (
                        vec.data(),
                        ptr,
                        header -> nerveCount * sizeof( MemoryNerveSize )
                    );

                    auto pushCount = 0;

                    std::vector <Nerve*> nerves( header -> nerveCount );

                    net -> lock();

                    for( unsigned long long i = 0; i < header -> nerveCount; i++ )
                    {
                        /* Get nerve id */
                        std::string nerveId( ( char* )ptr, vec[ i ].nerveIdSize );
                        ptr += vec[ i ].nerveIdSize;

                        /* Get nerve */
                        auto nerve = net -> getNerveList() -> byId( nerveId );
                        if
                        (
                            nerve != nullptr &&
                            nerve -> calcWeightsBufferSize() == vec[ i ].nerveDataSize
                        )
                        {
                            nerves[ i ] = nerve;
                            pushCount ++;
                        }
                    }

                    if( pushCount == net -> getNerveList() -> getCount() )
                    {
                        for( unsigned long long int i = 0; i < header -> nerveCount; i++ )
                        {
                            nerves[ i ] -> readFromBuffer
                            (
                                ptr,
                                vec[ i ].nerveDataSize
                            );
                            ptr += vec[ i ].nerveDataSize;
                        }
                    }

                    /* Unlock the net */
                    net -> unlock();
                }
            }
            return this;
        }



        /*
            Fill net neurons new weights
        */
        MemoryPayload* fillNet();


        /******************************************************************************
            Setters and getters
        */

        /*
            Get application monitor object
        */
        Mon* getMon()
        {
            return mon;
        }



        /******************************************************************************
            Payload events
        */

        /*
            Main Memory loop event
        */
        void onEngineLoop( bool ) override;
};
