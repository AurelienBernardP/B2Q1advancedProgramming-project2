#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "MedianQueue.h"


/* ------------------------------------------------------------------------- *
 * Structure
 *
 * A structure for storing a floating point signal and the position of this
 * floating point signal in an array.
 * ------------------------------------------------------------------------- */
typedef struct {
    double value;
    size_t indexInCircular;
}Heaps;

/* ------------------------------------------------------------------------- *
 * Structure
 *
 * A structure for storing a floating point signal and information concerning
 * the heap where they are stored.
 * ------------------------------------------------------------------------- */
typedef struct {
    double value;
    Heaps* heapAddrs;
    size_t indexInHeaps;
}Circular;

struct median_queue_t {
	Circular* circular;
    Heaps* maxHeap;
    Heaps* minHeap;
	size_t size;
	size_t start;
};

void mqFree(MedianQueue* mq){
	free(mq->maxHeap);
	free(mq->circular);
	free(mq);
}

/* ------------------------------------------------------------------------- *
 * Swaps two elements from the max heap array in the mq object and updates
 * the value of the pointers in the circular array.
 *
 * PARAMETERS
 * mq             A median queue object.
 * a and b        Index in the array of the elements to swap.
 *
 * ------------------------------------------------------------------------- */
static void swapInMaxHeap(MedianQueue* mq, size_t a, size_t b){
    if(!mq) { return; }

    Heaps tmp= mq->maxHeap[a];
    mq->maxHeap[a] = mq->maxHeap[b];
    mq->maxHeap[b] = tmp;
    mq->circular[mq->maxHeap[a].indexInCircular].indexInHeaps = a;
    mq->circular[mq->maxHeap[b].indexInCircular].indexInHeaps = b;
    return;
}

/* ------------------------------------------------------------------------- *
 * Swaps two elements from the min heap array in the mq object and updates
 * the value of the pointers in the circular array.
 *
 * PARAMETERS
 * mq             A median queue object.
 * a and b        Index in the array of the elements to swap.
 *
 * ------------------------------------------------------------------------- */
static void swapInMinHeap(MedianQueue* mq, size_t a, size_t b){
    if(!mq) { return; }

    Heaps tmp= mq->minHeap[a];
    mq->minHeap[a] = mq->minHeap[b];
    mq->minHeap[b] = tmp;
    mq->circular[mq->minHeap[a].indexInCircular].indexInHeaps = a;
    mq->circular[mq->minHeap[b].indexInCircular].indexInHeaps = b;
    return;
}

/* ------------------------------------------------------------------------- *
 * Swaps two elements from the two heaps arrays in the mq object and updates
 * the value of the pointers in the circular array.
 *
 * PARAMETERS
 * mq             A median queue object.
 * indexInMax     Index of the element in the max heap to be swaped.
 * indexInMin     Index of the element in the min heap to be swaped.
 *
 * ------------------------------------------------------------------------- */
static void swapBetweenHeaps(MedianQueue* mq, size_t indexInMax, size_t indexInMin){
    if(!mq) { return; }

    Heaps tmp= mq->minHeap[indexInMin];
    mq->minHeap[indexInMin] = mq->maxHeap[indexInMax];
    mq->maxHeap[indexInMax] = tmp;
    mq->circular[mq->minHeap[indexInMin].indexInCircular].indexInHeaps = indexInMin;
    mq->circular[mq->maxHeap[indexInMax].indexInCircular].indexInHeaps = indexInMax;
    
    mq->circular[mq->minHeap[indexInMin].indexInCircular].heapAddrs = mq->minHeap;
    mq->circular[mq->maxHeap[indexInMax].indexInCircular].heapAddrs = mq->maxHeap;
    return;
}

/* ------------------------------------------------------------------------- *
 * Pushes a value down in a min heap until it is in its right place.
 *
 * PARAMETERS
 * mq             A median queue object.
 * nodeIndex      Index in the array of the node to be pushed down the heap.
 * end            End position of the min heap.
 *
 * ------------------------------------------------------------------------- */
static void minHeapify(MedianQueue* mq, size_t NodeIndex, size_t end){
    if (!mq){
        return;
    }
    
    size_t LeftSon = NodeIndex*2;
    size_t RightSon = (NodeIndex * 2 + 1);
    size_t IndexOfSmallest = NodeIndex;

    if((LeftSon < end) && ((mq->minHeap[LeftSon].value) < (mq->minHeap[NodeIndex].value))){
        IndexOfSmallest = LeftSon;     
    }
    if((RightSon < end)&&((mq->minHeap[RightSon].value) < (mq->minHeap[IndexOfSmallest].value))){
        IndexOfSmallest = RightSon;
    }
    if(IndexOfSmallest != NodeIndex){
        swapInMinHeap(mq, IndexOfSmallest, NodeIndex);
        minHeapify(mq, IndexOfSmallest, end);
    }
    return;
}

