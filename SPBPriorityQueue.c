#include <stdlib.h>
#include <assert.h>
#include "SPBPriorityQueue.h"

struct sp_bp_queue_t {
    int maxSize;
    int currSize;
    BPQueueElement *data;
};

SPBPQueue *spBPQueueCreate(int maxSize) {
    SPBPQueue *queue = malloc(sizeof(SPBPQueue));
    if (queue == NULL) return NULL;
    queue->data = malloc(maxSize * sizeof(BPQueueElement));
    if (queue->data == NULL) return NULL;
    queue->maxSize = maxSize;
    queue->currSize = 0;
    return queue;
}

SPBPQueue *spBPQueueCopy(SPBPQueue *source) {
    assert(source != NULL);
    SPBPQueue *queue = spBPQueueCreate(source->maxSize);
    if (queue == NULL) return NULL;
    for (int i = 0; i < source->currSize; i++) {
        queue->data[i] = source->data[i];
    }
    queue->currSize = source->currSize;
    return queue;
}


void spBPQueueDestroy(SPBPQueue *source) {
    if (source == NULL) return;
    free(source->data);
    free(source);
}


void spBPQueueClear(SPBPQueue *source) {
    assert(source != NULL);
    source->currSize = 0;
}


int spBPQueueSize(SPBPQueue *source) {
    assert(source != NULL);
    return source->currSize;
}


int spBPQueueGetMaxSize(SPBPQueue *source) {
    assert(source != NULL);
    return source->maxSize;
}


SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue *source, int index, double value) {
    if (source == NULL) return SP_BPQUEUE_INVALID_ARGUMENT;
    int i = source->currSize;
    bool isFull = source->currSize == source->maxSize;
    while (i > 0 && (value > source->data[i - 1].value || (value == source->data[i - 1].value && index > source->data->index))) i--;
    if (i == 0 && isFull) return SP_BPQUEUE_SUCCESS;
    if (isFull) {
        int start = 0;
        i--;
        while (start < i) {
            source->data[start] = source->data[start + 1];
            start++;
        }
    } else {
        int start = source->currSize;
        while (start > i) {
            source->data[start] = source->data[start - 1];
            start--;
        }
    }
    source->data[i].value = value;
    source->data[i].index = index;

    if (isFull) return SP_BPQUEUE_SUCCESS; //SP_BPQUEUE_FULL;
    source->currSize++;

    return SP_BPQUEUE_SUCCESS;
}


SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue *source) {
    if (source == NULL) return SP_BPQUEUE_INVALID_ARGUMENT;
    if (source->currSize == 0) return SP_BPQUEUE_EMPTY;
    source->currSize--;
    return SP_BPQUEUE_SUCCESS;
}


SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue *source, BPQueueElement *res) {
    if (source == NULL) return SP_BPQUEUE_INVALID_ARGUMENT;
    if (source->currSize == 0) return SP_BPQUEUE_EMPTY;
    *res = source->data[source->currSize - 1];
    return SP_BPQUEUE_SUCCESS;
}


SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue *source, BPQueueElement *res) {
    if (source == NULL) return SP_BPQUEUE_INVALID_ARGUMENT;
    if (source->currSize == 0) return SP_BPQUEUE_EMPTY;
    *res = source->data[0];
    return SP_BPQUEUE_SUCCESS;
}


double spBPQueueMinValue(SPBPQueue *source) {
    BPQueueElement element;
    if (spBPQueuePeek(source, &element) != SP_BPQUEUE_SUCCESS) return 0; // todo think about this
    return element.value;
}


double spBPQueueMaxValue(SPBPQueue *source) {
    BPQueueElement element;
    if (spBPQueuePeekLast(source, &element) != SP_BPQUEUE_SUCCESS) return 0; // todo think about this
    return element.value;
}


bool spBPQueueIsEmpty(SPBPQueue *source) {
    assert(source != NULL);
    return source->currSize == 0;
}


bool spBPQueueIsFull(SPBPQueue *source) {
    assert(source != NULL);
    return source->currSize == source->maxSize;
}