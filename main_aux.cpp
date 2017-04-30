#include <cstdio>
#include <cstdlib>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "SPImageProc.h"

#include "main_aux.h"

extern "C" {
#include "SPLogger.h"
#include "SPConfig.h"
#include "SPPoint.h"
#include "SPBPriorityQueue.h"
#include "KDArray.h"
#include "KDTree.h"
}

#define FEATURE_FILE_TYPE ".feats"
#define MAX_BUFFER_SIZE 1024

int createFeaturesFile(char *path, SPPoint **features, int dim, int index, int numOfFeatures) {
    FILE *file;
    double val;
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

SPPoint **readFeaturesFile(const char *filePath, int *nFeatures) {
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
    features = (SPPoint **) malloc(numOfFeatures * sizeof(SPPoint *));
    if (features == NULL) {
        // TODO error message
        return NULL;
    }
    data = (double *) malloc(dim * sizeof(double));
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
    *nFeatures = numOfFeatures;
    return features;
}

int extractFromImages(SPConfig config) {
    int numOfImages, numOfFeatures;
    int *nFeatures;
    char path[MAX_BUFFER_SIZE];
    SPPoint **imageFeatures;
    SP_CONFIG_MSG msg;
    sp::ImageProc imageProc(config);

    numOfImages = spConfigGetNumOfImages(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        // TODO error message
        return 1;
    }
    numOfFeatures = spConfigGetNumOfFeatures(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        // TODO error message
        return 1;
    }
    nFeatures = (int *) malloc(sizeof(int));
    if (!nFeatures) {
        // TODO error message
        return 1;
    }

    for (int i = 0; i < numOfImages; ++i) {
        if (spConfigGetImagePath(path, config, i) != SP_CONFIG_SUCCESS) {
            // TODO error message
            break;
        }
        // TODO check bug
        imageFeatures = imageProc.getImageFeatures(path, i, nFeatures);
        if (imageFeatures == NULL) {
            // TODO error message
            break;
        }
        if (sprintf(path, "%s%s", path, FEATURE_FILE_TYPE) < 0) {
            free(imageFeatures);
            // TODO error message
            break;
        }
        if (!createFeaturesFile(path, imageFeatures, spPointGetDimension(*imageFeatures), i, *nFeatures)) {
            // TODO error message
        }
        free(imageFeatures);
    }
    free(nFeatures);
    return 0;
}

SPPoint **extractFromFile(SPConfig config, int *totalNumOfFeatures) {
    int numOfImages, j = 0;
    int *numOfFeatures, *nFeatures;
    char path[MAX_BUFFER_SIZE];
    SP_CONFIG_MSG *msg;
    SPPoint ***tmpFeatures, **tmpImageFeatures, **features = NULL;

    if (!totalNumOfFeatures) {
        // TODO error message
        return NULL;
    }
    msg = (SP_CONFIG_MSG *) malloc(sizeof(SP_CONFIG_MSG));
    if (!msg) {
        // TODO error message
        return NULL;
    }

    nFeatures = (int *) malloc(sizeof(int));
    if (!nFeatures) {
        // TODO error message
        return NULL;
    }

    numOfImages = spConfigGetNumOfImages(config, msg);
    if (*msg != SP_CONFIG_SUCCESS) {
        // TODO error message
        return NULL;
    }

    numOfFeatures = (int *) malloc(numOfImages * sizeof(int));
    if (!numOfFeatures) {
        // TODO error message
        return NULL;
    }

    tmpFeatures = (SPPoint ***) malloc(sizeof(SPPoint **));
    if (!tmpFeatures) {
        // TODO error message
        return NULL;
    }

    for (int i = 0; i < numOfImages; ++i) {
        spConfigGetImagePath(path, config, i);
        if (sprintf(path, "%s%s", path, FEATURE_FILE_TYPE) < 0) {
            // TODO error message
            return NULL;
        }
        tmpImageFeatures = readFeaturesFile(path, nFeatures);
        if (!tmpImageFeatures) {
            // TODO error message
            return NULL;
        }
        totalNumOfFeatures += *nFeatures;
        numOfFeatures[i] = *nFeatures;
        tmpFeatures[j++] = tmpImageFeatures;
        free(tmpImageFeatures);
    }
    free(nFeatures);
    features = (SPPoint **) malloc(*totalNumOfFeatures * sizeof(SPPoint *));
    if (!features) {
        // TODO error message
        return NULL;
    }
    j = 0;
    for (int i = 0; i < numOfImages; ++i) {
        for (int k = 0; k < numOfFeatures[i]; ++k) {
            features[j++] = tmpFeatures[i][k];
        }
    }

    return features;
}

KDTree *extractKDTree(SPConfig config) {
    int n;
    SP_CONFIG_MSG msg;
    SplitMethod splitMethod;
    SPPoint **features;

    splitMethod = spConfigGetKDTreeSplitMethod(config);

    // If in extraction mode than extract features from images
    if (spConfigIsExtractionMode(config, &msg)) {
        extractFromImages(config);
    } else if (msg != SP_CONFIG_INVALID_ARGUMENT) {
        // TODO error message
        return 0;
    }
    // get features from data
    features = extractFromFile(config, &n);
    if (!features) {
        // TODO error message
        return 0;
    }

    // Create KDTree using KDArray
    KDArray *kdArray = init(features, (size_t) n);
    if (!kdArray) return 0;
    KDTree *kdTree = createKDTree(kdArray, splitMethod);
    destroyKDArray(kdArray);
    if (!kdTree) return 0;
    return kdTree;
}

int searchSimilarImages(SPConfig config, char *queryPath, KDTree *kdTree) {
    int spKNN, numOfImages, numOfSimilarImages, numOfFeatures;
    int *imageRank;
    SPPoint **features, **queryFeatures;
    SP_CONFIG_MSG msg;
    SplitMethod splitMethod;
    SPBPQueue *knnQueue;
    BPQueueElement qElement;

    spKNN = spConfigGetKNN(config);

    sp::ImageProc imageProc(config);

    numOfImages = spConfigGetNumOfImages(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        // TODO error message
        return 0;
    }

    // TODO check bug
    queryFeatures = imageProc.getImageFeatures(queryPath, -1, &numOfFeatures);
    if (!queryFeatures) {
        // TODO error message
        return 0;
    }

    imageRank = (int *) malloc(numOfImages * sizeof(int));
    if (!imageRank) {
        // TODO error message
        return 0;
    }

    for (int i = 0; i < numOfImages; ++i)
        imageRank[i] = 0;
    for (int i = 0; i < numOfFeatures; ++i) {
        knnQueue = kNearestNeighbors(kdTree, features[i], spKNN);
        // Create an array such that imageRank[i] is the
        // number of times a feature of image i is among
        // the spKNN nearest features with respect to some feature
        while (!spBPQueueIsEmpty(knnQueue)) {
            spBPQueuePeek(knnQueue, &qElement);
            imageRank[qElement.index]++;
            spBPQueueDequeue(knnQueue);
        }
        spBPQueueDestroy(knnQueue);
    }

    numOfSimilarImages = spConfigGetNumOfSimilarImages(config); // TODO create spConfigGetNumOfSimilarImages(config, msg);
    char continueFlag;
    int *indexArray = (int *) malloc(numOfSimilarImages * sizeof(int));
    if (!indexArray) {
        // TODO error message
        return 0;
    }
    for (int i = 0; i < numOfSimilarImages; ++i) {
        indexArray[i] = -1;
    }

    // TODO check the possibility that numOfSimilarImages > numOfImages
    for (int j = 0; j < numOfSimilarImages; ++j) {
        int maxIndex = -1, max = -1;
        continueFlag = 0;
        for (int i = 0; i < numOfImages; ++i) {
            // Check if index was already selected
            for (int l = 0; l < j; ++l)
                if (indexArray[l] == i) {
                    continueFlag = 1;
                    break;
                }
            // If it already was selected skip to next index
            if (continueFlag) break;
            // If better rank image found update maxIndex
            if (max > imageRank[i]) {
                max = imageRank[i];
                maxIndex = i;
            }
        }
        indexArray[j] = maxIndex;
    }

    // Display result
    char path[MAX_BUFFER_SIZE];
    bool minimalGUI = spConfigMinimalGui(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        // TODO error message
        return 0;
    }
    if (!minimalGUI) printf("Best candidates for - %s - are:\n", queryPath);
    for (int k = 0; k < numOfSimilarImages; ++k) {
        if (spConfigGetImagePath(path, config, k) != SP_CONFIG_SUCCESS) {
            return 0;
        }
        if (minimalGUI) {
            scanf("Press any key to show next image\n");
            imageProc.showImage(path);
        } else
            printf("%s\n", path);
    }
    return 1;
}
