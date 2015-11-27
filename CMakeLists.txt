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
if (NOT WIN32)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#find_package(Boost 1.55 REQUIRED COMPONENTS thread system
#)

set (COMP_FLAGS "-g -Werror -Wall -Wno-reorder -std=c++11")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMP_FLAGS}")

endif()

# version number
set (OPENAIR_VERSION_MAJOR 0)
set (OPENAIR_VERSION_MINOR 1)



configure_file (
    src/libopenair/openair_config.in.hpp
    src/libopenair/openair_config.hpp
    )

include_directories(src ${Boost_INCLUDE_DIRS})

#message("PROJECT_SOURCE_DIR points to " ${PROJECT_SOURCE_DIR})

if (NOT WIN32)
add_library(openair-core
        SHARED
        src/libopenair/airplane.cpp
        src/libopenair/parallel.hpp
        src/libopenair/common.hpp
        src/libopenair/sqlite3.c
        src/libopenair/sqlite3.h
        src/libopenair/sqlite3pp.cpp
        src/libopenair/sqlite3pp.hpp
        )

set_source_files_properties(src/libopenair/sqlite3.c PROPERTIES COMPILE_FLAGS -DSQLITE_OMIT_LOAD_EXTENSION)
else()
add_library(openair-core
        STATIC
        src/libopenair/airplane.cpp
        src/libopenair/parallel.hpp
        src/libopenair/common.hpp
        src/libopenair/sqlite3.c
        src/libopenair/sqlite3.h
        src/libopenair/sqlite3pp.cpp
        src/libopenair/sqlite3pp.hpp
        )
endif()

add_executable(openair-test 
    src/openair/openair_test.cpp
)

add_executable(openair-server
    src/openair/openair_server.cpp
)

add_executable(sqlitepp-test

        src/test/sqlitepp_test.cpp)



#message("Boost libs: " ${Boost_LIBRARIES})
if(NOT WIN32)
    target_link_libraries(openair-core ${Boost_LIBRARIES} Threads::Threads)
else()
    target_link_libraries(openair-core ${Boost_LIBRARIES})
endif()
target_link_libraries(openair-test openair-core ${Boost_LIBRARIES})
target_link_libraries(sqlitepp-test openair-core ${Boost_LIBRARIES})
target_link_libraries(openair-server openair-core ${Boost_LIBRARIES})

enable_testing()

