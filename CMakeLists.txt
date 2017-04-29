cmake_minimum_required(VERSION 3.6)
project(Final_Projcet)

find_package(OpenCV REQUIRED)

#set(CMAKE_C_STANDARD 99)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

message(STATUS "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")
message(STATUS "OpenCV_LIBS=${OpenCV_LIBS}")

set(SOURCE_FILES main.c
        KDTree.h
        KDTree.c
        KDArray.h
        KDArray.c
        SPBPriorityQueue.h
        SPBPriorityQueue.c
        SPPoint.h
        SPPoint.c
        SPConfig.h
        SPLogger.c
        SPLogger.h
        SPImageProc.h
        SPConfig.c main_aux.cpp main_aux.h)

add_executable(Final_Projcet ${SOURCE_FILES})
target_link_libraries(Final_Projcet ${OpenCV_LIBS})
set_property(TARGET Final_Projcet PROPERTY C_STANDARD 99)