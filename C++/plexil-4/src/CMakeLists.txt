cmake_minimum_required(VERSION 3.7)
project(PlexilSrc)

set(CMAKE_CXX_STANDARD 11)

# Note: remove -m32 flag to generate binaries for 64 bit architecture.
# Add -m32 flag for generating 32 bit binaries.
set(CMAKE_CXX_FLAGS "-g -fPIC -m32")
set(CMAKE_C_FLAGS "-g -fPIC -m32")

add_definitions(-DHAVE_CONFIG_H -DPIC)

set(PLEXIL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../)

add_subdirectory(third-party/ipc-3.9.1)
add_subdirectory(third-party/pugixml)
add_subdirectory(value)
add_subdirectory(utils)
add_subdirectory(expr)
add_subdirectory(intfc)
add_subdirectory(exec)
add_subdirectory(xml-parser)
add_subdirectory(app-framework)
add_subdirectory(interfaces/Sockets)
add_subdirectory(interfaces/UdpAdapter)
add_subdirectory(interfaces/Launcher)
add_subdirectory(interfaces/GanttListener)
add_subdirectory(interfaces/IpcUtils)
add_subdirectory(interfaces/IpcAdapter)
add_subdirectory(interfaces/CfsAdapter)
add_subdirectory(interfaces/PlanDebugListener)
add_subdirectory(interfaces/LuvListener)
add_subdirectory(universal-exec)
add_subdirectory(wrapper)