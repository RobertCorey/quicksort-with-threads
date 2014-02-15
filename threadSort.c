/* This program uses 2 threads that each sort half of a global array. It then uses a 3rd thread to merge the halfs of an array in 
* The sorting algorithem used is in place quicksort
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE_OF_ARRAY 10

//Declare function headers
int partition(int list[], int head, int tail, int pivot);
void swap(int list[], int i, int j);
void *quicksort(void *quicksortArg);
void *listMerge(void *listArg);

//Structure to hold arguments for quicksort
struct listData
{
	int *list;
	int head;
	int tail;
};
//Structure to hold arguments for listMerge
struct mergeData
{
	int *sourceList;
	int *destinationList;
	int listSize;
};

//Main
int main(){
	//Ask user for input
	//Initialze the array
	int size = SIZE_OF_ARRAY;
	int *originalList = (int *) malloc(size * sizeof(int));
	int *sortedList = (int *) malloc(size * sizeof(int));
	//fill the array with random numbers
	for(int i = 0; i < size; i++){
		originalList[i] = rand()%100;
	}
	//Create listData structures pointing to the two different sublists
	//points to the first half of the list
	struct listData headListData;
	headListData.list = originalList;
	headListData.head = 0;
	headListData.tail = ((size / 2) - 1);
	//points to the second half of the list
	struct listData tailListData;
	tailListData.list = originalList;
	tailListData.head = (size / 2);
	tailListData.tail = (size - 1);
	//Initialize pthreads
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	int error1, error2; //catches any errors when creating threads
	error1 = pthread_create(&thread1,NULL, quicksort, (void *)&headListData);
	error2 = pthread_create(&thread2,NULL, quicksort, (void *)&tailListData);
	if(error1){
		exit(-1);
	}
	if(error1){
		exit(-1);
	}
	//Wait for threads to finish
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	//Merge the list
	struct mergeData argumentsForListMerge;
	argumentsForListMerge.sourceList = originalList;
	argumentsForListMerge.destinationList = sortedList;
	argumentsForListMerge.listSize = size;
	error1 = pthread_create(&thread3,NULL, listMerge, (void *)&argumentsForListMerge);
	if(error1){
		exit(-1);
	}
	pthread_join(thread3,NULL);

	free(originalList);
	free(sortedList);
	return 0;
}
//Partition 
int partition(int list[], int head, int tail, int pivot){
	int pivotValue = list[pivot];
	swap(list,pivot,tail);
	int moveHere = head;
	int i;
	for (i = head; i < tail; ++i){
		if (list[i] <= pivotValue){
			swap(list,i,moveHere);
			moveHere += 1;
		}
	}
	swap(list,moveHere,tail);
	return moveHere;
}
//Swap
void swap(int list[],int i, int j){
	int temp = list[i];
	list[i] = list[j];
	list[j] = temp;
}
//Quicksort
void *quicksort(void *quicksortArg){
	struct listData *quicksortData;
	quicksortData = (struct listData *) quicksortArg;
	int length = ((quicksortData->tail - quicksortData->head) + 1);
	int i;
	if (quicksortData->head < quicksortData->tail){
		int pivot = rand() % length; //get a range of possible pivots
		pivot += quicksortData->head; //Shift pivot range so it can only choose head >= indexes <= tail
		int newPivotIndex = partition(quicksortData->list,quicksortData->head,quicksortData->tail,pivot);

		struct listData smallListData;
		smallListData.list = quicksortData->list;
		smallListData.head = quicksortData->head;
		smallListData.tail = newPivotIndex - 1;
		quicksort( (void *) &smallListData );

		struct listData bigListData;
		bigListData.list = quicksortData->list;
		bigListData.head = newPivotIndex + 1;
		bigListData.tail = quicksortData->tail;
		quicksort( (void *) &bigListData );	
	}	
}

void *listMerge(void *listArg){

	struct mergeData *localMergeData;
	localMergeData = (struct mergeData *) listArg;

	int length = localMergeData->listSize;
	int head1 = 0;
	int head2 = length/2;
	int tail1 = ((length/2) - 1);
	int tail2 = length - 1;
	int destinationListIndex = 0;

	while (head1 <= tail1 && head2 <= tail2){
		if (localMergeData->sourceList[head1] < localMergeData->sourceList[head2]){
			localMergeData->destinationList[destinationListIndex] = localMergeData->sourceList[head1];
			head1++;
		}else{
			localMergeData->destinationList[destinationListIndex] = localMergeData->sourceList[head2];
			head2++;
		}
		destinationListIndex++;
	}
	while (head1 <= tail1){
		localMergeData->destinationList[destinationListIndex] = localMergeData->sourceList[head1];
		head1++;
		destinationListIndex++;
	}
	while (head2 <= tail2){
		localMergeData->destinationList[destinationListIndex] = localMergeData->sourceList[head2];
		head2++;
		destinationListIndex++;
	}
}
