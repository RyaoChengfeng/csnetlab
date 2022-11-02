cmake_minimum_required(VERSION 3.22)
project(csnetlab)

set(CMAKE_CXX_STANDARD 17)

find_package (Threads)

add_executable(lab1 lab1/main.cpp lab1/httpParse.cpp)
target_link_libraries (lab1 ${CMAKE_THREAD_LIBS_INIT})


#SET(CMAKE_C_COMPTLER GCC)
#set(CMAKE_CXX FLAGS -fexec-charset=GBK)
#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lab2/include)
#aux_source_directory(${PROJECT_SOURCE_DIR}/lab2/src SRC_LIST)
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lab2/bin)
#ADD_EXECUTABLE(stop_wait ${SRC_LIST})
#FIND_LIBRARY(NETSIM_LIB libnetsim.a ${PROJECT_SOURCE_DIR}/lab2/lib)
#TARGET_LINK_LIBRARIES(stop_wait  ${NETSIM_LIB})

#Stop Wait
INCLUDE_DIRECTORIES(lab2/include)
aux_source_directory(lab2/src SRC_LIST)
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lab2/bin)
ADD_EXECUTABLE(stop_wait ${SRC_LIST})
FIND_LIBRARY(NETSIM_LIB libnetsim.a lab2/lib)
TARGET_LINK_LIBRARIES(stop_wait  ${NETSIM_LIB})

# GBN
INCLUDE_DIRECTORIES(lab2/include)
aux_source_directory(lab2/GBN GBN_LIST)
add_executable(GBN ${GBN_LIST})
FIND_LIBRARY(NETSIM_LIB libnetsim.a lab2/lib)
TARGET_LINK_LIBRARIES(GBN  ${NETSIM_LIB})

# SR
INCLUDE_DIRECTORIES(lab2/include)
aux_source_directory(lab2/SR SR_LIST)
add_executable(SR ${SR_LIST})
FIND_LIBRARY(NETSIM_LIB libnetsim.a lab2/lib)
TARGET_LINK_LIBRARIES(SR  ${NETSIM_LIB})

# TCP
INCLUDE_DIRECTORIES(lab2/include)
aux_source_directory(lab2/TCP TCP_LIST)
add_executable(TCP ${TCP_LIST})
FIND_LIBRARY(NETSIM_LIB libnetsim.a lab2/lib)
TARGET_LINK_LIBRARIES(TCP  ${NETSIM_LIB})