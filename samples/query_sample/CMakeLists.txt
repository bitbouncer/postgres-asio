project(query_sample)

SET(SRCS
query_sample.cpp
)

add_executable(query_sample ${SRCS})
target_link_libraries(query_sample ${EXT_LIBS})
