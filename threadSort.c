/*
 * Assigns two threads to sort opposites halves of a list, then uses a third thread to merge the two halves
 * University of Rhode Island, CSC 412, Operating Systems and Networks
 * Project 2
 * 2/15/2014
 * gcc threadSort.c -pthread -std=c99 -o ThreadSort
 * @author Robert Corey
 * @return 0 if succesful
 * @return -1 is fails
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE_OF_ARRAY 10
//Declare function headers
int partition(int list[], int head, int tail, int pivot);
void swap(int list[], int i, int j);
void *quicksort(void *quicksortArg);
void *quicksortHelper(void *quicksortArg);
void *listMerge(void *listArg);
int isSorted(int list[], int size);

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
	//Print Title
	printf(" _____ _                    _   ___          _   \n");
	printf("|_   _| |_  _ _ ___ __ _ __| | / __| ___ _ _| |_ \n");
	printf("  | | | ' \\| '_/ -_) _` / _` | \\__ \\/ _ \\ '_|  _|\n");
	printf("  |_| |_||_|_| \\___\\__,_\\__,_| |___/\\___/_|  \\__|\n");
	printf("\nRobert Corey\n\n");

	//Ask user for input
	int userChoice = -1;
	int printData = 0;
	while(userChoice < 0){
		printf("Enter size of list to be sorted, or 0 for %i entries: ", SIZE_OF_ARRAY);
		scanf("%d",&userChoice);
		if (userChoice < 0){
			printf("\nPlease enter a number greater than or equal to 0\n");
		}
	}
	printf("Press 0 for quiet mode or 1 for loud mode: ");
	scanf("%d",&printData);

	//Initialze the array
	int size = 0;
	if (userChoice == 0)
	{
		size = SIZE_OF_ARRAY;
	}else{
		size = userChoice;
	}
	//size is dynamic, so use malloc
	int *originalList = (int *) malloc(size * sizeof(int));
	int *sortedList = (int *) malloc(size * sizeof(int));
	//fill the array with random numbers
	for(int i = 0; i < size; i++){
		originalList[i] = rand()%100;
	}
	if (printData == 1)
	{
		printf("Unsorted List:\n");
		for (int i = 0; i < size; ++i)
		{
			printf("%i ", originalList[i]);
		}
		printf("\n");
	}
	//Create listData structures pointing to the two different sublists
	//Points to the first half of the list
	struct listData headListData;
	headListData.list = originalList;
	headListData.head = 0;
	headListData.tail = ((size / 2) - 1);
	//Points to the second half of the list
	struct listData tailListData;
	tailListData.list = originalList;
	tailListData.head = (size / 2);
	tailListData.tail = (size - 1);

	//Initialize pthreads
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	int error1, error2; 
	//Create the sorting threads and check for errors
	error1 = pthread_create(&thread1,&attr, quicksortHelper, (void *)&headListData);
	error2 = pthread_create(&thread2,&attr, quicksortHelper, (void *)&tailListData);

	if(error1){
		exit(-1);
	}
	if(error1){
		exit(-1);
	}
	//Wait for threads to finish
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	if (printData == 1){
		printf("Sorted List 1:\n");
		for (int i = 0; i < size; ++i)
		{
			if(i == size/2){
				printf("\nSorted List 2:\n");
			}
			printf("%i ", originalList[i]);
		}
		printf("\n");
	}
	//Merge the list
	struct mergeData argumentsForListMerge;
	argumentsForListMerge.sourceList = originalList;
	argumentsForListMerge.destinationList = sortedList;
	argumentsForListMerge.listSize = size;
	error1 = pthread_create(&thread3,&attr, listMerge, (void *)&argumentsForListMerge);
	if(error1){
		exit(-1);
	}
	//Wait for thread to finish
	pthread_join(thread3,NULL);
	if (printData == 1)
	{
		printf("Sorted List:\n");
		for (int i = 0; i < size; ++i)
		{
			printf("%i ", sortedList[i]);
		}
		printf("\n");
	}
	//Check if array is sorted
	isSorted(sortedList,size);
	system("ps");
	//Delete arrays	
	free(originalList);
	free(sortedList);
	return 0;
}
/**
 * Takes a pivot index of a list then moves all items smaller than the pivot to the left and all larger to the right
 * @param list[] an array of integers containing the target list 
 * @param head the index of the first element of the list
 * @param tail the index of the last element of the list
 * @param pivot the index of the element that the list will be sorted around
 * @return moveHere the new index of the pivot element after sorting has finished
 */    
