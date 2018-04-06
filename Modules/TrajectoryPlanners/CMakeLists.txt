cmake_minimum_required(VERSION 3.5)
project(PathPlanner)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS -m32)

add_subdirectory(RRT)
add_subdirectory(Astar)
add_subdirectory(BSplines)

set(SOURCE_FILES PathPlanner.cpp AstarPlan.cpp GridAstarPlan.cpp RRTPlan.cpp SplinesPlan.cpp Dynamics.cpp CWrapper/TrajectoryPlanner_wrap.cxx)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../ACCoRD/inc
                    ${CMAKE_CURRENT_SOURCE_DIR}/../FlightDataManager
                    ${CMAKE_CURRENT_SOURCE_DIR}/RRT
                    ${CMAKE_CURRENT_SOURCE_DIR}/Astar
                    ${CMAKE_CURRENT_SOURCE_DIR}/BSplines)

link_directories(${LIBRARY_OUTPUT_PATH})

add_library(PathPlanner SHARED ${SOURCE_FILES})

target_link_libraries(PathPlanner RRT ASTAR BSPLINES ACCoRD FlightDataManager)

add_executable(PathTest Test/main.cpp)
target_link_libraries(PathTest PathPlanner)