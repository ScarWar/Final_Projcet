#ifndef KDTREE_KDTREE_H
#define KDTREE_KDTREE_H

#include "SPBPriorityQueue.h"
#include "KDArray.h"

typedef struct kd_tree_t KDTree;

typedef struct kd_tree_node_t KDTreeNode;

typedef enum split_method_t {
    MAX_SPREAD,
    RANDOM,
    INCREMENTAL
} SplitMethod;

/**
 * Create a KDTree using KDTreeNodes and Split method
 * @param kdArray       - A pointer to a KDArray
 * @param splitMethod   - The split method used to split
 * @return
 */
KDTree *createKDTree(KDArray *kdArray, SplitMethod splitMethod);

/**
 * Free all the memory used in for the tree
 * @param kdTree    - Pointer to the KDTree
 */
void destroyKDTree(KDTree *kdTree);

/**
 * Creates a KDTree Node using the schema -
 * -  If the size of the KDArray equals 1 than create a leaf
 * node such that:
 *                  val = INVALID
 *                  dim = INVALID
 *                  left = NULL
 *                  right = NULL
 *                  data = KDArray->Point
 * -  Otherwise split the KDArray using {spKDTreeSplitMethod}
 *    and {dim}, and set
 *                  val = INVALID
 *                  dim = splitDim
 *                  left = KDTreeNode(leftKDArray, splitDim)
 *                  right = KDTreeNode(rightKDArray, splitDim)
 *                  data = NULL
 *
 * If kdArray is needed for future uses create
 * a copy otherwise the kdArray will be destroyed
 * @param kdArray   - A pointer to a KDArray
 * @param dim       - The dim for which the mode is created
 * @return A valid KdTree node
 */
KDTreeNode *createKDTreeNode(KDArray *kdArray, int dim, SplitMethod splitMethod);

/**
 * Free all the memory used in for the tree
 * @param kdTreeNode
 */
void destroyKDTreeNode(KDTreeNode *kdTreeNode);

/**
 * Return the next split dimension according to split method
 * MAX_SPREAD   - Define the spread of the i-th dimension to
 *                be the difference between the maximum and
 *                minimum of the i-th coordinate of all points.
 *                Split the kd-array according the dimension
 *                with the highest spread (if there are several
 *                candidates choose the lowest dimension)
 * RANDOM       - Choose a random dimension
 * INCREMENTAL  - if the splitting dimension of the upper level
 *                was {i} then the splitting dimension at this
 *                level is ({i}+ 1)%{d}
 * @param kdArray       - A pointer to a KDArray
 * @param splitMethod   - The split method to use
 * @param dim           - the dimension of the split
 * @return
 */
int getSplitDim(KDArray *kdArray, SplitMethod splitMethod, int dim);

/**
 * Return a pointer to the KDTeeNode
 * @param kdTree    - A pointer to a KDTree
 * @return a KDTreeNode
 */
KDTreeNode *getRoot(KDTree *kdTree);

/**
 * Return the dim value of the KDTreeNode
 * @param kdTreeNode    - A pointer to a KDTreeNode
 * @returnthe dim value of the KDTreeNode
 */
int getKDTreeNodeDim(KDTreeNode *kdTreeNode);

/**
 * Return the median according the split dimension
 * @param kdTreeNode    - A pointer to a KDTreeNode
 * @return Median value
 */
double getVal(KDTreeNode *kdTreeNode);

/**
 * Return the left child of the node
 * @param kdTreeNode    - A pointer to a KDTreeNode
 * @return The left child of the node
 */
KDTreeNode *getLeftChild(KDTreeNode *kdTreeNode);

/**
 * Return the right child of the node
 * @param kdTreeNode    - A pointer to a KDTreeNode
 * @return The right child of the node
 */
KDTreeNode *getRightChild(KDTreeNode *kdTreeNode);

/**
 * Return the data in the node
 * @param kdTreeNode    - A pointer to a KDTreeNode
 * @return The data in the node
 */
SPPoint *getData(KDTreeNode *kdTreeNode);

/**
 * Check if the node is a leaf. Node is a leaf if and only if
 * left and right childes are NULL
 * @param kdTreeNode
 * @return 1 if the node is a leaf, 0 otherwise
 */
int isLeaf(KDTreeNode *kdTreeNode);

/**
 * Return spKNN nearest points in the tree to {point}
 * as a min-priority queue, such that value is the
 * square distance between the points
 * @param kdTree    - A pointer to KDTree
 * @param point     - The query point
 * @param spKNN     - Number of points to return
 * @return spKNN nearest points in the tree to {point}
 */
SPBPQueue *kNearestNeighbors(KDTree *kdTree, SPPoint *point, int spKNN);

/**
 * Find nearest points recursively
 * @param kdTreeNode    - A pointer to KDTreeNode
 * @param queue         - A min-priority queue
 * @param point         - The query point
 */
void kNearestNeighborsRecursive(KDTreeNode *kdTreeNode, SPBPQueue *queue, SPPoint *point);

#endif //KDTREE_KDTREE_H
