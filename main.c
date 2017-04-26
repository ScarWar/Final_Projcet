#include <stdlib.h>
#include <stdio.h>
#include "SPPoint.h"
#include "KDArray.h"
#include "KDTree.h"


int main() {
    /*--------------- Simple test ---------------*/
    double dataA[2] = {1, 2}, dataB[2] = {123, 70}, dataC[2] = {2, 7}, dataD[2] = {9, 11}, dataE[2] = {3, 4};
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

    KDTree *kdTree = createKDTree(kdArray);
    double dataPoint[2] = {7, 15};
    SPPoint *point = spPointCreate(dataPoint, 2, 19);
    SPBPQueue *q = kNearestNeighbors(kdTree, point);
    BPQueueElement bpQueueElement;
    for (int i = 0; i < 2; ++i) {
        spBPQueuePeek(q, &bpQueueElement);
        printf("%d ", bpQueueElement.index);
        spBPQueueDequeue(q);
    }

    // Freeing memory TODO find mem leaks
    spPointDestroy(point);
    spBPQueueDestroy(q);
    destroyKDTree(kdTree);
}

/*--------------- Methods for future use ---------------*/

#define FEATURE_FILE_TYPE "feats"
#define MAX_BUFFER_SIZE 1024

int createFeaturesFile(const char *dirPath, const char *imageName, SPPoint **features, int dim, int index,
                       int numOfFeatures) {
    char path[MAX_BUFFER_SIZE];
    FILE *file;
    double val;
    // Create full file path
    sprintf(path, "%s%s.%s", dirPath, imageName, FEATURE_FILE_TYPE);
    // Create new file for storing features
    file = fopen(path, "w");
    if (file == NULL) {
        // TODO error message
        return 0;
    }
    // Write image index number
    if (!fwrite(&index, sizeof(int), 1, file)) {
        // TODO error message - handle
        return 0;
    }
    // Write dim number
    if (!fwrite(&dim, sizeof(int), 1, file)) {
        // TODO error message - handle
        return 0;
    }
    // Write number of features
    if (!fwrite(&numOfFeatures, sizeof(int), 1, file)) {
        // TODO error message - handle
        return 0;
    }
    // Write points
    for (int i = 0; i < numOfFeatures; ++i) {
        for (int j = 0; j < dim; ++j) {
            val = spPointGetAxisCoor(features[i], j);
            if (!fwrite(&val, sizeof(double), 1, file)) {
                // TODO error message - handle
                return 0;
            }
        }
    }
    return 1;
}

SPPoint **readFeaturesFile(const char *filePath) {
    FILE *file;
    int numOfFeatures, dim, index;
    double val;
    double *data;
    SPPoint **features;
    file = fopen(filePath, "r");
    if (file == NULL) {
        // TODO error message
        return NULL;
    }
    // Read index number
    if (!fread(&index, sizeof(int), 1, file)) {
        // TODO error message - handle
        return NULL;
    }
    // Read dim number
    if (!fread(&dim, sizeof(int), 1, file)) {
        // TODO error message - handle
        return NULL;
    }
    // Read number of features
    if (!fread(&numOfFeatures, sizeof(int), 1, file)) {
        // TODO error message - handle
        return NULL;
    }
    features = malloc(numOfFeatures * sizeof(SPPoint *));
    if (features == NULL) {
        // TODO error message
        return NULL;
    }
    data = malloc(dim * sizeof(double));
    if (data == NULL) {
        // TODO error message
        free(features);
        return NULL;
    }
    for (int i = 0; i < numOfFeatures; ++i) {
        for (int j = 0; j < dim; ++j) {
            if (!fread(&val, sizeof(double), 1, file)) {
                // TODO error message - handle
                free(features);
                free(data);
                return NULL;
            }
            data[j] = val;
        }
        features[i] = spPointCreate(data, dim, index);
    }
    free(data);
    return features;
}