/* ------------------------------------------------------------------------- *
 * Pushes a value down in a max heap until it is in its right place.
 *
 * PARAMETERS
 * mq             A median queue object.
 * nodeIndex      Index in the array of the node to be pushed down the heap.
 * end            End position of the min heap.
 *
 * ------------------------------------------------------------------------- */
static void maxHeapify(MedianQueue* mq, size_t NodeIndex, size_t end){
	if (!mq){
        printf("structure empty\n");
        return;
    }

    size_t LeftSon = NodeIndex*2;
    size_t RightSon = (NodeIndex * 2 + 1);
    size_t IndexOfLargest = NodeIndex;

    if((LeftSon < end) && ((mq->maxHeap[LeftSon].value) > (mq->maxHeap[NodeIndex].value))){
        IndexOfLargest = LeftSon;
    }
    if((RightSon < end)&&((mq->maxHeap[RightSon].value) > (mq->maxHeap[IndexOfLargest].value))){
        IndexOfLargest = RightSon;
    }
    if(IndexOfLargest != NodeIndex){
        swapInMaxHeap(mq, IndexOfLargest, NodeIndex);
        maxHeapify(mq, IndexOfLargest, end);
    }
    return;
}

/* ------------------------------------------------------------------------- *
 * Pushes a value up in a min heap until it is in its right place.
 *
 * PARAMETERS
 * mq             A median queue object.
 * nodeIndex      Index in the array of the node to be pushed up the heap.
 * end            End position of the min heap.
 *
 * ------------------------------------------------------------------------- */
static void pushUpInMinHeap(MedianQueue* mq, size_t nodeIndex, size_t end){
    if (!mq){
        printf("structure empty\n");
        return;
    }
    if(nodeIndex >= end || nodeIndex == 0){
        return;
    }

    size_t parent = nodeIndex/2;

    if( mq->minHeap[nodeIndex].value <= mq->minHeap[parent].value){

        swapInMinHeap(mq, nodeIndex, parent);
        pushUpInMinHeap(mq, parent, end);
    }

    return;
}

/* ------------------------------------------------------------------------- *
 * Pushes a value up in a max heap until it is in its right place.
 *
 * PARAMETERS
 * mq             A median queue object.
 * nodeIndex      Index in the array of node to be pushed up the heap .
 * end            End position of the max heap.
 *
 * ------------------------------------------------------------------------- */
static void pushUpInMaxHeap(MedianQueue* mq, size_t nodeIndex, size_t end){
    if (!mq){
        printf("structure empty\n");
        return;
    }

    if(nodeIndex >= end || nodeIndex == 0){
        return;
    }

    size_t parent = nodeIndex/2;
    
    if(mq->maxHeap[nodeIndex].value >= mq->maxHeap[parent].value){
        swapInMaxHeap(mq, nodeIndex, parent);
        pushUpInMaxHeap(mq, parent, end);
    }

    return;
}

/* ------------------------------------------------------------------------- *
 * Builds a max heap in an array.
 *
 * PARAMETERS
 * mq             A median queue object.
 * end            End position of the max heap to be build
 *
 * ------------------------------------------------------------------------- */
static void BuildMaxHeap(MedianQueue* mq, size_t end){

    for(long int i = (end/2); i >= 0; i-- ){
        maxHeapify(mq, i, end);
    }
    return;
}

/* ------------------------------------------------------------------------- *
 * Builds a min heap in an array.
 *
 * PARAMETERS
 * mq             A median queue object.
 * end            End position of the min heap to be build
 *
 * ------------------------------------------------------------------------- */
static void BuildMinHeap(MedianQueue* mq, size_t end){

    for(long int i = (end/2); i >= 0; i-- ){
        minHeapify(mq, i, end);
    }
    return;
}

/* ------------------------------------------------------------------------- *
 * Double comparison function.
 * ------------------------------------------------------------------------- */
static int compareDouble(const void* d1, const void* d2);
static int compareDouble(const void* d1, const void* d2) {
    Heaps* val1 = (Heaps*)d1;
    Heaps* val2 = (Heaps*)d2;
	double diff = (val2->value - val1->value);
	return diff > 0 ? -1 : (diff < 0 ? 1 : 0);
}

