#include <cstdio>
#include <cstdlib>
#include <opencv2/core/mat.hpp>
#include "SPImageProc.h"

#include "main_aux.h"

extern "C" {
}

#define FEATURE_FILE_TYPE ".feats"

#define ERR_MSG_CREATE_FILE "Unable to create file"
#define ERR_MSG_OPEN_FILE "Unable to open file"
#define ERR_MSG_WRITE_FILE "Error while writing to file"
#define ERR_MSG_READ_FILE "Error while reading from file"
#define NUM_OF_IMAGES_ERROR "Number of images couldn't be resolved"
#define NUM_OF_FEATS_ERROR "Number of features couldn't be resolved"
#define IMAGE_PATH_ERROR "Image path couldn't be resolved"
#define IMAGE_FEATURES_ERR "Couldn't extract features from image"
#define ERR_MSG_PATH_CREATION "Error while creating a string"
#define PCA_DIM_ERROR_MSG "PCA dimension couldn't be resolved"
#define MINIMAL_GUI_ERROR "Minimal GUI mode couldn't be resolved"
#define EXTRACTION_MODE_ERROR "Extraction mode couldn't be resolved"

int createFeaturesFile(char *path, SPPoint **features, int dim, int index, int numOfFeatures) {
    FILE *file;
    double val;
    // Create new file for storing features
    file = fopen(path, "w");
    if (file == NULL) {
        spLoggerPrintError(ERR_MSG_CREATE_FILE, __FILE__, __func__, __LINE__);
        return 0;
    }
    // Write image index number
    if (!fwrite(&index, sizeof(int), 1, file)) {
        spLoggerPrintError(ERR_MSG_WRITE_FILE, __FILE__, __func__, __LINE__);
        fclose(file);
        return 0;
    }
    // Write dim number
    if (!fwrite(&dim, sizeof(int), 1, file)) {
        spLoggerPrintError(ERR_MSG_WRITE_FILE, __FILE__, __func__, __LINE__);
        fclose(file);
        return 0;
    }
    // Write number of features
    if (!fwrite(&numOfFeatures, sizeof(int), 1, file)) {
        spLoggerPrintError(ERR_MSG_WRITE_FILE, __FILE__, __func__, __LINE__);
        fclose(file);
        return 0;
    }
    // Write points
    for (int i = 0; i < numOfFeatures; ++i) {
        for (int j = 0; j < dim; ++j) {
            val = spPointGetAxisCoor(features[i], j);
            if (!fwrite(&val, sizeof(double), 1, file)) {
                spLoggerPrintError(ERR_MSG_WRITE_FILE, __FILE__, __func__, __LINE__);
                fclose(file);
                return 0;
            }
        }
    }
    fclose(file);
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
        spLoggerPrintError(ERR_MSG_OPEN_FILE, __FILE__, __func__, __LINE__);
        return NULL;
    }
    // Read index number
    if (!fread(&index, sizeof(int), 1, file)) {
        fclose(file);
        spLoggerPrintError(ERR_MSG_READ_FILE, __FILE__, __func__, __LINE__);
        return NULL;
    }
    // Read dim number
    if (!fread(&dim, sizeof(int), 1, file)) {
        fclose(file);
        spLoggerPrintError(ERR_MSG_READ_FILE, __FILE__, __func__, __LINE__);
        return NULL;
    }
    // Read number of features
    if (!fread(&numOfFeatures, sizeof(int), 1, file)) {
        fclose(file);
        spLoggerPrintError(ERR_MSG_READ_FILE, __FILE__, __func__, __LINE__);
        return NULL;
    }
    features = (SPPoint **) malloc(numOfFeatures * sizeof(SPPoint *));
    if (features == NULL) {
        fclose(file);
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return NULL;
    }
    data = (double *) malloc(dim * sizeof(double));
    if (data == NULL) {
        fclose(file);
        free(features);
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return NULL;
    }
    for (int i = 0; i < numOfFeatures; ++i) {
//        printf("Point %d -", i);
        for (int j = 0; j < dim; ++j) {
            if (!fread(&val, sizeof(double), 1, file)) {
                fclose(file);
                for (int k = 0; k < i; ++k) {
                    spPointDestroy(features[k]);
                }
                free(features);
                free(data);
                spLoggerPrintError(ERR_MSG_READ_FILE, __FILE__, __func__, __LINE__);
                return NULL;
            }
            data[j] = val;
//            printf(" %4.4lf ",val);
        }
        features[i] = spPointCreate(data, dim, index);
        if (!features[i]) {
            fclose(file);
            for (int j = 0; j < i; ++j) {
                spPointDestroy(features[i]);
            }
            free(features);
            free(data);
            return NULL;
        }
//        printf("\n");
    }
    free(data);
    *nFeatures = numOfFeatures;
    fclose(file);
    return features;
}

