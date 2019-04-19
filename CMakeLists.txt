cmake_minimum_required(VERSION 2.8.11)

project(vib )

 

set(BINDIR  ${CMAKE_SOURCE_DIR}/bin)
set(AUTHOR_NAME "ywkim")

ADD_DEFINITIONS(-Wall )
 

include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB SRCS 
	src/Socket.cpp 
	src/ServerSocket.cpp
	src/object.cpp
	src/parser.cpp
	src/msg.cpp
	src/work.cpp
	src/task.cpp
	src/thread.cpp
	src/sensor.cpp
	src/server.cpp
)

 

add_executable(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread)

install(TARGETS ${PROJECT_NAME} DESTINATION ${BINDIR})

