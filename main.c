#include <stdlib.h>
#include <stdio.h>
#include "SPPoint.h"
#include "KDArray.h"
#include "KDTree.h"


int main() {
    /*--------------- Simple test ---------------*/
    double dataA[2] = {1, 2},
            dataB[2] = {123, 70},
            dataC[2] = {2, 7},
            dataD[2] = {9, 11},
            dataE[2] = {3, 4};
    SPPoint *a = spPointCreate(dataA, 2, 0);
    SPPoint *b = spPointCreate(dataB, 2, 1);
    SPPoint *c = spPointCreate(dataC, 2, 2);
    SPPoint *d = spPointCreate(dataD, 2, 3);
    SPPoint *e = spPointCreate(dataE, 2, 4);
    SPPoint **arr = malloc(5 * sizeof(SPPoint *));
    *arr = a;
    *(arr + 1) = b;
    *(arr + 2) = c;
    *(arr + 3) = d;
    *(arr + 4) = e;

    KDArray *kdArray = init(arr, 5);

    for (int i = 0; i < 5; ++i) {
        spPointDestroy(arr[i]);
    }
    free(arr);

    KDTree *kdTree = createKDTree(kdArray, MAX_SPREAD);
    double dataPoint[2] = {7, 15};
    SPPoint *point = spPointCreate(dataPoint, 2, 19);
    SPBPQueue *q = kNearestNeighbors(kdTree, point, 5);
    BPQueueElement bpQueueElement;
    for (int i = 0; i < 2; ++i) {
        spBPQueuePeek(q, &bpQueueElement);
        printf("%d ", bpQueueElement.index);
        spBPQueueDequeue(q);
    }

    // Freeing memory
    spPointDestroy(point);
    spBPQueueDestroy(q);
    destroyKDTree(kdTree);
}