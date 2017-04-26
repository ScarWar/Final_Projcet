cmake_minimum_required(VERSION 3.6)
project(Final_Projcet)

set(CMAKE_C_STANDARD 99)

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
        SPConfig.c)
add_executable(Final_Projcet ${SOURCE_FILES})