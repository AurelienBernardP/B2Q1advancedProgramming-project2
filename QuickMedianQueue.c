/* ========================================================================= *
 * QuickMedianQueue
 * Implementation of the median filter using the QuickSort algorithm.
 * ========================================================================= */

#include <stdlib.h>
#include <math.h>

#include "MedianQueue.h"


struct median_queue_t {
	double* circular;
	double* sorted;
	size_t size;
	size_t start;
};

/* -------------------------------------------------------------------- *
 * Swaps the elements of the signal and places the elements smaller than the pivot element on its left
 * and the larger or equal ones on its right.
 *
 * ARGUMENTS
 * signal: pointer to first element of an array
 * start: index of the lower boundary where the permutation takes effect in the subarray.
 * end: index of the top boundary where the permutation takes effect in the subarray.
 *
 * RETURN
 * Index of the pivot in the permutated array.
 * --------------------------------------------------------------------*/
static size_t partition(double* signal, size_t end){

    double pivot = signal[end];
    size_t i = 0;

    for(size_t j = 0; j < end; j++){
        if(signal[j] <= pivot){
            i++;
            double tmp = signal[i-1];
            signal[i-1] = signal[j];
            signal[j] = tmp;
        }
    }
    double tmp = signal[end];
    signal[end] = signal[i];
    signal[i] = tmp;
	return i;
}


/* -------------------------------------------------------------------- *
 * Using the QuickSort algorithm to find the median of a signal
 *
 * ARGUMENTS
 * signal: pointer to first element of an array
 * end: index of the top boundary of the array
 * med: index of the initial signal's median
 *
 * --------------------------------------------------------------------*/
static void quickMed(double* signal, size_t end, size_t med){
    if(end > 1){
        size_t pivot = partition(signal, end-1);

        if(pivot > med)
            return quickMed(signal, pivot, med);
        if(pivot < med)
            return quickMed(signal+pivot+1, end-(pivot+1), med-(pivot+1));
	}
    return;
}

MedianQueue* mqCreate(const double* values, size_t size){
    // create the median queue
    MedianQueue* mq = malloc(sizeof(MedianQueue));
    if(!mq)
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

	quickMed(mq->sorted, mq->size, mq->size/2);

 	return mq;
 }

void mqFree(MedianQueue* mq) {
    free(mq->sorted);
    free(mq->circular);
    free(mq);
 }

void mqUpdate(MedianQueue* mq, double value) {
    if (!mq) 
        return;
    mq->circular[mq->start] = value;
    mq->start = (mq->start + 1) % mq->size;
    for (size_t i = 0; i < mq->size; ++i)
        mq->sorted[i] = mq->circular[(mq->start + i) % mq->size];

    quickMed(mq->sorted, mq->size, mq->size/2);
 }

double mqMedian(const MedianQueue* mq) {
    if (!mq)
 	    return INFINITY;
    return mq->sorted[mq->size / 2];
 }
