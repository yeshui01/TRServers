CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(msg_client)

FILE(GLOB MAIN_SRC ${PROJECT_SOURCE_DIR}/*.cpp)

MESSAGE("PROJECT_SOURCE_DIR:${PROJECT_SOURCE_DIR}")
ADD_EXECUTABLE(msg_client ${TIMER_SRC} ${MAIN_SRC} ${COMMON_SRC}
${NETWORK_SRC} ${LOG_SRC} ${MSG_PACKET_SRC})

#链接库文件
TARGET_LINK_LIBRARIES(msg_client liblog4cplus.so)
