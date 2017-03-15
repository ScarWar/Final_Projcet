#include <stdlib.h>
#include "SPPoint.h"
#include "KDArray.h"
#include "KDTree.h"
#include <time.h>


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
    SPPoint **data;
};

KDTree *createKDTree(KDArray *kdArray) {
    if (kdArray == NULL) {
        return NULL; // TODO error message
    }

    KDTree *kdTree;
    if (!(kdTree = malloc(sizeof(KDTree)))) {
        return NULL; // TODO error message
    }

    int splitDim = getSplitDim(kdArray, spKDTreeSplitMethod, 0); // TODO Change dim, just for test
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

    if (getSize(kdArray) == 1) {
        kdTreeNode->dim = INVALID;
        kdTreeNode->val = INVALID;
        kdTreeNode->left = NULL;
        kdTreeNode->right = NULL;
        kdTreeNode->data = getArr(kdArray);


        return kdTreeNode;
    }


    KDArray **pArray;
    KDArray *kdLeft;
    KDArray *kdRight;

    splitDim = getSplitDim(kdArray, spKDTreeSplitMethod, dim);

    pArray = Split(kdArray, splitDim);
    kdLeft = pArray[0];
    kdRight = pArray[1];

    kdTreeNode->val = getMedian(kdArray, splitDim);
    kdTreeNode->dim = splitDim;
    kdTreeNode->left = createKDTreeNode(kdLeft, splitDim);
    kdTreeNode->right = createKDTreeNode(kdRight, splitDim);
    kdTreeNode->data = NULL;

    free(pArray);
    freeKDArray(kdArray);
    return kdTreeNode;

}

void destroyKDTreeNode(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL) {
        return;
    }
    if (kdTreeNode->data != NULL) {
        spPointDestroy(*kdTreeNode->data);
        free(kdTreeNode->data);
    }
    destroyKDTreeNode(kdTreeNode->left);
    destroyKDTreeNode(kdTreeNode->right);
    free(kdTreeNode);
}

int getSplitDim(KDArray *kdArray, SplitMethod splitMethod, int dim) {
    double maxSpread = 0, currSpread = 0;
    int splitIndex = -1, matIndexStart, matIndexEnd;
    if (kdArray == NULL) {
        return -1; // TODO error message
    }
    // TODO maybe to do switch statement
    if (splitMethod == RANDOM) {
        srand((unsigned int) time(NULL));
        return rand() % getKDArrayDim(kdArray);
    }
    if (splitMethod == INCREMENTAL) {
        return ++dim % getKDArrayDim(kdArray);
    }
    if (splitMethod == MAX_SPREAD) {
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

KDTreeNode *getLeft(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL) {
        return NULL; // TODO error message
    }
    return kdTreeNode->left;
}

KDTreeNode *getRight(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL) {
        return NULL; // TODO error message
    }
    return kdTreeNode->right;
}

SPPoint **getData(KDTreeNode *kdTreeNode) {
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

SPBPQueue *kNearestNeighbors(KDTree *kdTree, SPPoint *point) {
    if (kdTree == NULL || point == NULL)
        return NULL;
    SPBPQueue *queue = spBPQueueCreate(spKNN); // Change
    kNearestNeighborsRecursive(kdTree->root, queue, point);
    return queue;
}

void kNearestNeighborsRecursive(KDTreeNode *kdTreeNode, SPBPQueue *queue, SPPoint *point) {
    double t = 0;
    if (kdTreeNode == NULL) {
        return; // TODO error message
    }

    if (isLeaf(kdTreeNode)) {
        t = spPointL2SquaredDistance(*kdTreeNode->data, point);

        spBPQueueEnqueue(queue, spPointGetIndex(*kdTreeNode->data),
                         spPointL2SquaredDistance(*kdTreeNode->data, point));
        return;
    }

    bool isLeft = true;

    if (spPointGetAxisCoor(point, kdTreeNode->dim) <= kdTreeNode->val)
        kNearestNeighborsRecursive(kdTreeNode->left, queue, point);
    else {
        isLeft = false;
        kNearestNeighborsRecursive(kdTreeNode->right, queue, point);
    }

    t = kdTreeNode->val - spPointGetAxisCoor(point, kdTreeNode->dim);
    if (!spBPQueueIsFull(queue) || SQ(t) <= spBPQueueMaxValue(queue)) {
        if (isLeft)
            kNearestNeighborsRecursive(kdTreeNode->right, queue, point);
        else
            kNearestNeighborsRecursive(kdTreeNode->left, queue, point);
    }
}

void printKDTreeNode(KDTreeNode *kdTreeNode) {
    if (kdTreeNode == NULL)
        return;
}
