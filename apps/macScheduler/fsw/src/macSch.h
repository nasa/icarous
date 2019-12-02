/**
 * @file macSch.h
 * @brief function declarations, definitions of macros, datastructures and global variables for the mac sch application
 */
#ifndef _macsch_h_
#define _macsch_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MACSCH_RESERVED_EID              0
#define MACSCH_STARTUP_INF_EID           1
#define MACSCH_COMMAND_ERR_EID           2
#define MACSCH_CONNECTED_TO_AP_EID       3

#define MACSCH_MAJOR_VERSION    1
#define MACSCH_MINOR_VERSION    0

/**
 * @defgroup MACSCH
 * @brief A scheduling application
 * @ingroup APPLICATIONS
 *
 */

/**
 * Entry point for app
 * @brief This is the main entry point for your application that will be used by cFS
 */
void MACSCH_AppMain(void);

/**
 * Initialize app properties
 * @brief Initialize app data: set up pipes, subscribe to messages, read table, initialize data
 */
void MACSCH_AppInit(void);

/**
 * Clean up variables
 */
void MACSCH_AppCleanUp(void);

typedef struct{
   uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
}macScheduler_t;

macScheduler_t FREQ_50;
macScheduler_t FREQ_30;
macScheduler_t FREQ_20;
macScheduler_t FREQ_10;
macScheduler_t FREQ_05;
macScheduler_t FREQ_01;

uint32_t timerId_50;
uint32_t timerId_30;
uint32_t timerId_10;
uint32_t timerId_01;


void timer50_callback(uint32_t timerId);
void timer30_callback(uint32_t timerId);
void timer10_callback(uint32_t timerId);
void timer01_callback(uint32_t timerId);



#endif /* _apInterface_h_ */
