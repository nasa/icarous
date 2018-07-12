/**
 * @file logger_table.h
 * @brief Logger app table definition
 */
#ifndef _LOGGER_TABLE_H
#define _LOGGER_TABLE_H
#include <stdint.h>
#include <stdbool.h>
/**
 * @defgroup LOGGER_TABLES
 * Input parameters for logger applications.
 * @ingroup TABLES
 */

/**
 * @struct LoggerTable_t
 * @brief Input parameters for logger application
 * @ingroup LOGGER_TABLES
 */
typedef struct
{
    bool logRecord;       ///< record log if true, playback log if false
    char timeStamp[20];   ///< time stamp of log files used for playback.

}LoggerTable_t;


#endif
