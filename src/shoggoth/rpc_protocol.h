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
    CMD_UNKNWON,        /* Unknown command */
    CMD_READ_NET,       /* Server return full Net configuration */
    CMD_WRITE_VALUES,   /* Server receive layer values */
    CMD_READ_VALUES,    /* Server return the layer values data */
    CMD_WRITE_ERRORS,   /* Server receive layer errors */
    CMD_READ_ERRORS,    /* Server return the layer errors data */
    CMD_WRITE_WEIGHTS,  /* Server receive the nerve weights data */
    CMD_READ_WEIGHTS    /* Server return the nerve weights data */
};



/*
    Convert command to string
*/
std::string cmdToStr
(
    Command
);
