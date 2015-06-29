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
     set(EXT_LIBS postgres_asio pq ${CSI_HTTP_LIBS})
endif()

add_subdirectory(postgres_asio)
add_subdirectory(samples)