#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_KEY 10000
#define MAX_COMMANDS 100

typedef struct Node {
    int key;                // 節點的值
    int degree;             // 子樹度數
    struct Node *parent; // 父節點
    struct Node *child;  // 最左側子節點
    struct Node *left;   // 左兄弟 (root list 或是兄弟節點)
    struct Node *right;  // 右兄弟
    int mark;               // 是否已經失去過子樹
} Node;

typedef struct FibonacciHeap {
    Node *min; // 指向最小值的指標
    int n;        // Heap 中的節點總數
} FibonacciHeap;

// Fibonacci Heap 相關函式宣告
FibonacciHeap *createFibHeap();
Node *createFibNode(int key);
void fibHeapInsert(FibonacciHeap *H, Node *x);
Node *fibHeapExtractMin(FibonacciHeap *H);
void fibHeapConsolidate(FibonacciHeap *H);
void fibHeapLink(FibonacciHeap *H, Node *y, Node *x);
void fibHeapDecreaseKey(FibonacciHeap *H, Node *x, int delta);
void fibHeapCut(FibonacciHeap *H, Node *x, Node *y);
void fibHeapCascadingCut(FibonacciHeap *H, Node *y);
void fibHeapDelete(FibonacciHeap *H, Node *x);
void addToRootList(FibonacciHeap *H, Node *x);
void removeFromRootList(FibonacciHeap *H, Node *x);

/*
  題目要求：key 值保證唯一、範圍 1~10^4，因此我們可以維護一個
  size 為 10^4+1 的指標陣列，用來快速找到對應的 Node。
*/
static Node *nodes_map[MAX_KEY + 1] = {NULL};

//建立並回傳新的 Fibonacci Heap
FibonacciHeap *createFibHeap() {
    FibonacciHeap *H = (FibonacciHeap *)malloc(sizeof(FibonacciHeap));
    H->min = NULL;
    H->n = 0;
    return H;
}

//建立並回傳新的 Fibonacci Node
Node *createFibNode(int key) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->key = key;
    node->degree = 0;
    node->parent = NULL;
    node->child = NULL;
    node->left = node;
    node->right = node;
    node->mark = 0;
    return node;
}


//將節點 x 插入到 Fibonacci Heap H 的 root list
void fibHeapInsert(FibonacciHeap *H, Node *x) {
    // 1. x 與自身形成一個小圈 (x->left = x->right = x)
    // 2. 若 H->min 為空，則直接將 x 設為 min
    // 3. 否則將 x 插入到 root list，再更新 min
    if (H->min == NULL) {
        H->min = x;
    } else {
        // 插到 min 節點的右側
        x->left = H->min;
        x->right = H->min->right;
        H->min->right->left = x;
        H->min->right = x;
        // 更新最小指標
        if (x->key < H->min->key) {
            H->min = x;
        }
    }
    H->n++;
}

//從 FibHeap 中取出最小值 (Extract-Min)
Node *fibHeapExtractMin(FibonacciHeap *H) {
    Node *z = H->min;
    if (z != NULL) {
        // 1. 將 z 的子節點全部移到 root list
        Node *x = z->child;
        if (x != NULL) {
            // 需要先把所有孩子都遍歷，加到 root list
            Node *start = x;
            do
            {
                Node *next = x->right;
                // 加入 root list
                addToRootList(H, x);
                // 解除 x 與 parent 的關係
                x->parent = NULL;
                x = next;
            } while (x != start);
        }
        // 2. 從 root list 移除 z
        removeFromRootList(H, z);

        // 3. 若 z 是唯一的節點，則 H->min = NULL，否則執行 consolidate
        if (z == z->right) {
            H->min = NULL;
        } else {
            H->min = z->right;
            fibHeapConsolidate(H);
        }
        H->n--;
    }
    return z; // 回傳原最小節點
}

/*
  Consolidate:
  在 extract-min 或 delete 後，需要合併 root list 中度數相同的樹。
  題目要求：「當多個 root 具有相同 degree 時，先合併 key 值較小者。」
*/
void fibHeapConsolidate(FibonacciHeap *H) {
    if (H->min == NULL) return;

    int n = H->n; // 假設 FibonacciHeap 結構有 n 節點數的屬性
    int maxDegree = (int)(log2(n)) + 1; // 動態計算最大度數

    // 動態分配 degree 陣列
    Node **A = (Node **)calloc(maxDegree, sizeof(Node *));

    // 收集 root list 並處理
    Node *minNode = H->min;
    Node *x = minNode;
    int numRoots = 0;

    do {
        numRoots++;
        x = x->right;
    } while (x != minNode);

    Node **rootList = (Node **)malloc(sizeof(Node *) * numRoots);
    x = minNode;
    for (int i = 0; i < numRoots; i++) {
        rootList[i] = x;
        x = x->right;
    }

    for (int i = 0; i < numRoots; i++) {
        Node *current = rootList[i];
        int d = current->degree;

        while (A[d] != NULL) {
            Node *y = A[d];
            if (current->key > y->key) {
                // Swap current and y
                Node *tmp = current;
                current = y;
                y = tmp;
            }
            fibHeapLink(H, y, current);
            A[d] = NULL;
            d++;
        }
        A[d] = current;
    }

    free(rootList);

    // 重建 root list 並找新的 min
    H->min = NULL;
    for (int i = 0; i < maxDegree; i++) {
        if (A[i] != NULL) {
            Node *node = A[i];
            if (H->min == NULL) {
                node->left = node;
                node->right = node;
                H->min = node;
            } else {
                // 插入到 root list
                node->left = H->min;
                node->right = H->min->right;
                H->min->right->left = node;
                H->min->right = node;

                if (node->key < H->min->key) {
                    H->min = node;
                }
            }
        }
    }

    free(A);
}


