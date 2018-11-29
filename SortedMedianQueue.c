#include <stdlib.h>
#include <math.h>

#include "MedianQueue.h"



struct median_queue_t {
	double* circular;
	double* sorted;
	size_t size;
	size_t start;
};

MedianQueue* mqCreate(const double* values, size_t size) {
	// create the median queue
	MedianQueue* mq = malloc(sizeof(MedianQueue));
	if (!mq)
		return NULL;

	mq->size = size;
	mq->start = 0;

	// allocate circular array
	mq->circular = malloc(sizeof(double) * mq->size);
	if (!mq->circular) {
		free(mq);
		return NULL;
	}

	// allocated sorted array
	mq->sorted = malloc(sizeof(double) * mq->size);
	if (!mq->circular) {
		free(mq->circular);
		free(mq);
		return NULL;
	}

	for (size_t i = 0; i < mq->size; ++i) {
		mq->circular[i] = values[i];
		mq->sorted[i] = values[i];
	}

    size_t j;
    double tmp;
    for (size_t i = 0; i < mq->size; i++){
        tmp = mq->sorted[i];
        for(j = i; j > 0 && mq->sorted[j-1] > tmp; j--)
            mq->sorted[j] = mq->sorted[j-1];
        mq->sorted[j]=tmp;
    }
	return mq;
}

void mqFree(MedianQueue* mq) {
	free(mq->sorted);
	free(mq->circular);
	free(mq);
}

void mqUpdate(MedianQueue* mq, double value) {
	if (!mq) { return; }
	double old = mq->circular[mq->start];
	mq->circular[mq->start] = value;
	mq->start = (mq->start + 1) % mq->size;

	size_t i = 0;
	while(i < mq->size && old != mq->sorted[i])
		i++;
	mq->sorted[i] = value;

	if(i != 0){
		while(i > 0 && mq->sorted[i-1] > value){
			mq->sorted[i] = mq->sorted[i-1];
			mq->sorted[i-1] = value;
			i--;
		}
	}

	if(i != mq->size-1){
		while (i < mq->size-1 && mq->sorted[i+1] < value){
			mq->sorted[i] = mq->sorted[i+1];
			mq->sorted[i+1] = value;
			i++;
		}
	}
}

double mqMedian(const MedianQueue* mq) {
	if (!mq) { return INFINITY; }
	return mq->sorted[mq->size / 2];
}
