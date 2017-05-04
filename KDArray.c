#include <stdlib.h>
#include "KDArray.h"
#include "SPLogger.h"


struct kd_array_t {
    int dim;
    size_t size;
    int **mat;
    SPPoint **arr;
};

struct tuple_t {
    double val;
    int index;
};

KDArray *init(SPPoint **arr, size_t size) {
    if (arr == NULL) {
        spLoggerPrintError(ERR_MSG_NULL_POINTER, __FILE__, __func__, __LINE__);
        return NULL;
    }
    KDArray *kdArray;
    if (!(kdArray = malloc(sizeof(KDArray)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return NULL;
    }

    kdArray->dim = spPointGetDimension(arr[0]);
    kdArray->size = size;

    kdArray->mat = malloc(kdArray->dim * sizeof(*kdArray->mat));
    if (kdArray->mat == NULL) {
        free(kdArray);
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return NULL;
    }

    for (int k = 0; k < kdArray->dim; ++k) {
        if (!(kdArray->mat[k] = malloc(kdArray->size * sizeof(kdArray->mat)))) {
            for (int i = 0; i < k; ++i) {
                free(kdArray->mat[i]);
            }
            free(kdArray->mat);
            free(kdArray);
            spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
            return NULL;
        }
    }
    if (!(kdArray->arr = malloc(kdArray->size * sizeof(SPPoint *)))) {
        for (int i = 0; i < kdArray->dim; ++i) {
            free(kdArray->mat[i]);
        }
        free(kdArray->mat);
        free(kdArray);
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return NULL;
    }

    // Inserting data
    // Copying points
    for (unsigned int l = 0; l < kdArray->size; ++l) {
        kdArray->arr[l] = spPointCopy(arr[l]);
        if (!kdArray->arr[l]) {
            for (int i = 0; i < kdArray->dim; ++i) {
                free(kdArray->mat[i]);
            }
            free(kdArray->mat);
            free(kdArray);
            for (unsigned int j = 0; j < l; ++j) {
                spPointDestroy(kdArray->arr[j]);
            }
            free(kdArray->arr);
            spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
            return NULL;
        }
    }

    // Filling the matrix with sorted indexes
    Tuple *tmp = malloc(size * sizeof(Tuple));
    if (tmp == NULL) {
        for (int i = 0; i < kdArray->dim; ++i) {
            free(kdArray->mat[i]);
        }
        free(kdArray->mat);
        free(kdArray);
        for (unsigned int j = 0; j < kdArray->size; ++j) {
            spPointDestroy(kdArray->arr[j]);
        }
        free(kdArray->arr);
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        return NULL;
    }
    // insert indexes to mat (using SPPoint element index)
    for (int i = 0; i < kdArray->dim; ++i) {
        sortByCoor(tmp, arr, size, i);
        for (unsigned int j = 0; j < kdArray->size; ++j) {
            kdArray->mat[i][j] = tmp[j].index;
        }
    }
    free(tmp);
    return kdArray;
}

int destroyKDArray(KDArray *kdArray) {
    if (kdArray == NULL) return 0;

    // Free point array, including each point
    for (unsigned int i = 0; i < kdArray->size; ++i) {
        spPointDestroy(kdArray->arr[i]);
    }
    free(kdArray->arr);

    // Free all entries of the matrix and the matrix
    for (int i = 0; i < kdArray->dim; ++i)
        free(kdArray->mat[i]);
    free(kdArray->mat);
    free(kdArray);

    return 1;
}

KDArray **Split(KDArray *kdArray, int coor) {
    KDArray *kdLeft = NULL, *kdRight = NULL;
    int *x = NULL, *map1 = NULL, *map2 = NULL;
    int freeLeftMat = -1, freeLeftArr = 0;
    int freeRightMat = -1, freeRightArr = 0;


    if (kdArray == NULL) {
        spLoggerPrintError(ERR_MSG_NULL_POINTER, __FILE__, __func__, __LINE__);
        return NULL;
    } else if (!(0 <= coor && coor <= kdArray->dim - 1)) {
        spLoggerPrintError(ERR_MSG_INVALID_ARG, __FILE__, __func__, __LINE__);
        return NULL;
    }

    size_t mid = (kdArray->size & 1) + (kdArray->size >> 1);

    if (!(kdLeft = malloc(sizeof(KDArray)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    }

    if (!(kdRight = malloc(sizeof(KDArray)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    }

    // Create a characteristic vector of the left half plane
    if (!(x = malloc(kdArray->size * sizeof(int)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    }

    for (unsigned int j = 0; j < kdArray->size; ++j) {
        x[j] = 1;
    }

    // Create a map for indexes of each half plane
    if (!(map1 = malloc(kdArray->size * sizeof(int)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    } else if (!(map2 = malloc(kdArray->size * sizeof(int)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    }

    // Fill the characteristic vector
    for (unsigned int j = 0; j < mid; ++j) {
        x[kdArray->mat[coor][j]] = 0;
    }

    // Fill maps
    // P1 are left half plane points
    // P2 are right half plane points
    // map1[k] = j if P[k] = P1[j], -1 otherwise
    // map2[k] = j if P[k] = P2[j], -1 otherwise
    int index1 = 0, index2 = 0;
    for (unsigned int k = 0; k < kdArray->size; ++k) {
        if (x[k] == 0) {
            map1[k] = index1++;
            map2[k] = -1;
        } else {
            map1[k] = -1;
            map2[k] = index2++;
        }
    }

    // Creating the left kdArray and the right kdArray w/o sorting
    kdLeft->dim = kdArray->dim;
    kdRight->dim = kdArray->dim;

    kdLeft->size = mid;
    kdRight->size = kdArray->size - mid;

    if (!(kdLeft->mat = malloc(kdLeft->dim * sizeof(*kdLeft->mat)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    }
    for (int k = 0; k < kdArray->dim; ++k) {
        if (!(kdLeft->mat[k] = malloc(kdLeft->size * sizeof(kdLeft->mat)))) {
            spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
            freeLeftMat = k;
            goto freeMemSplit;
        }
    }
    freeLeftMat = kdArray->dim;

    if (!(kdRight->mat = malloc(kdRight->dim * sizeof(*kdRight->mat)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    }
    for (int k = 0; k < kdArray->dim; ++k) {
        if (!(kdRight->mat[k] = malloc(kdRight->size * sizeof(kdRight->mat)))) {
            spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
            freeRightMat = k;
            goto freeMemSplit;
        }
    }
    freeRightMat = kdArray->dim;

    if (!(kdLeft->arr = malloc(kdLeft->size * sizeof(SPPoint *)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    }
    freeLeftArr = 1;

    if (!(kdRight->arr = malloc((kdRight->size * sizeof(SPPoint *))))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    }
    freeRightArr = 1;

    // Fill the left and the right KDArray
    for (int i = 0; i < kdArray->dim; ++i) {
        index1 = 0, index2 = 0;
        for (unsigned int j = 0; j < kdArray->size; ++j) {
            // Put int deceleration outside
            int index = kdArray->mat[i][j];
            if (x[index] == 0) {
                kdLeft->mat[i][index1++] = map1[index];
            } else {
                kdRight->mat[i][index2++] = map2[index];
            }
        }
    }

    KDArray **pArray;
    if (!(pArray = malloc(2 * sizeof(KDArray *)))) {
        spLoggerPrintError(ERR_MSG_ALLOC_FAIL, __FILE__, __func__, __LINE__);
        goto freeMemSplit;
    }

    for (size_t i = 0; i < kdArray->size; ++i) {
        if (map1[i] != -1)
            kdLeft->arr[map1[i]] = spPointCopy(kdArray->arr[i]);
        else
            kdRight->arr[map2[i]] = spPointCopy(kdArray->arr[i]);
    }

    pArray[0] = kdLeft;
    pArray[1] = kdRight;

    free(x);
    free(map1);
    free(map2);
    return pArray;

    // Free temporary memory allocated for the method
    freeMemSplit:
    free(x);
    free(map1);
    free(map2);

    for (int i = 0; i < freeLeftMat; ++i) {
        free(kdLeft->mat[i]);
    }
    if (freeLeftMat >= 0) free(kdLeft->mat);
    if (freeLeftArr) free(kdLeft->arr);
    free(kdLeft);

    for (int i = 0; i < freeRightMat; ++i) {
        free(kdRight->mat[i]);
    }
    if (freeRightMat >= 0) free(kdRight->mat);
    if (freeRightArr) free(kdRight->arr);
    free(kdRight);

    return NULL;
}

void sortByCoor(Tuple *trgt, SPPoint **arr, size_t size, int i) {
    if (arr == NULL) {
        spLoggerPrintError(ERR_MSG_NULL_POINTER, __FILE__, __func__, __LINE__);
        return;
    } else if (size == 0 || !(0 <= i && i <= spPointGetDimension(*arr))) {
        spLoggerPrintError(ERR_MSG_INVALID_ARG, __FILE__, __func__, __LINE__);
        return;
    }
    for (unsigned int j = 0; j < size; ++j) {
        trgt[j].val = spPointGetAxisCoor(arr[j], i);
        trgt[j].index = j;
    }
    qsort(trgt, size, sizeof(Tuple), cmpFunc);
}

int cmpFunc(const void *a, const void *b) {
    const Tuple *da = (const Tuple *) a;
    const Tuple *db = (const Tuple *) b;
    return (da->val > db->val) - (da->val < db->val);
}

int getKDArrayDim(KDArray *kdArray) {
    if (kdArray == NULL) {
        spLoggerPrintError(ERR_MSG_NULL_POINTER, __FILE__, __func__, __LINE__);
        return -1;
    }
    return kdArray->dim;
}

size_t getSize(KDArray *kdArray) {
    if (kdArray == NULL) {
        spLoggerPrintError(ERR_MSG_NULL_POINTER, __FILE__, __func__, __LINE__);
        return 0;
    }
    return kdArray->size;
}

int **getMatrix(KDArray *kdArray) {
    if (kdArray == NULL) {
        spLoggerPrintError(ERR_MSG_NULL_POINTER, __FILE__, __func__, __LINE__);
        return NULL;
    }
    return kdArray->mat;
}

SPPoint **getArr(KDArray *kdArray) {
    if (kdArray == NULL) {
        spLoggerPrintError(ERR_MSG_NULL_POINTER, __FILE__, __func__, __LINE__);
        return NULL;
    }
    return kdArray->arr;
}

double getMedian(KDArray *kdArray, int i) {
    if (kdArray == NULL) {
        spLoggerPrintError(ERR_MSG_NULL_POINTER, __FILE__, __func__, __LINE__);
        return -1;
    }
    size_t mid;
    mid = (kdArray->size & 1) + (kdArray->size >> 1);
    return spPointGetAxisCoor(kdArray->arr[kdArray->mat[i][mid - 1]], i);
}

void destroyKDArrayLeaf(KDArray *kdArray) {
    if (kdArray == NULL) return;

    spPointDestroy(*kdArray->arr);
    free(kdArray->arr);

    // Free all entries of the matrix and the matrix
    for (int i = 0; i < kdArray->dim; ++i)
        free(kdArray->mat[i]);
    free(kdArray->mat);
    free(kdArray);

}
