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
        case CMD_WRITE_VALUES   : return "WRITE_VALUES";
        case CMD_READ_VALUES    : return "READ_VALUES";
        case CMD_WRITE_ERRORS   : return "WRITE_ERRORS";
        case CMD_READ_ERRORS    : return "READ_ERRORS";
        case CMD_WRITE_WEIGHTS  : return "WRITE_WEIGHTS";
        case CMD_READ_WEIGHTS   : return "READ_WEIGHTS";
    }
    return "UNKNOWN";
}

