include_directories(
	${PROJECT_SOURCE_DIR}/sys
	${PROJECT_SOURCE_DIR}/tinyxml
	${PROJECT_SOURCE_DIR}/memory
)


set(LIB_HEADERS
	cmd_serialize.h
	dealcmddelegate.h
	epollpool.h
	iocppool.h
	mcenter.h
	net.h
	netlib.h
	stream.h
	socket.h
	usercommand.h
	cmd_serialize.h
	nodecenter.h
	selectpool.h
	nullcommand.h
)
set(LIB_SRCS 
	epollpool.cpp
	iocppool.cpp
	mcenter.cpp
	net.cpp
	netlib.cpp
	nodecenter.cpp
	socket.cpp
	selectpool.cpp
)

add_library(network ${LIB_SRCS} ${LIB_HEADERS})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set_target_properties(network PROPERTIES OUTPUT_NAME "network")
