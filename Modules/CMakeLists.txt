cmake_minimum_required(VERSION 2.6.4)
project(ICAROUS_MODULES)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_subdirectory(ACCoRD)
add_subdirectory(GeofenceMonitor)
add_subdirectory(TrafficMonitor)
add_subdirectory(TrajectoryPlanners)
add_subdirectory(Utils)
add_subdirectory(Guidance)
add_subdirectory(Cognition)


