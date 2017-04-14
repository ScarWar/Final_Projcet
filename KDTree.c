#include <stdlib.h>
#include "SPPoint.h"
#include "KDArray.h"
#include "KDTree.h"
#include <time.h>
#include <stdio.h>

#define INVALID -1 // TODO change if needed
#define SQ(x) ((x) * (x))


// For test only
#define spKDTreeSplitMethod RANDOM // TODO change, Just for test
#define spKNN 2 // TODO change, just for test

struct kd_tree_t {
    KDTreeNode *root;
};

struct kd_tree_node_t {
    int dim;
    double val;
    KDTreeNode *left;
    KDTreeNode *right;
    SPPoint *data;
};

// TODO change the signature so the split method is also a parameter
// KDTree *createKDTree(KDArray *kdArray, SplitMethod splitMethod)
KDTree *createKDTree(KDArray *kdArray) {
    if (kdArray == NULL) {
        return NULL; // TODO error message
    }

    KDTree *kdTree;
    if (!(kdTree = malloc(sizeof(KDTree)))) {
        return NULL; // TODO error message
    }

    int splitDim = getSplitDim(kdArray, spKDTreeSplitMethod, 0);
    kdTree->root = createKDTreeNode(kdArray, splitDim);
    return kdTree;
}

void destroyKDTree(KDTree *kdTree) {
    destroyKDTreeNode(kdTree->root);
    free(kdTree);
}

KDTreeNode *createKDTreeNode(KDArray *kdArray, int dim) {
    int splitDim;
    if (kdArray == NULL) {
        return NULL; // TODO error message
    }
    KDTreeNode *kdTreeNode;
    if (!(kdTreeNode = malloc(sizeof(KDTreeNode)))) {
        return NULL; // TODO error message
    }
    // If left with only one point in the kdArray
    // create a leaf node
    if (getSize(kdArray) == 1) {
        // Set values of the kdTree leaf node
        kdTreeNode->val = INVALID;
        kdTreeNode->dim = INVALID;
        kdTreeNode->left = NULL;
        kdTreeNode->right = NULL;

        // Create a copy of the point and free the kdArray
        kdTreeNode->data = spPointCopy(*getArr(kdArray));
        destroyKDArrayLeaf(kdArray);

        return kdTreeNode;
    }

    KDArray **tmpArray, *kdLeft, *kdRight;

    // Get the next dimension according to the split method
    // and the current dimension
    splitDim = getSplitDim(kdArray, spKDTreeSplitMethod, dim);

    // Split the kdArray at {splitDim}
    tmpArray = Split(kdArray, splitDim);
    kdLeft = tmpArray[0];
    kdRight = tmpArray[1];

    // Set values of the kdTree Node
    kdTreeNode->val = getMedian(kdArray, splitDim);
    kdTreeNode->dim = splitDim;

    // Build the KDTreeNode recursively
    kdTreeNode->left = createKDTreeNode(kdLeft, splitDim);
    kdTreeNode->right = createKDTreeNode(kdRight, splitDim);
    kdTreeNode->data = NULL;

    if (kdTreeNode->left == NULL || kdTreeNode->right == NULL) {
        // TODO error message
        destroyKDTreeNode(kdTreeNode->left);
        destroyKDTreeNode(kdTreeNode->right);
        free(kdTreeNode);
        free(tmpArray);
        destroyKDArray(kdArray);
        return NULL;
    }

    // Free memory of tmpArray and kdArray
    free(tmpArray);
    destroyKDArray(kdArray);

    return kdTreeNode;
}

void destroyKDTreeNode(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL) {
        return;
    }
    if (kdTreeNode->data != NULL) {
        // If leaf node than delete point
        spPointDestroy(kdTreeNode->data);
    } else {
        // Otherwise delete recursively left and right nodes
        destroyKDTreeNode(kdTreeNode->left);
        destroyKDTreeNode(kdTreeNode->right);
    }
    free(kdTreeNode);
}


