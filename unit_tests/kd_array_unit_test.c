#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../KDArray.h"

#define BASIC_ARRAY_SIZE 5
#define BASIC_ARRAY_DIM 2

//Logger is not defined
static bool basicKDArrayTest() {
	double data[BASIC_ARRAY_SIZE][BASIC_ARRAY_DIM] = {
			{0, 0},
			{1, 1},
			{2, 2},
			{3, -3},
			{4, 4}
	};

	SPPoint *points[BASIC_ARRAY_SIZE];
	for (int i = 0; i < BASIC_ARRAY_SIZE; i++) {
		points[i] = spPointCreate(data[i], BASIC_ARRAY_DIM, i);
        ASSERT_TRUE(points[i] != NULL);
	}

	KDArray *array = init(points, BASIC_ARRAY_SIZE);
    ASSERT_TRUE(array != NULL);
    ASSERT_TRUE(getKDArrayDim(array) == BASIC_ARRAY_DIM);

    for(int i = 0; i < BASIC_ARRAY_DIM; i++) {
        KDArray **splitArray = Split(array, i);
        ASSERT_TRUE(splitArray != NULL);

        KDArray *left = splitArray[0];
        KDArray *right = splitArray[1];

        double maxLeft = INT32_MIN;

        for (int j = 0; j < (int) getSize(left); j++) {
            SPPoint **leftArr = getArr(left);
            double val = spPointGetAxisCoor(leftArr[j], i);
            if (val > maxLeft) maxLeft = val;
        }

        for (int j = 0; j < (int) getSize(right); j++) {
            SPPoint **rightArr = getArr(right);
            ASSERT_TRUE(spPointGetAxisCoor(rightArr[j], i) > maxLeft);
        }

        destroyKDArray(left);
        destroyKDArray(right);
        free(splitArray);
    }
    destroyKDArray(array);
    return true;
}

int main() {
	RUN_TEST(basicKDArrayTest);
	return 0;
}
