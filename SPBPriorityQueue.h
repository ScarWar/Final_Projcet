#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_

#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 * An implementation of Maximal size Min-Queue using an array, every
 * element consists of a value and an index. The values are positive
 * doubles and index are non-negative integers.
 *
 * spBPQueueCreate      - Creates a new queue
 * spBPQueueCopy        - Creates a new copy of a given queue
 * spBPQueueDestroy     - Free all resources associated with a queue
 * spBPQueueClear       - Clear all data from a given queue
 * spBPQueueSize        - A getter of the current size of a queue
 * spBPQueueGetMaxSize  - A getter of the maximal size of a queue
 * spBPQueueEnqueue     - Inserts a new element to the queue
 * spBPQueueDequeue     - Removes the smallest element from the queue
 * spBPQueuePeek        - Sets to a given element the smallest element in the queue
 * spBPQueuePeekLast    - Sets to a given element the largest element in the queue
 * spBPQueueMinValue    - Return the minimal value stored in the queue
 * spBPQueueMaxValue    - Return the maximal value stored in the queue
 * spBPQueueIsEmpty     - Return True if the queue is empty, otherwise False
 * spBPQueueIsFull      - Return True if the queue is full, otherwise False
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t SPBPQueue;

typedef struct sp_bpq_element_t {
    int index;
    double value;
} BPQueueElement;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
    SP_BPQUEUE_OUT_OF_MEMORY,
    SP_BPQUEUE_FULL,
    SP_BPQUEUE_EMPTY,
    SP_BPQUEUE_INVALID_ARGUMENT,
    SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;


/**
 * Creates new min-queue with maximum number of elements
 * @param maxSize - integer, sets the maximum size of the queue
 * @return pointer to the newly created queue returns NULL pointer
 * upon allocation error, or if the maxSize argument is invalid
 */
SPBPQueue *spBPQueueCreate(int maxSize);

/**
 * Creates a queue with the copied data from
 * the source queue and returns it.
 * @param source - The object being copied
 * @return New copy of the source queue
 */
SPBPQueue *spBPQueueCopy(SPBPQueue *source);

/**
 * Frees memory from the queue, including the
 * data inside the queue. If the source is NULL
 * nothing happens.
 * @param source - the queue being destroyed.
 */
void spBPQueueDestroy(SPBPQueue *source);

/**
 * Clears the source queue from data by resetting the
 * current size of the queue to 0, doing that any actions
 * on the queue will override previous data.
 * The source queue will behave as if it has no
 * data in it.
 * @param source - the queue being cleared from data
 */
void spBPQueueClear(SPBPQueue *source);

/**
 * Returns the number of elements in the queue.
 * @param source - The queue for which we return its size
 * For example - a queue with two points will return 2
 */
int spBPQueueSize(SPBPQueue *source);

/**
 * Returns the maximal size which was set to the queue
 * @param source - the queue for which the maximal size
 * is being returned.
 */
int spBPQueueGetMaxSize(SPBPQueue *source);

/**
 * Inserts a new element to the queue in decreasing
 * order based on the value, If the queue is full
 * the element with largest value is pushed out
 * of the queue, if the queue is not full the size
 * of the queue is updated.
 * A message is being returned to represents the
 * status of the enqueue, An invalid argument
 * message for NULL source, a success message if
 * no error occurs, e.g. the element was enqueued
 * to the correct place in the queue.
 *
 * Assuming the queue is not full and the index of placement
 * is i, all the element which placed in index greater then i
 * will move to index {currentIndex + 1}
 *
 * Assuming the queue is full, if the value of the new element
 * is greater then the value of the of the largest element in
 * the queue then the new element won't be inserted. Otherwise
 * assuming the index of placement is i, all the element
 * which placed in index smaller then i will move to
 * index {currentIndex - 1}  and the element in index 0
 * will be pushed out
 *
 * @param source - The queue which the new element will be inserted to.
 * @param value - The value of the new element.
 * @param index - The index of the new element.
 * @return A message which indicates the status of the insertion
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue *source, int index, double value);

/**
 * Removes the element with the smallest value from the queue and
 * updates the size of the queue. If the queue is NULL an invalid
 * argument message is returned. If the queue is empty an empty
 * queue message is returned, if the dequeue proceeded successfully
 * a success message is returned.
 * @param source - The queue from the element is removed.
 * @return A message which indicates the status of the dequeue.
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue *source);

/**
 * Pointing res to the element in the queue which have the
 * smallest value and returning a message which indicates
 * if the process was done successfully. If the queue is
 * NULL an invalid argument message is returned, If the
 * queue is empty an empty queue message is returned.
 * Assuming the queue is order in increasing order by value.
 *
 * Given Q = (P_0,P_1,...,P_{currentSize-1}) such that:
 * - P_i = Q[i] (The ith element of source and P are the same)
 * - currentSize = Q->currentSize, the size of queue Q
 * res = P_{currentSize-1}
 *
 * @param source - The queue which we peek from
 * @param res - a pointer to the data of the smallest element
 * @return A message which indicates the success of peeking
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue *source, BPQueueElement *res);

/**
 * Pointing res to the element in the queue which have the
 * largest value and returning a message which indicates
 * if the process was done successfully. If the queue is
 * NULL an invalid argument message is returned, If the
 * queue is empty an empty queue message is returned.
 * Assuming the queue is order in increasing order by value.
 *
 * Given Q = (P_0,P_1,...,P_{currentSize-1}) such that:
 * - P_i = Q[i] (The ith element of source and P are the same)
 * - currentSize = Q->currentSize, the size of queue Q
 * res = P_{0}
 *
 * @param source - The queue which we peek from
 * @param res - a pointer to the data of the smallest element
 * @return A message which indicates the success of peeking
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue *source, BPQueueElement *res);

/**
 * Returns the minimal value which stored in queue
 * if no errors occur.
 * @param source - A queue
 * @return The minimal value stored in on the
 * elements in queue, otherwise 0 is returned.
 */
double spBPQueueMinValue(SPBPQueue *source);

/**
 * Returns the maximal value which stored in queue
 * if no errors occur.
 * @param source - A queue
 * @return The maximal value stored in on the
 * elements in queue, otherwise 0 is returned.
 */
double spBPQueueMaxValue(SPBPQueue *source);

/**
 * @param source - A queue
 * @assert (source != NULL)
 * @return True if the queue has no elements in
 * it, Otherwise False is returned.
 */
bool spBPQueueIsEmpty(SPBPQueue *source);

/**
 * @param source - A queue
 * @assert (source != NULL)
 * @return True if the queue has maxSize number
 * of elements in it, Otherwise False is returned.
 */
bool spBPQueueIsFull(SPBPQueue *source);

#endif
