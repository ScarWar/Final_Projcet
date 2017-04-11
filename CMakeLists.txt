cmake_minimum_required(VERSION 3.6)
project(Final_Projcet)

set(CMAKE_C_STANDARD 99)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

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
        )
add_executable(Final_Projcet ${SOURCE_FILES})