CC = gcc
OBJS = kd_tree_unit_test.o SPConfig.o
EXEC = kd_tree_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
kd_tree_unit_test.o:  $(TESTS_DIR)/kd_tree_unit_test.c \
$(TESTS_DIR)/unit_test_util.h  $(TESTS_DIR)/../KDTree.h \
$(TESTS_DIR)/../SPPoint.h  $(TESTS_DIR)/../SPBPriorityQueue.h \
$(TESTS_DIR)/../SPLogger.h  $(TESTS_DIR)/../KDArray.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
KDTree.o: KDTree.c KDTree.h SPPoint.h SPBPriorityQueue.h SPLogger.h \
KDArray.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)