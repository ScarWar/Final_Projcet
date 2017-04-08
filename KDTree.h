#ifndef KDTREE_KDTREE_H
#define KDTREE_KDTREE_H

#include "SPBPriorityQueue.h"

typedef struct kd_tree_t KDTree;

typedef struct kd_tree_node_t KDTreeNode;

typedef enum split_method_t {
    MAX_SPREAD,
    RANDOM,
    INCREMENTAL
} SplitMethod;

/*
 * TODO write documentation
 *
 *
 *
 *
 *
 */

KDTree *createKDTree(KDArray *kdArray, SplitMethod splitMethod);

void destroyKDTree(KDTree *kdTree);

KDTreeNode *createKDTreeNode(KDArray *kdArray, SplitMethod splitMethod, int dim);

void destroyKDTreeNode(KDTreeNode *kdTreeNode);

int getSplitDim(KDArray *kdArray, SplitMethod splitMethod, int dim);

KDTreeNode *getRoot(KDTree *kdTree);

int getKDTreeNodeDim(KDTreeNode *kdTreeNode);

double getVal(KDTreeNode *kdTreeNode);

KDTreeNode *getLeft(KDTreeNode *kdTreeNode);

KDTreeNode *getRight(KDTreeNode *kdTreeNode);

SPPoint *getData(KDTreeNode *kdTreeNode);

int isLeaf(KDTreeNode *kdTreeNode);

SPBPQueue *kNearestNeighbors(KDTree *kdTree, SPPoint *point);

void kNearestNeighborsRecursive(KDTreeNode *kdTreeNode, SPBPQueue *queue, SPPoint *point);

#endif //KDTREE_KDTREE_H
