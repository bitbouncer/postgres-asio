cmake_minimum_required(VERSION 2.8.1)
cmake_policy(SET CMP0015 NEW)

option(BUILD_SAMPLES "build examples" ON)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS log_setup log thread program_options filesystem system REQUIRED)
set(BOOST_LIBS
        ${Boost_LOG_LIBRARY}
        ${Boost_LOG_SETUP_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        pthread
        rt
        c
        )

if(WIN32)
  if (NOT DEFINED PGSQLSRC)
    SET(PGSQLSRC "C:/Program Files/PostgreSQL/9.5")
  endif ()
  include_directories(${PGSQLSRC}/include ${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIR})
  link_directories(${PGSQLSRC}/lib ${Boost_LIBRARY_DIRS}/$(Platform)/lib)
  set(EXT_LIBS postgres_asio libpq libeay32 ssleay32 Ws2_32)
else ()
  SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
  SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
  include_directories(${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIR})
  link_directories(${Boost_LIBRARY_DIRS})
  set(EXT_LIBS postgres_asio pq ${BOOST_LIBS})

  add_definitions(-D_FILE_OFFSET_BITS=64 -D_REENTRANT -DEXTERNAL_LOCKS -DMULTITHREAD)
  add_definitions(-fPIC)
  add_definitions(-DBOOST_LOG_DYN_LINK)

endif()

SET(LIB_SRCS
        postgres_asio/postgres_asio.h
        postgres_asio/postgres_asio.cpp
        )

add_library(postgres_asio STATIC ${LIB_SRCS})

add_executable(cursor_sample samples/cursor_sample.cpp)
target_link_libraries(cursor_sample ${EXT_LIBS})

add_executable(insert_sample samples/insert_sample.cpp)
target_link_libraries(insert_sample ${EXT_LIBS})

add_executable(query_sample samples/query_sample.cpp)
target_link_libraries(query_sample ${EXT_LIBS})




