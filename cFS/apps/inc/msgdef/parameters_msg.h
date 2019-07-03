/**
 * @file parameters_msg.h
 * @brief parameters message definitions
 */

#ifndef PARAMS_DEFS_H_
#define PARAMS_DEFS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cfe.h"

/**
 * @defgroup PARAMETER_MESSAGES
 * @brief parameter message description
 * @ingroup MESSAGES
 * @{
 */

/**
 * @enum parameters
 * @brief Defines the various parameters
 */
typedef enum {
   PARAM_VAL1
} parameter_e;

/**
 * @struct parameters_t
 * @brief parameter data.
 */
typedef struct __attribute__((__packed__))
{
    uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
    uint8    acid;                           /**< identifier */
    uint8    parameter_name;                 /**< total waypoints. Cannot be greater than max. */
    union{
        double parameter_valueF;
        int32_t parameter_valueI;
        char parameter_valueS[50];
    }parameter_value;
}flightplan_t;


/**@}*/

#endif /* ARDUPILOT_DEFS_H_ */
