/**
 * @file merger_msg.h
 * @brief Defintion of messages used by merger_msg.h
 */

#ifndef ICAROUS_CFS_MERGER_MSG_H
#define ICAROUS_CFS_MERGER_MSG_H

#include <cfe.h>
#include <stdint.h>

#define INTERSECTION_MAXC 10 


/**
 * @struct mergingData_t
 * @brief data to calculate schedule
 */
typedef struct __attribute__((__packed__)){
   CCSDS_PriHdr_t hdr;
   int8_t aircraftID;           /**< Aircraft ID */
   int8_t intersectionID;       /**< Current intersection for which scheduling is required */
   double earlyArrivalTime;    /**< Earliest a/c can reach the intersection (using max speed) */
   double currentArrivalTime;  /**< Current arrival time  (using current speed)*/
   double lateArrivalTime;     /**< Late arrival time (using slowest speed + allowed deviations) */
   int32_t numSchedulesComputed;/**< Number of times scheduler has been run */
   uint8_t zoneStatus;          /**< zone status */
}cMergingData_t;

/**
 * @struct mergingParams_t
 * @brief parameters for merging algorithm
 */
 typedef struct{
   uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
   double maxVehicleSpeed;                           /**< Maximum vehicle speed */
   double minVehicleSpeed;                           /**< Minimum vehicle speed */
   double corridorWidth;                             /**< Corridor width */
   double entryRadius;                               /**< Radius within which maneuvering is essential */
   double coordZone;                                 /**< Radius within which coordination is essential (data exchange) */
   double scheduleZone;                              /**< Radius within which scheduling takes place */
   double minSeparationDistance;                     /**< Minimum separation to maintain from other vehicle. */
   double minSeparationTime;                         /**< Minimum separation time. */
   double maxVehicleTurnRadius;                      /**< Vehicle turn radius */
   double IntersectionLocation[INTERSECTION_MAXC][3]; /**< Intersection locations */
   uint32_t IntersectionID[INTERSECTION_MAXC];        /**< Intersection IDs */
   int    startIntersection;                         /**< Starting intersection */
   double missionSpeed;                              /**< Default mission speed */
 }merger_parameters_t;

#endif //ICAROUS_CFS_MERGER_MSG_H
