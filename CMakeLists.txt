cmake_minimum_required(VERSION 2.6)
project(HELLO)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_definitions(-std=c++11 -Wall)

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${BOOST_INCLUDE_DIR})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})
add_definitions(-DGLFW_NO_GLU)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})
add_definitions(${PNG_DEFINITIONS})

find_package(LuaJIT REQUIRED)
include_directories(${LUAJIT_INCLUDE_DIR})

file(GLOB_RECURSE sources src/*.cpp)
include_directories(src)
add_executable(test ${sources})
target_link_libraries(test ${Boost_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${PNG_LIBRARIES} GL ${LUAJIT_LIBRARY})

set(CMAKE_CXX_FLAGS_DEBUG "-g -gdwarf-3" CACHE STRING "Flags used by the compiler during debug builds" FORCE)

install(TARGETS test DESTINATION .)
install(DIRECTORY res/ DESTINATION .)