int extractFromImages(SPConfig config) {
    int dim, numOfImages, /*numOfFeatures,*/ nFeatures;
    char path[MAX_BUFFER_SIZE];
    SPPoint **imageFeatures;
    SP_CONFIG_MSG msg;
    sp::ImageProc imageProc(config);

    numOfImages = spConfigGetNumOfImages(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
        return 0;
    }

    dim = spConfigGetPCADim(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(PCA_DIM_ERROR_MSG, __FILE__, __func__, __LINE__);
        return 0;
    }

    for (int i = 0; i < numOfImages; ++i) {
        if (spConfigGetImagePath(path, config, i) != SP_CONFIG_SUCCESS) {
            spLoggerPrintError(IMAGE_PATH_ERROR, __FILE__, __func__, __LINE__);
            break;
        }

        imageFeatures = imageProc.getImageFeatures(path, i, &nFeatures);
        if (imageFeatures == NULL) {
            spLoggerPrintError(IMAGE_FEATURES_ERR, __FILE__, __func__, __LINE__);
            break;
        }

        if (spConfigGetImageFeaturesPath(path, config, i) != SP_CONFIG_SUCCESS) {
            free(imageFeatures);
            spLoggerPrintError(IMAGE_PATH_ERROR, __FILE__, __func__, __LINE__);
            break;
        }
        if (sprintf(path, "%s%s", path, FEATURE_FILE_TYPE) < 0) {
            free(imageFeatures);
            spLoggerPrintError(ERR_MSG_PATH_CREATION, __FILE__, __func__, __LINE__);
            break;
        }
        if (!createFeaturesFile(path, imageFeatures, dim, i, nFeatures)) {
            free(imageFeatures);
            break;
        }
        free(imageFeatures);
    }
    return 1;
}

SPPoint **extractFromFile(SPConfig config, int *totalNumOfFeatures) {
    int numOfImages, j = 0, nFeatures;
    int *numOfFeatures;
    char path[MAX_BUFFER_SIZE];
    SP_CONFIG_MSG msg;
    SPPoint ***tmpFeatures = NULL, **tmpImageFeatures = NULL, **features = NULL;

    if (!totalNumOfFeatures) {
        spLoggerPrintError(ERR_MSG_NULL_POINTER, __FILE__, __func__, __LINE__);
        return NULL;
    }
    numOfImages = spConfigGetNumOfImages(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
        return NULL;
    }

    numOfFeatures = (int *) malloc(numOfImages * sizeof(int));
    if (!numOfFeatures) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return NULL;
    }

    tmpFeatures = (SPPoint ***) malloc(numOfImages * sizeof(SPPoint **));
    if (!tmpFeatures) {
        free(numOfFeatures);
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return NULL;
    }

    for (int i = 0; i < numOfImages; ++i) {
        spConfigGetImageFeaturesPath(path, config, i);
        if (sprintf(path, "%s%s", path, FEATURE_FILE_TYPE) < 0) {
            spLoggerPrintError(ERR_MSG_PATH_CREATION, __FILE__, __func__, __LINE__);
            return NULL;
        }
        tmpImageFeatures = readFeaturesFile(path, &nFeatures);
        if (!tmpImageFeatures) {
            free(numOfFeatures);
            free(tmpFeatures);
            return NULL;
        }
        *totalNumOfFeatures += nFeatures;
        numOfFeatures[i] = nFeatures;
        tmpFeatures[i] = tmpImageFeatures;
    }
    features = (SPPoint **) malloc(*totalNumOfFeatures * sizeof(SPPoint *));
    if (!features) {
        free(numOfFeatures);
        free(tmpFeatures);
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return NULL;
    }
    for (int i = 0; i < numOfImages; ++i) {
        for (int k = 0; k < numOfFeatures[i]; ++k) {
            features[j++] = tmpFeatures[i][k];
        }
    }
    free(numOfFeatures);
    free(tmpFeatures);
    return features;
}