int getSplitDim(KDArray *kdArray, SplitMethod splitMethod, int dim) {
    double maxSpread = 0, currSpread = 0;
    int splitIndex = -1, matIndexStart, matIndexEnd;

    if (kdArray == NULL) {
        return -1; // TODO error message
    }

    if (splitMethod == RANDOM) {
        // Use srand to generate random number
        srand((unsigned int) time(NULL));
        return rand() % getKDArrayDim(kdArray);
    } else if (splitMethod == INCREMENTAL) {
        // Increment the dimension modulo d
        return ++dim % getKDArrayDim(kdArray);
    } else if (splitMethod == MAX_SPREAD) {
        // Find the dimension of max spread
        for (int i = 0; i < dim; ++i) {
            for (int j = 0; j < getSize(kdArray); ++j) {
                matIndexStart = getMatrix(kdArray)[i][0];
                matIndexEnd = getMatrix(kdArray)[i][getSize(kdArray) - 1];
                currSpread = spPointGetAxisCoor(getArr(kdArray)[matIndexEnd], i) -
                             spPointGetAxisCoor(getArr(kdArray)[matIndexStart], i);
                if (currSpread > maxSpread) {
                    splitIndex = i;
                    maxSpread = currSpread;
                }
            }
        }
    }
    return splitIndex;
}

KDTreeNode *getRoot(KDTree *kdTree) {
    if (kdTree == NULL) {
        return NULL; // TODO error message
    }
    return kdTree->root;
}

int getKDTreeNodeDim(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL) {
        return INVALID; // TODO error message
    }
    return kdTreeNode->dim;
}

double getVal(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL) {
        return INVALID; // TODO error message
    }
    return kdTreeNode->val;
}

KDTreeNode *getLeftChild(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL) {
        return NULL; // TODO error message
    }
    return kdTreeNode->left;
}

KDTreeNode *getRightChild(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL) {
        return NULL; // TODO error message
    }
    return kdTreeNode->right;
}

SPPoint *getData(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL) {
        return NULL; // TODO error message
    }
    return kdTreeNode->data;
}

int isLeaf(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL)
        return 0; // TODO error message
    return (kdTreeNode->left == NULL) && (kdTreeNode->right == NULL);
}

// TODO change the signature so K is a parameter
// SPBPQueue *kNearestNeighbors(KDTree *kdTree, SPPoint *point, int kNN)
SPBPQueue *kNearestNeighbors(KDTree *kdTree, SPPoint *point) {
    if (kdTree == NULL || point == NULL)
        return NULL;
    SPBPQueue *queue = spBPQueueCreate(spKNN); // TODO Change
    kNearestNeighborsRecursive(kdTree->root, queue, point);
    return queue;
}

void kNearestNeighborsRecursive(KDTreeNode *kdTreeNode, SPBPQueue *queue, SPPoint *point) {
    double t = 0;
    if (kdTreeNode == NULL) {
        return; // TODO error message
    }

    // If the is a leaf enqueue it
    if (isLeaf(kdTreeNode)) {
        spBPQueueEnqueue(queue, spPointGetIndex(kdTreeNode->data),
                         spPointL2SquaredDistance(kdTreeNode->data, point));
        return;
    }


    // Check if the query point is at left or right hyperplane and search recursively
    bool isLeftTraversed = true; // A flag to check what hyperplane was traversed
    if (spPointGetAxisCoor(point, kdTreeNode->dim) <= kdTreeNode->val)
        kNearestNeighborsRecursive(kdTreeNode->left, queue, point);
    else {
        isLeftTraversed = false;
        kNearestNeighborsRecursive(kdTreeNode->right, queue, point);
    }

    // If the queue is not full or the next hyperplane is closer than
    // the max-priority element in the queue search it too
    t = kdTreeNode->val - spPointGetAxisCoor(point, kdTreeNode->dim);
    if (!spBPQueueIsFull(queue) || SQ(t) <= spBPQueueMaxValue(queue)) {
        if (isLeftTraversed)
            kNearestNeighborsRecursive(kdTreeNode->right, queue, point);
        else
            kNearestNeighborsRecursive(kdTreeNode->left, queue, point);
    }
}
