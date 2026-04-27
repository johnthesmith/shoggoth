/*
    Shoggoth rpc contract
    It is using in the
        shoggoth rpc server
        shoggoth rpc client
*/

#pragma once



/*
    Shoggoth protocol commands
*/
enum Command
{
    /* Unknown command */
    CMD_UNKNOWN,
    /* Server returns full Net configuration */
    CMD_READ_NET,
    /* Server returns current information about net */
    CMD_READ_NET_INFO,
    /* Commit net*/
    CMD_COMMIT_NET,
    /* Server clone net */
    CMD_CLONE_NET,
    /* Server switch to specified net */
    CMD_SWITCH_NET,
    /* Server send layers with errors and values plan */
    CMD_WRITE_LAYERS,
    /* Server receive layers with errors and values plan */
    CMD_READ_LAYERS,
    /* Request weights for one neuron */
    CMD_REQUEST_WEIGHTS,
    /* Drop layer tick counter */
    CMD_DROP_LAYER_TICK,
    /* Server return the layer statistics */
    CMD_READ_LAYER_STAT,
    /* Server receive  */
    CMD_WRITE_WEIGHTS,
    /* Server return the layer  */
    CMD_READ_WEIGHTS,
    /* Request and return net mode from server */
    CMD_GET_NET_MODE,
    /* Set net mode for server */
    CMD_SET_NET_MODE,
    /* Clients (teacher) sent test result */
    CMD_TEST_RESULT,
    /* Sinchronize layers between client and server 2026-04-22 */
    CMD_SYNC_LAYERS
};



/*
    Convert command to string
*/
inline std::string commandToString
(
    Command a
)
{
    switch( a )
    {
        default:
        case CMD_UNKNOWN        : return "UNKNOWN";
        case CMD_COMMIT_NET     : return "COMMIT_NET";
        case CMD_READ_NET       : return "READ_NET";
        case CMD_READ_NET_INFO  : return "READ_NET_INFO";
        case CMD_CLONE_NET      : return "CLONE_NET";
        case CMD_SWITCH_NET     : return "SWITCH_NET";
        case CMD_WRITE_LAYERS   : return "WRITE_LAYERS";
        case CMD_READ_LAYERS    : return "READ_LAYERS";
        case CMD_REQUEST_WEIGHTS: return "REQUEST_WEIGHTS";
        case CMD_WRITE_WEIGHTS  : return "WRITE_WEIGHTS";
        case CMD_READ_WEIGHTS   : return "READ_WEIGHTS";
        case CMD_DROP_LAYER_TICK: return "DROP_LAYER_TICK";
        case CMD_READ_LAYER_STAT: return "READ_LAYER_STAT";
        case CMD_GET_NET_MODE   : return "GET_NET_MODE";
        case CMD_SET_NET_MODE   : return "SET_NET_MODE";
        case CMD_TEST_RESULT    : return "TEST_RESULT";
        case CMD_SYNC_LAYERS    : return "SYNC_LAYERS";
    }
}



/*
    Convert calculation stage from string
*/
inline Command commandFromString
(
    std::string a
)
{
    if( a == "READ_NET" )            return CMD_READ_NET;
    if( a == "READ_NET_INFO" )       return CMD_READ_NET_INFO;
    if( a == "COMMIT_NET" )          return CMD_COMMIT_NET;
    if( a == "CLONE_NET" )           return CMD_CLONE_NET;
    if( a == "SWITCH_NET" )          return CMD_SWITCH_NET;
    if( a == "WRITE_LAYERS" )        return CMD_WRITE_LAYERS;
    if( a == "READ_LAYERS" )         return CMD_READ_LAYERS;
    if( a == "REQUEST_WEIGHTS" )     return CMD_REQUEST_WEIGHTS;
    if( a == "WRITE_WEIGHTS" )       return CMD_WRITE_WEIGHTS;
    if( a == "READ_WEIGHTS" )        return CMD_READ_WEIGHTS;
    if( a == "DROP_LAYER_TICK" )     return CMD_DROP_LAYER_TICK;
    if( a == "READ_LAYER_STAT" )     return CMD_READ_LAYER_STAT;
    if( a == "GET_NET_MODE" )        return CMD_GET_NET_MODE;
    if( a == "SET_NET_MODE" )        return CMD_SET_NET_MODE;
    if( a == "TEST_RESULT" )         return CMD_TEST_RESULT;
    if( a == "SYNC_LAYERS" )         return CMD_SYNC_LAYERS;

    return CMD_UNKNOWN;
}
