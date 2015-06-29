cmake_minimum_required(VERSION 2.8)
INCLUDE(${CMAKE_SOURCE_DIR}/../csi-build-scripts/CMakeDependencies.txt)

if(WIN32)
    SET (PGSQLSRC "C:/Program Files/PostgreSQL/9.3")
    include_directories(${PGSQLSRC}/include)
    link_directories(${CMAKE_SOURCE_DIR}/lib/$(Platform))
    link_directories("C:/Program Files/PostgreSQL/9.3/lib")
    set(EXT_LIBS postgres_asio libpq libeay32 ssleay32 Ws2_32) 
endif()    

if(__LINUX__)
     set(EXT_LIBS postgres_asio pq boost_log_setup boost_log boost_date_time boost_timer boost_thread boost_system boost_program_options boost_filesystem boost_regex boost_chrono curl cares idn ldap lber ssl crypto rt pthread z rtmp c)
endif()

add_subdirectory(postgres_asio)
add_subdirectory(samples)