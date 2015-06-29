project(insert_sample)

SET(SRCS
insert_sample.cpp
)

add_executable(insert_sample ${SRCS})
target_link_libraries(insert_sample ${EXT_LIBS})