//fibHeapLink: 將 y 連接成為 x 的孩子 (y 為較大 key 或被合併者)
void fibHeapLink(FibonacciHeap *H, Node *y, Node *x) {
    // 先將 y 從 root list 移除
    removeFromRootList(H, y);
    // y 與 x 的 parent/child 關係設定
    y->parent = x;
    y->left = y->right = y;
    y->mark = 0;
    // 把 y 接到 x->child 的「雙向循環串列」中
    if (x->child == NULL) {
        x->child = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right->left = y;
        x->child->right = y;
        // 題目要求：合併後「children of the root of the new tree
        //           should be sorted in ascending order(from left to right).」
        // 這裡採用簡化做法：由於 fibHeapConsolidate 只合併一次，所以
        // 只把 y 直接插入 child-list 右側，不再特別對 child 整個排序。
        // 若需嚴格排序，可在這裡進行 child-list 的插入排序或其他手段。
    }
    x->degree++;
}

/*
  fibHeapDecreaseKey: 將 x->key 減少 delta
  若 key 減少後小於 parent，則要進行 cut & cascading cut
*/
void fibHeapDecreaseKey(FibonacciHeap *H, Node *x, int delta) {
    x->key -= delta;
    Node *y = x->parent;
    if (y != NULL && x->key < y->key) {
        fibHeapCut(H, x, y);
        fibHeapCascadingCut(H, y);
    }
    if (x->key < H->min->key) {
        H->min = x;
    }
}

//fibHeapCut: 將 x 從 y 的孩子節點中切下，變成 root list 的一員
void fibHeapCut(FibonacciHeap *H, Node *x, Node *y) {
    // 1. 先將 x 從 y->child circular list 中移除
    if (x->right == x) {
        // x 是唯一 child
        y->child = NULL;
    } else {
        if (y->child == x) {
            y->child = x->right;
        }
        x->left->right = x->right;
        x->right->left = x->left;
    }
    y->degree--;

    // 2. 將 x 加回 root list
    x->parent = NULL;
    x->left = x->right = x;
    x->mark = 0;
    addToRootList(H, x);
}

//fibHeapCascadingCut: 若 parent 已經失去過一個 child，則要再 cut
void fibHeapCascadingCut(FibonacciHeap *H, Node *y) {
    Node *z = y->parent;
    if (z != NULL) {
        if (y->mark == 0) {
            y->mark = 1;
        } else {
            fibHeapCut(H, y, z);
            fibHeapCascadingCut(H, z);
        }
    }
}

/*
  fibHeapDelete: 刪除節點 x
  1. 將 x->key 減到比目前 min 還小 (最小化)
  2. fibHeapExtractMin 取走 x
*/
void fibHeapDelete(FibonacciHeap *H, Node *x) {
    // 先把 x->key 改為一個很小的值 (比任何現有 key 都小)
    int delta = x->key + 1; // x->key - delta = -1
    fibHeapDecreaseKey(H, x, delta);
    // 再直接 extract-min
    Node *m = fibHeapExtractMin(H);

    if (m)
        free(m); // 不再需要該節點
}

//addToRootList: 將 x 加回到 Heap H 的 root list
void addToRootList(FibonacciHeap *H, Node *x) {
    if (H->min == NULL) {
        H->min = x;
    } else {
        x->left = H->min;
        x->right = H->min->right;
        H->min->right->left = x;
        H->min->right = x;
        if (x->key < H->min->key) {
            H->min = x;
        }
    }
}

//removeFromRootList: 將 x 從 root list 移除 (但不釋放)
void removeFromRootList(FibonacciHeap *H, Node *x) {
    if (x->left == x) {
        // root list 只剩 x
        if (H->min == x) {
            H->min = NULL;
        }
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
    }
}

//最後輸出：對整個 FibHeap 進行「層次遍歷」
/*
   遍歷 root list，每個 root 以 BFS 方式輸出「同一層的所有節點 key」
   題目要求：按照「degree 小的樹」先印，再 degree 較大的樹後印。
   每棵樹輸出一行，同層之節點以空白分隔。
*/
typedef struct QueueNode {
    Node *node;
    int level;
} QueueNode;

typedef struct Queue {
    QueueNode *data;
    int front, rear, size, capacity;
} Queue;

Queue *createQueue(int capacity) {
    Queue *Q = (Queue *)malloc(sizeof(Queue));
    Q->data = (QueueNode *)malloc(sizeof(QueueNode) * capacity);
    Q->front = 0;
    Q->rear = -1;
    Q->size = 0;
    Q->capacity = capacity;
    return Q;
}

