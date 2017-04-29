#include <stdlib.h>
#include "KDArray.h"


typedef struct kd_array_t {
    int dim;
    size_t size;
    int **mat;
    SPPoint **arr;
} KDArray;

typedef struct tuple_t {
    double val;
    int index;
} Tuple;

KDArray *init(SPPoint **arr, size_t size) {
    if (arr == NULL) return NULL; // TODO error message

    KDArray *kdArray;
    if (!(kdArray = malloc(sizeof(KDArray))))
        return NULL; // TODO error message

    kdArray->dim = spPointGetDimension(arr[0]);
    kdArray->size = size;

    if (!(kdArray->mat = malloc(kdArray->dim * sizeof(*kdArray->mat)))) {
        free(kdArray);
        return NULL; // TODO error message
    }

    for (int k = 0; k < kdArray->dim; ++k) {
        if (!(kdArray->mat[k] = malloc(kdArray->size * sizeof(kdArray->mat)))) {
            for (int i = 0; i < k; ++i) {
                free(kdArray->mat[i]);
            }
            free(kdArray->mat);
            free(kdArray);
            return NULL; // TODO error message
        }
    }
    if (!(kdArray->arr = malloc(kdArray->size * sizeof(SPPoint *)))) {
        for (int i = 0; i < kdArray->dim; ++i) {
            free(kdArray->mat[i]);
        }
        free(kdArray->mat);
        free(kdArray);
        return NULL; // TODO error message;
    }

    // Inserting data
    // Copying points
    for (int l = 0; l < kdArray->size; ++l) {
        kdArray->arr[l] = spPointCopy(arr[l]);
    }


    // Filling the matrix with sorted indexes
    Tuple *tmp = malloc(size * sizeof(Tuple));
    // insert indexes to mat (using SPPoint element index)
    for (int i = 0; i < kdArray->dim; ++i) {
        sortByCoor(tmp, arr, size, i);
        for (int j = 0; j < kdArray->size; ++j) {
            kdArray->mat[i][j] = tmp[j].index;
        }
    }
    free(tmp);

    return kdArray;
}

int destroyKDArray(KDArray *kdArray) { // TODO add end cases
    if (kdArray == NULL) return 0;

    // Free point array, including each point
    for (int i = 0; i < kdArray->size; ++i) {
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


    if (kdArray == NULL || !(0 <= coor && coor <= kdArray->dim - 1)) {
        // TODO error message
        return NULL;
    }

    size_t mid = kdArray->size & 1 ? 1 + kdArray->size >> 1 : kdArray->size >> 1;

    if (!(kdLeft = malloc(sizeof(KDArray)))) {
        // TODO error message
        goto freeMemSplit;
    }

    if (!(kdRight = malloc(sizeof(KDArray)))) {
        // TODO error message
        goto freeMemSplit;
    }

    // Create a characteristic vector of the left half plane
    if (!(x = malloc(kdArray->size * sizeof(int)))) {
        // TODO error message
        goto freeMemSplit;
    }

    for (int j = 0; j < kdArray->size; ++j) {
        x[j] = 1;
    }

    // Create a map for indexes of each half plane
    if (!(map1 = malloc(kdArray->size * sizeof(int)))) {
        // TODO error message
        goto freeMemSplit;
    }
    if (!(map2 = malloc(kdArray->size * sizeof(int)))) {
        // TODO error message
        goto freeMemSplit;
    }

    // Fill the characteristic vector
    for (int j = 0; j < mid; ++j) {
        x[kdArray->mat[coor][j]] = 0;
    }

    // Fill maps
    // P1 are left half plane points
    // P2 are right half plane points
    // map1[k] = j if P[k] = P1[j], -1 otherwise
    // map2[k] = j if P[k] = P2[j], -1 otherwise
    int index1 = 0, index2 = 0;
    for (int k = 0; k < kdArray->size; ++k) {
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
        // TODO error message
        goto freeMemSplit;
    }
    for (int k = 0; k < kdArray->dim; ++k) {
        if (!(kdLeft->mat[k] = malloc(kdLeft->size * sizeof(kdLeft->mat)))) {
            // TODO error message
            freeLeftMat = k;
            goto freeMemSplit;
        }
    }
    freeLeftMat = kdArray->dim;

    if (!(kdRight->mat = malloc(kdRight->dim * sizeof(*kdRight->mat)))) {
        // TODO error message
        goto freeMemSplit;
    }
    for (int k = 0; k < kdArray->dim; ++k) {
        if (!(kdRight->mat[k] = malloc(kdRight->size * sizeof(kdRight->mat)))) {
            // TODO error message
            freeRightMat = k;
            goto freeMemSplit;
        }
    }
    freeRightMat = kdArray->dim;

    if (!(kdLeft->arr = malloc(kdLeft->size * sizeof(SPPoint *)))) {
        // TODO error message
        goto freeMemSplit;
    }
    freeLeftArr = 1;

    if (!(kdRight->arr = malloc((kdRight->size * sizeof(SPPoint *))))) {
        // TODO error message
        goto freeMemSplit;

        return NULL; // TODO error message
    }
    freeRightArr = 1;

    // Fill the left and the right KDArray
    for (int i = 0; i < kdArray->dim; ++i) {
        index1 = 0, index2 = 0;
        for (int j = 0; j < kdArray->size; ++j) {
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
        goto freeMemSplit; // TODO error message
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
    if (arr == NULL || size == 0 || !(0 <= i && i <= spPointGetDimension(*arr))) {
        // TODO error message
        return;
    }
    for (int j = 0; j < size; ++j) {
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
    if (kdArray == NULL) return -1; // TODO error message
    return kdArray->dim;
}

size_t getSize(KDArray *kdArray) {
    if (kdArray == NULL) return 0; // TODO error message
    return kdArray->size;
}

int **getMatrix(KDArray *kdArray) {
    if (kdArray == NULL) return NULL; // TODO error message
    return kdArray->mat;
}

SPPoint **getArr(KDArray *kdArray) { // TODO error message
    if (kdArray == NULL) return NULL;
    return kdArray->arr;
}

double getMedian(KDArray *kdArray, int i) {
    if (kdArray == NULL)
        return -1; // TODO error message
    size_t mid = kdArray->size & 1 ? 1 + kdArray->size >> 1 : kdArray->size >> 1;
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