KDTree *extractKDTree(SPConfig config) {
    int n = 0;
    SP_CONFIG_MSG msg;
    SplitMethod splitMethod;
    SPPoint **features;

    splitMethod = spConfigGetKDTreeSplitMethod(config);
    // If in extraction mode than extract features from images
    if (spConfigIsExtractionMode(config, &msg)) {
        if (!extractFromImages(config)) {
            return 0;
        }
    } else if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(EXTRACTION_MODE_ERROR, __FILE__, __func__, __LINE__);
        return 0;
    }
    // get features from data
    features = extractFromFile(config, &n);
    if (!features) {
        return 0;
    }

    // Create KDTree using KDArray
    KDArray *kdArray = init(features, (size_t) n);
    if (!kdArray) return 0;
    KDTree *kdTree = createKDTree(kdArray, splitMethod);
    if (!kdTree) return 0;
    return kdTree;
}

int searchSimilarImages(SPConfig config, char *queryPath, KDTree *kdTree) {
    int spKNN, numOfImages, numOfSimilarImages, numOfFeatures;
    int *imageRank;
    SPPoint **queryFeatures;
    SP_CONFIG_MSG msg;
    SPBPQueue *knnQueue;
    BPQueueElement qElement;

    spKNN = spConfigGetKNN(config);

    sp::ImageProc imageProc(config);

    numOfImages = spConfigGetNumOfImages(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
        return 0;
    }

    queryFeatures = imageProc.getImageFeatures(queryPath, -1, &numOfFeatures);
    if (!queryFeatures) {
        spLoggerPrintError(IMAGE_FEATURES_ERR, __FILE__, __func__, __LINE__);
        return 0;
    }

    imageRank = (int *) malloc(numOfImages * sizeof(int));
    if (!imageRank) {
        for (int i = 0; i < numOfFeatures; ++i)
            spPointDestroy(queryFeatures[i]);
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return 0;
    }


    for (int i = 0; i < numOfImages; ++i)
        imageRank[i] = 0;
    for (int i = 0; i < numOfFeatures; ++i) {
        knnQueue = kNearestNeighbors(kdTree, queryFeatures[i], spKNN);
        if (!knnQueue) {
            for (int j = 0; j < numOfFeatures; ++i)
                spPointDestroy(queryFeatures[i]);
            free(imageRank);
            return 0;
        }
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
    for (int i = 0; i < numOfFeatures; ++i)
        spPointDestroy(queryFeatures[i]);

    // Find top numOfSimilarImages indexes
    numOfSimilarImages = spConfigGetNumOfSimilarImages(config);
    // If the number of images is smaller than numOfSimilarImages
    // set numOfSimilarImages to be the number of images and display
    // all of them in order
    if (numOfImages < numOfSimilarImages) numOfSimilarImages = numOfImages;

    int *indexArray = (int *) malloc(numOfSimilarImages * sizeof(int));
    if (!indexArray) {
        free(imageRank);
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return 0;
    }
    for (int i = 0; i < numOfSimilarImages; ++i)
        indexArray[i] = -1;

    char continueFlag;
    for (int j = 0; j < numOfSimilarImages; ++j) {
        int maxIndex = -1, max = -1;
        for (int i = 0; i < numOfImages; ++i) {
            continueFlag = 0;
            // Check if index was already selected
            for (int l = 0; l < j; ++l)
                if (indexArray[l] == i) {
                    continueFlag = 1;
                    break;
                }
            // If it already was selected skip to next index
            if (continueFlag) continue;
            // If better rank image found update maxIndex
            if (max < imageRank[i]) {
                max = imageRank[i];
                maxIndex = i;
            }
        }
        indexArray[j] = maxIndex;
    }
    free(imageRank);

    // Display result
    char path[MAX_BUFFER_SIZE];
    bool minimalGUI = spConfigMinimalGui(config, &msg);
    if (msg != SP_CONFIG_SUCCESS) {
        free(indexArray);
        spLoggerPrintError(MINIMAL_GUI_ERROR, __FILE__, __func__, __LINE__);
        return 0;
    }
    if (!minimalGUI)
        printf("Best candidates for - %s - are:\n", queryPath);
    for (int k = 0; k < numOfSimilarImages; ++k) {
        if (spConfigGetImagePath(path, config, indexArray[k]) != SP_CONFIG_SUCCESS) {
            return 0;
        }
        if (minimalGUI) {
            char s[MAX_BUFFER_SIZE];
            printf("Press any key to show next image\n");
            scanf("%s", s);
            imageProc.showImage(path);
        } else
            printf("%s\n", path);
    }
    free(indexArray);
    return 1;
}
