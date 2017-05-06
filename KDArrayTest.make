CC = gcc
OBJS = kd_array_unit_test.o KDArray.o
EXEC = kd_array_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
kd_array_unit_test.o: $(TESTS_DIR)/kd_array_unit_test.c $(TESTS_DIR)/unit_test_util.h KDArray.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
KDArray.o: KDArray.c KDArray.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)