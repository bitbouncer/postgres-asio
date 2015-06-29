SET(LIB_SRCS
    postgres_asio.h
    postgres_asio.cpp
	)
add_library (postgres_asio STATIC ${LIB_SRCS})

