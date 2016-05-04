cmake_minimum_required(VERSION 2.8.1)
cmake_policy(SET CMP0015 NEW)

include_directories(${CSI_INCLUDE_PATH} ${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

if(WIN32)
	if (NOT DEFINED PGSQLSRC)
		SET (PGSQLSRC "C:/Program Files/PostgreSQL/9.5")
	endif()
    include_directories(${PGSQLSRC}/include)
	link_directories(${PGSQLSRC}/lib ${Boost_LIBRARY_DIRS}/$(Platform)/lib)
	set(EXT_LIBS postgres_asio libpq libeay32 ssleay32 Ws2_32) 
else()    
	link_directories(${Boost_LIBRARY_DIRS})
    set(EXT_LIBS postgres_asio boost_log boost_thread boost_system pthread pq)
endif()

add_subdirectory(postgres_asio)

if(__BUILD_EXAMPLES__)
add_subdirectory(samples)
endif()



