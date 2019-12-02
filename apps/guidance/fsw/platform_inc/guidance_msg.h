/**
 * @file Guidance_msg.h
 */
#ifndef _GUIDANCE_MSG_H_
#define _GUIDANCE_MSG_H_

/**
 * @enum guidance_mode_e
 * @brief various guidance modes
 */
typedef enum{
    PRIMARY_FLIGHTPLAN,     ///< Follow the primary mission flight plan
    SECONDARY_FLIGHTPLAN,   ///< Follow the secondary flight plan (for detours/reroutes)
    VECTOR,                 ///< Follow the given velocity vector
    POINT2POINT,            ///< Fly to a specific position
    ORBIT,                  ///< Orbit around a given point
    HELIX,                  ///< A helical orbit
    TAKEOFF,                ///< Takeoff mode
    LAND,                   ///< Landing mode
    NOOP                    ///< No operations
}guidance_mode_e;

#endif