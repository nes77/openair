cmake_minimum_required (VERSION 3.0)
project (openair)

## set up Boost
#if (NOT DEFINED BOOST_ROOT AND
#    NOT DEFINED ENV{BOOST_ROOT} AND
#    NOT DEFINED BOOST_INCLUDEDIR AND
#    NOT DEFINED ENV{BOOST_INCLUDEDIR} AND
#    NOT DEFINED BOOST_LIBRARYDIR AND
#    NOT DEFINED ENV{BOOST_LIBRARYDIR})
#
#    set(BOOST_ROOT "libs/boost")
#
#endif()
#
#if (APPLE OR WIN32)
#    set(Boost_USE_STATIC_LIBS TRUE)
#endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#find_package(Boost 1.55 REQUIRED COMPONENTS thread system
#)

# version number
set (OPENAIR_VERSION_MAJOR 0)
set (OPENAIR_VERSION_MINOR 1)

set (COMP_FLAGS "-g -Werror -Wall -Wno-reorder -std=c++11")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMP_FLAGS}")

configure_file (
    src/libopenair/openair_config.in.hpp
    src/libopenair/openair_config.hpp
    )

include_directories(src ${Boost_INCLUDE_DIRS})

#message("PROJECT_SOURCE_DIR points to " ${PROJECT_SOURCE_DIR})

add_library(openair-core
    SHARED
    src/libopenair/airplane.cpp
    src/libopenair/sqlite3.c
)

add_executable(openair-test 
    src/openair/openair_test.cpp
)

add_executable(openair-server
    src/openair/openair_server.cpp
)

#message("Boost libs: " ${Boost_LIBRARIES})
target_link_libraries(openair-core ${Boost_LIBRARIES} Threads::Threads)
target_link_libraries(openair-test openair-core ${Boost_LIBRARIES})
target_link_libraries(openair-server openair-core ${Boost_LIBRARIES})

enable_testing()

