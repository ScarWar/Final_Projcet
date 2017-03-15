#include <stdlib.h>
#include "KDArray.h"

typedef struct kd_array_t {
    int dim;
    size_t size;
    int **mat;
    SPPoint **arr;
} KDArray;

typedef struct double_tuple_t {
    double val;
    int index;
} DoubleTuple;

KDArray *init(SPPoint **arr, size_t size) {
    if (arr == NULL || size < 0) return NULL; // TODO error message

    KDArray *kdArray;
    if (!(kdArray = malloc(sizeof(KDArray))))
        return NULL; // TODO error message

    kdArray->dim = spPointGetDimension(arr[0]);
    kdArray->size = size;

    if (!(kdArray->mat = malloc(kdArray->dim * sizeof(*kdArray->mat))))
        return NULL; // TODO error message
    for (int k = 0; k < kdArray->dim; ++k) {
        if (!(kdArray->mat[k] = malloc(kdArray->size * sizeof(kdArray->mat))))
            return NULL; // TODO error message
    }
    if (!(kdArray->arr = malloc(kdArray->size * sizeof(SPPoint *)))) {
        return NULL; // TODO error message;
    }

    // Inserting data
    // Copying points
    for (int l = 0; l < kdArray->size; ++l) {
        kdArray->arr[l] = spPointCopy(arr[l]);
    }


    // Filling the matrix with sorted indexes
    DoubleTuple *tmp = malloc(size * sizeof(DoubleTuple));
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

int freeKDArray(KDArray *kdArray) { // TODO add end cases
    if (kdArray == NULL) return 0;

    // Free point array, including each point
    for (int i = 0; i < kdArray->size; ++i)
        spPointDestroy(kdArray->arr[i]);
    free(kdArray->arr);

    // Free all entries of the matrix and the matrix
    for (int i = 0; i < kdArray->dim; ++i)
        free(kdArray->mat[i]);
    free(kdArray->mat);
    free(kdArray);

    return 1;
}

KDArray **Split(KDArray *kdArray, int coor) {
    if (kdArray == NULL || !(0 <= coor && coor <= kdArray->dim - 1))
        return NULL; // TODO error message

    size_t mid = kdArray->size & 1 ? 1 + kdArray->size >> 1 : kdArray->size >> 1;
    KDArray *kdLeft, *kdRight;
    if (!(kdLeft = malloc(sizeof(KDArray))))
        return NULL; // TODO error message

    if (!(kdRight = malloc(sizeof(KDArray))))
        return NULL; // TODO error message

    // Create a characteristic vector of the left half plane
    int *x = malloc(kdArray->size * sizeof(int));

    for (int j = 0; j < kdArray->size; ++j) {
        x[j] = 1;
    }

    // Create a map for indexes of each half plane
    int *map1 = malloc(kdArray->size * sizeof(int));
    int *map2 = malloc(kdArray->size * sizeof(int));

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

    if (!(kdLeft->mat = malloc(kdLeft->dim * sizeof(*kdLeft->mat))))
        return NULL; // TODO error message
    for (int k = 0; k < kdArray->dim; ++k) {
        if (!(kdLeft->mat[k] = malloc(kdLeft->size * sizeof(kdLeft->mat))))
            return NULL; // TODO error message
    }

    if (!(kdRight->mat = malloc(kdRight->dim * sizeof(*kdRight->mat))))
        return NULL; // TODO error message
    for (int k = 0; k < kdArray->dim; ++k) {
        if (!(kdRight->mat[k] = malloc(kdRight->size * sizeof(kdRight->mat))))
            return NULL; // TODO error message
    }

    if (!(kdLeft->arr = malloc(kdLeft->size * sizeof(SPPoint *))))
        return NULL; // TODO error message

    if (!(kdRight->arr = malloc((kdRight->size * sizeof(SPPoint *)))))
        return NULL; // TODO error message

    for (int i = 0; i < kdArray->dim; ++i) {
        index1 = 0, index2 = 0;
        for (int j = 0; j < kdArray->size; ++j) {
            int index = kdArray->mat[i][j];
            if (x[index] == 0) {
                kdLeft->mat[i][index1++] = map1[index]; // TODO does the map needed?
            } else {
                kdRight->mat[i][index2++] = map2[index]; // TODO does the map needed?
            }
        }
    }



    for (size_t i = 0; i < kdArray->size; ++i) {
        if(map1[i] != -1)
            kdLeft->arr[map1[i]] = spPointCopy(kdArray->arr[i]);
        else
            kdRight->arr[map2[i]] = spPointCopy(kdArray->arr[i]);
    }

    KDArray **pArray;
    if (!(pArray = malloc(2 * sizeof(KDArray *)))) {
        return NULL; // TODO error message
    }
    pArray[0] = kdLeft;
    pArray[1] = kdRight;

    free(x);
    free(map1);
    free(map2);
//    freeKDArray(kdArray);

    return pArray;
}

void sortByCoor(DoubleTuple *trgt, SPPoint **arr, size_t size, int i) {
    if (arr == NULL || size < 0 || i < 0) {
        // TODO error message
        return;
    }
    for (int j = 0; j < size; ++j) {
        trgt[j].val = spPointGetAxisCoor(arr[j], i);
        trgt[j].index = j;
    }
    qsort(trgt, size, sizeof(DoubleTuple), cmpFunc);
}

int cmpFunc(const void *a, const void *b) {
    const DoubleTuple *da = (const DoubleTuple *) a;
    const DoubleTuple *db = (const DoubleTuple *) b;
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
