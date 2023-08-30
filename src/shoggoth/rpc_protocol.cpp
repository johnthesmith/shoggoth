#include "rpc_protocol.h"

/*
    Convert command to string
*/
std::string cmdToStr
(
    Command a
)
{
    switch( a )
    {
        case CMD_GET_NET        : return "GET_NET";
        case CMD_GET_SYNC       : return "GET_SYNC";
        case CMD_WRITE_LAYER    : return "WRITE_LAYER";
        case CMD_READ_LAYER     : return "READ_LAYER";
    }
    return "UNKNOWN";
}

