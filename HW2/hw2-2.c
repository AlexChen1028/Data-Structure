#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

// Fibonacci Heap Node Structure
typedef struct Node {
    int key;
    int degree;
    struct Node *parent;
    struct Node *child;
    struct Node *left;
    struct Node *right;
    int marked;
} Node;

// Fibonacci Heap Structure
typedef struct FibonacciHeap {
    Node *min;
    int n; // 節點數量
} FibonacciHeap;

// 儲存 key -> node 的對應，用在 delete/decrease-key 找節點
#define MAX_KEYS 10001
static Node *nodeArray[MAX_KEYS] = {NULL};

// Create a new node
Node *createNode(int key) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->degree = 0;
    node->parent = NULL;
    node->child = NULL;
    node->left = node;
    node->right = node;
    node->marked = 0;
    return node;
}

// Initialize a new Fibonacci Heap
FibonacciHeap *createHeap() {
    FibonacciHeap *heap = (FibonacciHeap *)malloc(sizeof(FibonacciHeap));
    if (heap == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    heap->min = NULL;
    heap->n = 0;
    return heap;
}

// Insert a node into the Fibonacci Heap
void insert(FibonacciHeap *heap, int key) {
    Node *node = createNode(key);
    nodeArray[key] = node; // 將節點儲存到 nodeArray
    if (heap->min == NULL) {
        heap->min = node;
    } else {
        // Add node to root list
        node->right = heap->min;
        node->left = heap->min->left;
        heap->min->left->right = node;
        heap->min->left = node;
        if (key < heap->min->key) {
            heap->min = node;
        }
    }
    heap->n++;
}

// Link two nodes during consolidation
void linkNodes(Node *y, Node *x) {
    y->left->right = y->right;
    y->right->left = y->left;
    y->parent = x;
    if (x->child == NULL) {
        x->child = y;
        y->right = y;
        y->left = y;
    } else {
        y->right = x->child;
        y->left = x->child->left;
        x->child->left->right = y;
        x->child->left = y;
    }
    x->degree++;
    y->marked = 0;
}

// Consolidate the root list after extract-min
void consolidate(FibonacciHeap *heap) {
    int maxDegree = (int)(log(heap->n) / log(2)) + 1;
    Node **degreeTable = (Node **)calloc(maxDegree, sizeof(Node *));
    if (degreeTable == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    Node *current = heap->min;
    Node *start = heap->min;
    Node *next;

    // Consolidate trees by degree
    do {
        Node *x = current;
        next = current->right;
        int d = x->degree;

        while (degreeTable[d] != NULL) {
            Node *y = degreeTable[d];
            if (x->key > y->key) {
                Node *temp = x;
                x = y;
                y = temp;
            }
            linkNodes(y, x);
            degreeTable[d] = NULL;
            d++;
        }
        degreeTable[d] = x;
        current = next;
    } while (current != start);

    // Reconstruct the root list and find new minimum
    heap->min = NULL;
    for (int i = 0; i < maxDegree; i++) {
        if (degreeTable[i] != NULL) {
            if (heap->min == NULL) {
                heap->min = degreeTable[i];
                degreeTable[i]->left = degreeTable[i];
                degreeTable[i]->right = degreeTable[i];
            } else {
                degreeTable[i]->left = heap->min->left;
                degreeTable[i]->right = heap->min;
                heap->min->left->right = degreeTable[i];
                heap->min->left = degreeTable[i];
                if (degreeTable[i]->key < heap->min->key) {
                    heap->min = degreeTable[i];
                }
            }
        }
    }
    free(degreeTable);
}

// Extract the minimum node from the Fibonacci Heap
void extractMin(FibonacciHeap *heap) {
    if (heap->min == NULL) return;
    Node *minNode = heap->min;
    if (minNode->child != NULL) {
        Node *child = minNode->child;
        Node *start = child;
        do {
            child->parent = NULL;
            child = child->right;
        } while (child != start);
        Node *minLeft = minNode->left;
        Node *childLeft = minNode->child->left;
        minLeft->right = minNode->child;
        minNode->child->left = minLeft;
        childLeft->right = minNode->right;
        minNode->right->left = childLeft;
    }
    if (minNode->right == minNode) {
        heap->min = NULL;
    } else {
        heap->min->left->right = heap->min->right;
        heap->min->right->left = heap->min->left;
        heap->min = minNode->right;
        consolidate(heap);
    }
    heap->n--;
    nodeArray[minNode->key] = NULL;
    free(minNode);
}

// Perform cascading cuts
void cascadingCut(FibonacciHeap *heap, Node *y) {
    Node *z = y->parent;
    if (z != NULL) {
        if (y->marked == 0) {
            y->marked = 1;
        } else {
            // 刪除 y 從其兄弟鏈中
            y->left->right = y->right;
            y->right->left = y->left;
            if (z->child == y) {
                if (y->right == y) {
                    z->child = NULL;
                } else {
                    z->child = y->right;
                }
            }
            z->degree--;

            // 將 y 插入到根列表
            y->parent = NULL;
            y->left = heap->min->left;
            y->right = heap->min;
            heap->min->left->right = y;
            heap->min->left = y;
            y->marked = 0;

            cascadingCut(heap, z);
        }
    }
}

// Decrease key operation
void decreaseKey(FibonacciHeap *heap, int key, int newKey) {
    Node *node = nodeArray[key];
    if (node == NULL) {
        fprintf(stderr, "Key not found\n");
        return;
    }
    if (newKey > node->key) {
        fprintf(stderr, "New key is greater than current key\n");
        return;
    }
    node->key = newKey;
    Node *parent = node->parent;
    if (parent != NULL && node->key < parent->key) {
        node->left->right = node->right;
        node->right->left = node->left;
        if (parent->child == node) {
            if (node->right == node) {
                parent->child = NULL;
            } else {
                parent->child = node->right;
            }
        }
        parent->degree--;
        node->parent = NULL;
        node->left = heap->min->left;
        node->right = heap->min;
        heap->min->left->right = node;
        heap->min->left = node;
        node->marked = 0;
        cascadingCut(heap, parent);
    }
    if (node->key < heap->min->key) {
        heap->min = node;
    }
}

// Delete a key
void deleteKey(FibonacciHeap *heap, int key) {
    decreaseKey(heap, key, INT_MIN);
    extractMin(heap);
}

//queue，用來BFS
typedef struct Queue {
    Node *array[100];
    int front;
    int rear;
} Queue;

void enqueue(Queue *queue, Node *node) {
    queue->array[queue->rear++] = node;
}

Node *dequeue(Queue *queue) {
    if (queue->rear == queue->front) return NULL;
    return queue->array[queue->front++];
}

int isEmpty(Queue *queue) {
    return (queue->rear == queue->front);
}

//得到root list 的tree
int getRoots(FibonacciHeap *heap, Node *root[], int size) {
    if (heap->min == NULL) return 0;

    //收集root
    int number = 0;
    Node *startNode = heap->min;
    Node *currentNode = startNode;

    do {
        root[number++] = currentNode;
        currentNode = currentNode->right;
    } while (currentNode != startNode && number < size);

    //依照degree由小到大排序，如果degree一樣就用key值
    for (int i = 0; i < number; i++) {
        for (int j = i + 1; j < number; j++) {
            if ((root[i]->degree == root[j]->degree && root[i]->key > root[j]->key) || (root[i]->degree > root[j]->degree)) {
                //swap
                Node *temp = root[i];
                root[i] = root[j];
                root[j] = temp;
            }
        }
    }
    return number;
}

//print tree
void print1Tree(Node *root) {
    if (!root) return;

    //用queue實現
    Queue queue;
    queue.front = 0;
    queue.rear = 0;

    enqueue(&queue, root);

    //BFS
    while (!isEmpty(&queue)) {
        Node *node = dequeue(&queue);

        //print key
        printf("%d ", node->key);

        //enqueue node
        if (node->child) {
            Node *ch = node->child;
            do {
                enqueue(&queue, ch);
                ch = ch->right;
            } while (ch != node->child);
        }
    }
    printf("\n");
}

//輸出root，再以degree由小到大排序(levelorder)
void printHeap(FibonacciHeap *heap) {
    if (heap->min == NULL) return;

    //抓取root，接著排序
    Node *roots[50];
    int count = getRoots(heap, roots, 50);

    //print every tree
    for (int i = 0; i < count; i++) {
        print1Tree(roots[i]);
    }
}

// Free all nodes in the Fibonacci Heap
void freeHeap(FibonacciHeap *heap) {
    while (heap->min != NULL) {
        extractMin(heap);
    }
    free(heap);
}

// Main function
int main() {
    FibonacciHeap *heap = createHeap();
    char command[20];
    while (1) {
        scanf("%s", command);
        if (strcmp(command, "insert") == 0) {
            int key;
            scanf("%d", &key);
            insert(heap, key);
            //printHeap(heap);
        } else if (strcmp(command, "extract-min") == 0) {
            extractMin(heap);
            //printHeap(heap);
        } else if (strcmp(command, "decrease") == 0) {
            int key, value;
            scanf("%d %d", &key, &value);
            decreaseKey(heap, key, key - value);
            //printHeap(heap);
        } else if (strcmp(command, "delete") == 0) {
            int key;
            scanf("%d", &key);
            deleteKey(heap, key);
            printHeap(heap);
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Invalid command\n");
        }
    }
    printHeap(heap);
    free(heap);
    return 0;
}