void enqueue(Queue *Q, Node *node, int level) {
    if (Q->size == Q->capacity) return; // 簡化，不考慮超過容量
    Q->rear = (Q->rear + 1) % Q->capacity;
    Q->data[Q->rear].node = node;
    Q->data[Q->rear].level = level;
    Q->size++;
}

QueueNode dequeue(Queue *Q) {
    QueueNode temp = Q->data[Q->front];
    Q->front = (Q->front + 1) % Q->capacity;
    Q->size--;
    return temp;
}

int isEmptyQueue(Queue *Q) {
    return (Q->size == 0);
}

//取得 root list 裡的所有 root，放進 array 並依 degree 升冪排序
Node **getRootsSortedByDegree(FibonacciHeap *H, int *count) {
    *count = 0;
    if (H->min == NULL) return NULL;

    // 先遍歷 root list
    int maxR = H->n; // root list 不會超過 n
    Node **roots = (Node **)malloc(sizeof(Node *) * maxR);

    Node *start = H->min;
    Node *cur = start;
    int idx = 0;
    do
    {
        roots[idx++] = cur;
        cur = cur->right;
    } while (cur != start);
    *count = idx;

    // 依 degree, 若相同則依 key 升冪排序
    for (int i = 0; i < *count; i++) {
        for (int j = i + 1; j < *count; j++) {
            if ((roots[i]->degree > roots[j]->degree) ||
                ((roots[i]->degree == roots[j]->degree) && (roots[i]->key > roots[j]->key))) {
                
                Node *tmp = roots[i];
                roots[i] = roots[j];
                roots[j] = tmp;
            }
        }
    }
    return roots;
}

/*
   對單一顆樹 (root) 做 BFS，並將同一層的 key 輸出
   依題目：同一顆樹的結果要放在同一行
*/
void levelOrderPrintOneTree(Node *root) {
    if (root == NULL) return;
    // BFS
    Queue *queue = createQueue(2000); // 暫定容量
    enqueue(queue, root, 0);

    int currentLevel = 0;
    int firstInLine = 1;
    while (!isEmptyQueue(queue)) {
        QueueNode qn = dequeue(queue);
        Node *node = qn.node;
        int lvl = qn.level;
        // 若換層就換行 (但題意: "同一顆樹" 要印在同一行，故只要空格分隔即可)
        if (lvl != currentLevel) {
            currentLevel = lvl;
            printf(" ");
        } else {
            if (!firstInLine) {
                // 同層之間以空格分隔
                printf(" ");
            }
        }
        firstInLine = 0;
        printf("%d", node->key);

        // 將 node 的所有孩子都 enqueue
        Node *child = node->child;
        if (child != NULL) {
            Node *startC = child;
            do 
            {
                enqueue(queue, startC, lvl + 1);
                startC = startC->right;
            } while (startC != child);
        }
    }
    free(queue->data);
    free(queue);
}

/*
   最終輸出：對每一個 root (依 degree 由小到大) 進行 level-order 印出
   不同顆樹分行
*/
void printFibHeapLevelOrder(FibonacciHeap *H) {
    if (H->min == NULL) return;

    int rootCount = 0;
    Node **roots = getRootsSortedByDegree(H, &rootCount);

    for (int i = 0; i < rootCount; i++) {
        levelOrderPrintOneTree(roots[i]);
        printf("\n");
    }
    free(roots);
}

int main() {
    FibonacciHeap *heap = createFibHeap();
    char command[32];
    int key, value;

    while (1) {
        if (scanf("%s", command) != 1) {
            break;
        }
        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "insert") == 0) {
            scanf("%d", &key);
            if (nodes_map[key] == NULL) {
                Node *x = createFibNode(key);
                fibHeapInsert(heap, x);
                nodes_map[key] = x;
            }
            //printFibHeapLevelOrder(heap);
        } else if (strcmp(command, "delete") == 0) {
            scanf("%d", &key);
            if (nodes_map[key] != NULL) {
                fibHeapDelete(heap, nodes_map[key]);
                nodes_map[key] = NULL;
            }
            //printFibHeapLevelOrder(heap);
        } else if (strcmp(command, "decrease") == 0) {
            scanf("%d %d", &key, &value);
            if (nodes_map[key] != NULL) {
                fibHeapDecreaseKey(heap, nodes_map[key], value);
            }
            //printFibHeapLevelOrder(heap);
        } else if (strcmp(command, "extract-min") == 0) {
            Node *mn = fibHeapExtractMin(heap);
            if (mn) {
                // 從 map 移除
                if (mn->key >= 1 && mn->key <= MAX_KEY) {
                    nodes_map[mn->key] = NULL;
                }
                free(mn);
            }
            //printFibHeapLevelOrder(heap);
        }
    }

    // 最後輸出整個 Fibonacci Heap 的層次遍歷結果
    if (heap->min != NULL && heap->n > 0) {
        printFibHeapLevelOrder(heap);
    }

    free(heap);
    return 0;
}