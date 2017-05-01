CC = gcc
CPP = g++
#put all your object files here
OBJS = main.o SPImageProc.o SPPoint.o KDArray.o KDTree.o SPBPriorityQueue.o SPConfig.o SPLogger.o main_aux.o
#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

LDFLAGS = -Wl,-rpath-link,/usr/local/lib/opencv-3.1.0/lib/

CFLAGS=-std=c99

CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

#a rule for building a simple c++ source file
#use g++ -MM SPImageProc.cpp to see dependencies
#a rule for building a simple c source file
#use "gcc -MM SPPoint.c" to see the dependencies


$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp main_aux.h SPLogger.h SPImageProc.h SPConfig.h \
                KDTree.h KDArray.h SPPoint.h SPBPriorityQueue.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
main_aux.o: main_aux.cpp main_aux.h SPLogger.h SPImageProc.h SPConfig.h \
                KDTree.h KDArray.h SPPoint.h SPBPriorityQueue.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPImageProc.o: SPImageProc.cpp SPLogger.h SPImageProc.h SPConfig.h \
                KDTree.h SPPoint.h SPBPriorityQueue.h KDArray.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPPoint.o: SPPoint.c SPPoint.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
KDArray.o: KDArray.c KDArray.h SPPoint.h SPLogger.h
    $(CC) $(C_COMP_FLAG) -c $*.c
KDTree.o: KDTree.c KDTree.h SPPoint.h SPBPriorityQueue.h SPLogger.h \
          KDArray.h
    $(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h
    $(CC) $(C_COMP_FLAG) -c $*.c
SPBPriorityQueue.o SPBPriorityQueue.c SPBPriorityQueue.h SPLogger.h
    $(CC) $(C_COMP_FLAG) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)
