/**
 * @file plexil_table.h
 * @brief plexil table definition
 */

#ifndef ICAROUS_CFS_PLEXIL_TABLE_H
#define ICAROUS_CFS_PLEXIL_TABLE_H

/**
 * @defgroup PLEXIL_TABLES
 * @brief plexil input tables
 * @ingroup TABLES
 */

/**
 * @struct PLEXILTable_t
 * @brief plexil table parameters
 * @ingroup PLEXIL_TABLES
 */
typedef struct
{
    int argc;         ///< Total arguments passed into plexil executive
    char argv1[50];   ///< Arguments passed into plexil executive
    char argv2[50];   ///< Arguments passed into plexil executive
    char argv3[50];   ///< Arguments passed into plexil executive
    char argv4[50];   ///< Arguments passed into plexil executive
    char argv5[50];   ///< Arguments passed into plexil executive
    char argv6[50];   ///< Arguments passed into plexil executive
    char argv7[50];   ///< Arguments passed into plexil executive

}PLEXILTable_t;


#endif //ICAROUS_CFS_PLEXIL_TABLE_H
