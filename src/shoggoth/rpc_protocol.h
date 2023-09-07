#pragma once

#include <string>   /*for cmdToStr */



/*
    Shoggoth RPC protocol constants
*/
enum CalcDirection
{
    CALC_ALL,
    CALC_FORWARD,     /* Calculation forward */
    CALC_BACKWARD     /* Calculation backweard */
};



/*
*/
enum Command
{
    CMD_UNKNWON,        /**/
    CMD_GET_NET,        /* Server return full Net configuration */
    CMD_GET_SYNC,       /* Server return sync object */
    CMD_WRITE_LAYER,    /* Server receive layer value and error data */
    CMD_READ_LAYER,     /* Server return the layer value and error data */
    CMD_READ_NERVE,      /* Server return the nerve weights data */
    CMD_WRITE_NERVE     /* Server receive the nerve weights data */
};



/*
    Convert command to string
*/
std::string cmdToStr
(
    Command
);
