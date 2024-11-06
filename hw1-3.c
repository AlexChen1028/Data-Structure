#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

typedef struct MinHeap {
    int size;
    int data[MAX_SIZE];
} MinHeap;

//swap two elements
void swap(int *a, int *b) {
    int temp = *a;//get a's value
    *a = *b;//give b's value to a 
    *b = temp;//give a's value to a
}

//insert a new element into the Min-Heap
void insert(MinHeap *heap, int value) {
    if (heap->size >= MAX_SIZE) {
        printf("Heap overflow\n");
        return;
    }

    //insert the new element at the end of the heap
    heap->data[heap->size] = value;
    int current = heap->size;//the number of data
    heap->size++;

    //heapify up
    while (current > 0) {
        int parent = (current - 1) / 2;
        if (heap->data[current] < heap->data[parent]) {
            swap(&heap->data[current], &heap->data[parent]);
            current = parent;
        } else {
            break;
        }
    }
}

//heapify down from a given index
void heapifyDown(MinHeap *heap, int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;

    if (left < heap->size && heap->data[left] < heap->data[smallest]) {
        smallest = left;
    }

    if (right < heap->size && heap->data[right] < heap->data[smallest]) {
        smallest = right;
    }

    if (smallest != index) {
        swap(&heap->data[index], &heap->data[smallest]);
        heapifyDown(heap, smallest);//check until index doesn't need to change
    }
}

//delete a specific element from the Min-Heap
void deleteElement(MinHeap *heap, int value) {
    int index = -1;

    //find the index of the element to be deleted
    for (int i = 0; i < heap->size; i++) {
        if (heap->data[i] == value) {
            index = i;
            break;
        }
    }

    //if element is not found, do nothing
    if (index == -1) {
        return;
    }

    //replace the element with the last element in the heap
    heap->data[index] = heap->data[heap->size - 1];
    heap->size--;

    //heapify down from the replaced position
    heapifyDown(heap, index);
}

//print the heap in level-order
void printHeap(MinHeap *heap) {
    for (int i = 0; i < heap->size; i++) {
        printf("%d ", heap->data[i]);
    }
    printf("\n");
}

int main() {
    MinHeap heap;
    heap.size = 0;

    char command[10];
    int value;

    //read commands until the end of input
    while (scanf("%s %d", command, &value) != EOF) {
        if (command[0] == 'i') {//command is "insert"
            insert(&heap, value);
        } else if (command[0] == 'd') {//command is "delete"
            deleteElement(&heap, value);
        }
    }

    //print the final state of the heap in level-order
    printHeap(&heap);

    return 0;
}