/* ========================================================================= *
 * quickSort
 * Implementation of the quickSort algorithm.
 * ========================================================================= */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "MedianQueue.h"


struct median_queue_t {
	double* circular;
	double* sorted;
	size_t size;
	size_t start;
};

/* -------------------------------------------------------------------- *
 * Swaps the elements of the array and places the elements smaller than the pivot element on its left
 * and the larger or equal ones on its right.
 *
 * ARGUMENTS
 * array: pointer to first element of an array
 * start: index of the lower boundary where the permutation takes effect in the subarray.
 * end: index of the top boundary where the permutation takes effect in the subarray.
 *
 * RETURN
 * Index of the pivot in the permutated array.
 * --------------------------------------------------------------------*/
static int partition(double* array, size_t end){

    int pivot = array[end];
    int i = -1;

    for(size_t j = 0; j < end; j++){
        if(array[j] <= pivot){
            i++;
            int tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
        }
    }
    int tmp = array[end];
    array[end] = array[i+1];
    array[i+1] = tmp;
    ++i;
	return i;
}


/* -------------------------------------------------------------------- *
 * Sorting algorithm, ascending order
 *
 * ARGUMENTS
 * array: pointer to first element of an array
 * start: index of the lower boundary where the sorting takes effect in the array
 * end: index of the top boundary where the sorting takes effect in the array
 *
 * RETURN
 * array sorted from array[start] to array[end]
 * --------------------------------------------------------------------*/
static void quickMed(double* signal, long int end, size_t med){
    if(end>0){
        size_t pivot = partition(signal, end);
        if(pivot > med)
            return quickMed(signal, pivot-1, med);
        if(pivot < med)
            return quickMed(signal+pivot+1, end-(pivot+1), med-(pivot+1));
	}
    return;
}

/*void new_signal(double* signal, size_t length){
    if (!signal || !length)
        return;

    //double signal_med[length];
    for(int i = 0; i < length-med_length+;i++){
        //for(int j=0; j < med_length; j++)
        //    signal_med[j] = signal[i+j];
        signal[i] = quickMed(signal_med, 0, med_length-1, med_length/2);
    }
}*/

/* ------------------------------------------------------------------------- *
 * Create a median queue of $size$ elements initialized with the values
 * stored in the array $values$.
 *
 * PARAMETERS
 * values         Array of size $size$ containing the initial values to store
 *                in the median queue.
 * size           The number of elements in the median queue.
 *
 * NOTE
 * The returned structure should be cleaned with $mqFree$ after usage.
 *
 * RETURN
 * medianQueue    A pointer to the created median queue.
 * ------------------------------------------------------------------------- */
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

	quickMed(mq->sorted, mq->size-1, mq->size/2);

 	return mq;
 }

 void mqFree(MedianQueue* mq) {
 	free(mq->sorted);
 	free(mq->circular);
 	free(mq);
 }

 void mqUpdate(MedianQueue* mq, double value) {
 	if (!mq) { return; }
 	mq->circular[mq->start] = value;
 	mq->start = (mq->start + 1) % mq->size;
 	for (size_t i = 0; i < mq->size; ++i) {
 		mq->sorted[i] = mq->circular[(mq->start + i) % mq->size];
 	}
	quickMed(mq->sorted, mq->size-1, mq->size/2);
 }

 double mqMedian(const MedianQueue* mq) {
 	if (!mq) { return INFINITY; }
 	return mq->sorted[mq->size / 2];
 }
