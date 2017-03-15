#include <wchar.h>
#include "SPPoint.h"

#ifndef KDTREE_KDARRAY_H
#define KDTREE_KDARRAY_H


typedef struct kd_array_t KDArray;

typedef struct double_tuple_t DoubleTuple;

/**
 *
 * @param arr
 * @param size
 * @return
 */
KDArray *init(SPPoint **arr, size_t size);

/**
 * frees memory of kdArray - frees memory from kdArray->mat,
 * frees all points from kdArray->arr
 * @param kdArray
 * @return if successful return 1, otherwise 0
 */
int freeKDArray(KDArray *kdArray);


/**
 *
 * @param kdArray
 * @param coor
 * @return
 */
KDArray **Split(KDArray *kdArray, int coor);


/**
 * Insert indexes which are sorted according to the i-th coordinate
 * of the points, The value of the points is obtained from 'arr'
 * @param trgt - The tuple array to be sorted
 * @param arr - An array of points
 * @param size - The number of points in the array
 * @param i - The coordinate for which we sort the array
 */
void sortByCoor(DoubleTuple *trgt, SPPoint **arr, size_t size, int i);

/**
 * a compare function for DoubleTuple type
 * @param a - a DoubleType
 * @param b - a DoubleType
 * @return 1 if a > b, -1 if a < b, 0 if a == b
 */
int cmpFunc(const void *a, const void *b);

/**
 * returns the dimension of the points in kdArray
 * @param kdArray - A pointer to the KDArray
 * @return the dimension of the points in kdArray
 */
int getKDArrayDim(KDArray *kdArray);


/**
 * returns the number of the points in kdArray
 * @param kdArray - A pointer to the KDArray
 * @return the number of the points in kdArray
 */
size_t getSize(KDArray *kdArray);

/**
 * returns the index matrix of the kdArray
 * @param kdArray A pointer to the KDArray
 * @return the index matrix of the kdArray
 */
int **getMatrix(KDArray *kdArray);

/**
 * returns the points of the kdArray
 * @param kdArray - A pointer to the KDArray
 * @return the points of the kdArray
 */
SPPoint **getArr(KDArray *kdArray);

/**
 * Returns the median according to the i-th dimension
 * @param kdArray - A pointer to the KDArray
 * @param i - the dinsion
 * @return the median according to the i-th dimension
 */
double getMedian(KDArray *kdArray, int i);

#endif //KDTREE_KDARRAY_H