int partition(int list[], int head, int tail, int pivot){
	int pivotValue = list[pivot];
	//Move the pivot to the end of list temporarily
	swap(list,pivot,tail);
	//
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
/**
 * swaps to elements in an integer array
 * @param list[] array of ints containing elements
 * @param i the first element
 * @param j the second element
 */ 
void swap(int list[],int i, int j){
	int temp = list[i];
	list[i] = list[j];
	list[j] = temp;
}
void *quicksortHelper(void *quicksortArg){
	printf("Sort Thread ID : %u\n", (unsigned int)pthread_self());
	quicksort(quicksortArg);
}
/**
 *Quicksort sorting algorithem for use with threads. Sorts an array of integers from smallest to largest
 * @param quicksortArg a pointer to listData structure
 */ 
void *quicksort(void *quicksortArg){
	struct listData *quicksortData;
	quicksortData = (struct listData *) quicksortArg;

	int length = ((quicksortData->tail - quicksortData->head) + 1);
	// if list size == 0 || list size == 1 it doesn't need to be sorted
	if (quicksortData->head < quicksortData->tail){
		int pivot = rand() % length; //get a range of possible pivots eg range for a list of size 3 is (0,1,2);
		pivot += quicksortData->head; //since we're working with an array and not an actual list shift to the target elements
		//partition the list
		int newPivotIndex = partition(quicksortData->list,quicksortData->head,quicksortData->tail,pivot);
		// Sort everything to the left of the pivot 
		struct listData smallListData;
		smallListData.list = quicksortData->list;
		smallListData.head = quicksortData->head;
		smallListData.tail = newPivotIndex - 1;
		quicksort( (void *) &smallListData );
		//Sort everything to the right of the pivot
		struct listData bigListData;
		bigListData.list = quicksortData->list;
		bigListData.head = newPivotIndex + 1;
		bigListData.tail = quicksortData->tail;
		quicksort( (void *) &bigListData );	
	}	
}
/**
 * Merges two sorted arrays into one new array
 * @param a pointer to a mergeData object 
 */ 
void *listMerge(void *listArg){
	printf("Merge Thread ID: %u\n", (unsigned int)pthread_self());
	struct mergeData *localMergeData;
	localMergeData = (struct mergeData *) listArg;

	int length = localMergeData->listSize;
	int head1 = 0;
	int head2 = length/2;
	int tail1 = ((length/2) - 1);
	int tail2 = length - 1;
	int destinationListIndex = 0;
	//Traverse both lists until the end of one is reached
	while (head1 <= tail1 && head2 <= tail2){
		//compare the heads of the two lists, insert the smaller one into the new list and make its head the next element,
		//Then increment the insert position for destination list.
		if (localMergeData->sourceList[head1] < localMergeData->sourceList[head2]){
			localMergeData->destinationList[destinationListIndex] = localMergeData->sourceList[head1];
			head1++;
		}else{
			localMergeData->destinationList[destinationListIndex] = localMergeData->sourceList[head2];
			head2++;
		}
		destinationListIndex++;
	}
	//Once one list is exausted concatenate the elements of the non empty list into the new array
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
/**
 * Checks if an array of integers is sorted
 * @param list[] an array of integers
 * @param size the size of the array
 * @return 0 the list is not sorted
 * @return 1 the list is sorted
 */ 
int isSorted(int list[],int size){
	for (int i = 1; i < size; ++i)
	{
		if(i < (i-1)){
			printf("ERROR! List is not sorted correctly\n");
			return 0;
		}
	}
	printf("List sorted succesfully!\n");
	return 1;
}
