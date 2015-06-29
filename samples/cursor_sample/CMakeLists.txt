project(cursor_sample)

SET(SRCS
cursor_sample.cpp
)

add_executable(cursor_sample ${SRCS})
target_link_libraries(cursor_sample ${EXT_LIBS})
