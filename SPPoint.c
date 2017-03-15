#include <wchar.h>
#include <stdlib.h>
#include <assert.h>
#include "SPPoint.h"


struct sp_point_t {
    double *data;
    int dim;
    int index;
};

SPPoint *spPointCreate(double *data, int dim, int index) {
    SPPoint *point = malloc(sizeof(SPPoint));
    if (point == NULL) return NULL;
    point->data = malloc(dim * sizeof(double));
    if (point->data == NULL) return NULL;
    for (int i = 0; i < dim; i++) {
        point->data[i] = data[i];
    }
    point->dim = dim;
    point->index = index;
    return point;
}

SPPoint *spPointCopy(SPPoint *source) {
    assert (source != NULL);
    return spPointCreate(source->data, source->dim, source->index);
}

void spPointDestroy(SPPoint *point) {
    if (point == NULL) return;
    free(point->data);
    free(point);
}

int spPointGetDimension(SPPoint *point) {
    assert(point != NULL);
    return point->dim;
}

int spPointGetIndex(SPPoint *point) {
    assert(point != NULL);
    return point->index;
}

double spPointGetAxisCoor(SPPoint *point, int axis) {
    assert(point != NULL && axis < point->dim);
    return point->data[axis];
}

double spPointL2SquaredDistance(SPPoint *p, SPPoint *q) {
    assert(p != NULL && q != NULL && p->dim == q->dim);
    double s = 0;
    for (int i = 0; i < p->dim; i++) {
        double t = p->data[i] - q->data[i];
        s += t * t;
    }
    return s;
}