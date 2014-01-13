cmake_minimum_required(VERSION 2.6)
project(HELLO)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS_DEBUG "-g -gdwarf-3" CACHE STRING "Flags used by the compiler during debug builds" FORCE)

add_definitions(-std=c++11 -Wall)

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${BOOST_INCLUDE_DIR})
list(APPEND libs ${Boost_LIBRARIES})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})
add_definitions(-DGLFW_NO_GLU)
list(APPEND libs ${GLFW_LIBRARIES} GL)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
list(APPEND libs ${GLEW_LIBRARIES})

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})
add_definitions(${PNG_DEFINITIONS})
list(APPEND libs ${PNG_LIBRARIES})

find_package(LuaJIT REQUIRED)
include_directories(${LUAJIT_INCLUDE_DIR})
list(APPEND libs ${LUAJIT_LIBRARY})

include_directories(src)

file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE tests src/*_gtest.cpp)
list(REMOVE_ITEM sources ${CMAKE_SOURCE_DIR}/src/main.cpp ${tests})

add_library(kube STATIC ${sources})

add_executable(kubeclient src/main.cpp)
target_link_libraries(kubeclient kube ${libs})

enable_testing()
add_subdirectory(gtest-1.7.0)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable(all_gtests ${tests})
target_link_libraries(all_gtests kube ${libs} gtest gtest_main)
add_test(all_gtests all_gtests)

install(TARGETS kubeclient DESTINATION .)
install(DIRECTORY res/ DESTINATION .)
