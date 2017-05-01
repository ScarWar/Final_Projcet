#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../KDTree.h"

#define BASIC_ARRAY_SIZE 7
#define BASIC_ARRAY_DIM 2

//Logger is not defined
static bool basicKDTreeTest() {
	double data[BASIC_ARRAY_SIZE][BASIC_ARRAY_DIM] = {
			{0, 0},
			{1, 1},
			{2, 2},
			{3, -3},
			{4, 4},
			{4.1, 3.9},
			{4.05, 3.85}
	};

	SPPoint *points[BASIC_ARRAY_SIZE];
	for (int i = 0; i < BASIC_ARRAY_SIZE; i++) {
		points[i] = spPointCreate(data[i], BASIC_ARRAY_DIM, i);
        ASSERT_TRUE(points[i] != NULL);
	}

	KDArray *array = init(points, BASIC_ARRAY_SIZE);
    ASSERT_TRUE(array != NULL);

    KDTree *tree = createKDTree(array, MAX_SPREAD);
    ASSERT_TRUE(tree != NULL);
	SPBPQueue *neighbors = kNearestNeighbors(tree, points[4], 4);
    ASSERT_TRUE(neighbors != NULL);

    BPQueueElement element;
    ASSERT_TRUE(spBPQueuePeek(neighbors, &element) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(element.index == 4);
    spBPQueueDequeue(neighbors);
    ASSERT_TRUE(spBPQueuePeek(neighbors, &element) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(element.index == 5);
    spBPQueueDequeue(neighbors);
    ASSERT_TRUE(spBPQueuePeek(neighbors, &element) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(element.index == 6);
    spBPQueueDequeue(neighbors);
    ASSERT_TRUE(spBPQueuePeek(neighbors, &element) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(element.index == 2);
    spBPQueueDequeue(neighbors);

    //destroyKDArray(array);
    destroyKDTree(tree);
    spBPQueueDestroy(neighbors);
    for (int i = 0; i < BASIC_ARRAY_SIZE; i++) {
        spPointDestroy(points[i]);
    }

    return true;
}

int main() {
	RUN_TEST(basicKDTreeTest);
	return 0;
}