MedianQueue* mqCreate(const double* values, size_t size) {
	// create the median queue
	MedianQueue* mq = malloc(sizeof(MedianQueue));
	if (!mq)
		return NULL;
	mq->size = size;
	mq->start = 0;

	// allocate circular array
	mq->circular = malloc(sizeof(Circular) * mq->size);
	if (!mq->circular) {
		free(mq);
		return NULL;
	}

	// allocated heaps array
	mq->maxHeap = malloc(sizeof(Heaps) * mq->size);
	if (!mq->maxHeap) {
		free(mq->circular);
		free(mq);
		return NULL;
	}

    //initiilize pointer to top of the min heap
    mq->minHeap = &mq->maxHeap[(mq->size/2)+1];
    
    //initialize both arrays
	for (size_t i = 0; i < mq->size; ++i) {
		mq->circular[i].value = values[i];
        mq->maxHeap[i].value = values[i];
        mq->circular[i].indexInHeaps = i;
        mq->maxHeap[i].indexInCircular = i;
    }
    
    //sort the values in the heaps arrays in increasing order.
    qsort(mq->maxHeap, mq->size, sizeof(Heaps), compareDouble);
    
    //init the index values of the heaps array elements
    for(size_t i = 0; i < (mq->size/2)+1; i++){
        mq->circular[mq->maxHeap[i].indexInCircular].indexInHeaps = i;
        mq->circular[mq->maxHeap[i].indexInCircular].heapAddrs = mq->maxHeap; 
    }
    for(size_t i = (mq->size/2)+1, j = 0; i < mq->size; i++, j++){
        mq->circular[mq->minHeap[j].indexInCircular].indexInHeaps = j;
        mq->circular[mq->minHeap[j].indexInCircular].heapAddrs = mq->minHeap; 
    }

    //build the maxheap of w/2 smallest values and minHeap of w/2 largest values.
    BuildMaxHeap(mq, (long int)(mq->size/2)+1);
    BuildMinHeap(mq, (long int)mq->size/2);

	return mq;
}

/* ------------------------------------------------------------------------- *
 * Returns different cases depending of the position of the new value in the
 * array compared to the value that is beeing replaced.  
 *
 * PARAMETERS
 * mq             A median queue object.
 * value          Floating point, new value in the array.
 *
 * RETURNS
 * 1,2,3,4 for different case.
 * -1 on error.
 * 0 on default, abnormal case. 
 * ------------------------------------------------------------------------- */
static int determinPlace(MedianQueue* mq, double value){
    if (!mq) { return -1; }

    if(value > mq->maxHeap[0].value){
        //value belongs in min heap
        
        if(mq->circular[mq->start].heapAddrs == mq->maxHeap){
            //out value is in max heap
            return 1;
        }else{
            //out value is in min heap
            return 2;
        }
    }else{
        //value belongs in max heap       
        
        if(mq->circular[mq->start].heapAddrs == mq->maxHeap){
            //out value is in max heap  
            return 3;  
        }else{
            //out value is in min heap
            return 4;
        }        
    }

    return 0;
}

/* ------------------------------------------------------------------------- *
 * Replaces the oldest value in to the array with the new value and places
 * the new value in its correct place in the heaps.
 *
 * PARAMETERS
 * mq             A median queue object.
 * value          Floating point, new value in the array.
 * placeOfNewValue  The case in which we are to replace the value, this is
 *                  evaluated by the determinPlace function.  
 *
 * RETURNS
 * -1 on error.
 * 0 on success. 
 * ------------------------------------------------------------------------- */
static int heapReplace(MedianQueue* mq, double value, int placeOfNewValue){
    if (!mq) { return -1; }

    switch(placeOfNewValue){
        case 1:
            mq->maxHeap[mq->circular[mq->start].indexInHeaps].value = value;//value replaced in heaps
            pushUpInMaxHeap(mq, mq->circular[mq->start].indexInHeaps, (mq->size/2)+1);//push value to top of the heap
            swapBetweenHeaps(mq, 0,0);//swap heap tops
            minHeapify(mq, mq->circular[mq->start].indexInHeaps, (mq->size/2));//push value down
            break;
        case 2:
            mq->minHeap[mq->circular[mq->start].indexInHeaps].value = value;
            pushUpInMinHeap(mq, mq->circular[mq->start].indexInHeaps, mq->size/2);
            minHeapify(mq, mq->circular[mq->start].indexInHeaps, (mq->size/2));
            break;
        case 3:
            mq->maxHeap[mq->circular[mq->start].indexInHeaps].value = value;
            pushUpInMaxHeap(mq, mq->circular[mq->start].indexInHeaps, (mq->size/2)+1);
            maxHeapify(mq, mq->circular[mq->start].indexInHeaps, (mq->size/2)+1);
            break;
        case 4:
            mq->minHeap[mq->circular[mq->start].indexInHeaps].value = value;
            pushUpInMinHeap(mq, mq->circular[mq->start].indexInHeaps, (mq->size/2)+1);
            swapBetweenHeaps(mq, 0,0);
            maxHeapify(mq, mq->circular[mq->start].indexInHeaps, (mq->size/2)+1);
            break;
        default:
            printf("error detecting position in heaps of new value\n");
            return -1;
    }
    return 0;

}

void mqUpdate(MedianQueue* mq, double value) {
    if (!mq) { return; }

    int placeOfNewValue = determinPlace(mq, value);

    mq->circular[mq->start].value = value;
	heapReplace(mq, value, placeOfNewValue );
    mq->start = (mq->start + 1) % mq->size;

    return;
}

double mqMedian(const MedianQueue* mq) {
	
    if (!mq){
         return INFINITY; }
	return (mq->maxHeap[0].value);

}