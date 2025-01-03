#include <stdio.h>
#include <stdlib.h>

// 定义二项树节点
typedef struct Node {
    int data, degree;
    struct Node *child, *sibling, *parent;
} Node;

// 定义链表节点，用于存储二项堆中的二项树
typedef struct ListNode {
    Node *tree;
    struct ListNode *next;
} ListNode;

// 创建新的二项树节点
Node *newNode(int key) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = key;
    temp->degree = 0;
    temp->child = temp->sibling = temp->parent = NULL;
    return temp;
}

// 创建新的链表节点
ListNode *newListNode(Node *tree) {
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->tree = tree;
    node->next = NULL;
    return node;
}

// 合并两个二项树
Node *mergeBinomialTrees(Node *b1, Node *b2) {
    if (b1->data > b2->data) {
        Node *temp = b1;
        b1 = b2;
        b2 = temp;
    }
    b2->parent = b1;
    b2->sibling = b1->child;
    b1->child = b2;
    b1->degree++;
    return b1;
}

// 合并两个链表（按 degree 排序）
ListNode *unionBinomialHeap(ListNode *l1, ListNode *l2) {
    ListNode *newHeap = NULL, **tail = &newHeap;

    while (l1 && l2) {
        if (l1->tree->degree <= l2->tree->degree) {
            *tail = l1;
            l1 = l1->next;
        } else {
            *tail = l2;
            l2 = l2->next;
        }
        tail = &((*tail)->next);
    }

    // 合并剩余元素
    *tail = (l1) ? l1 : l2;
    return newHeap;
}

// 调整二项堆，确保每个 degree 唯一
ListNode *adjustHeap(ListNode *heap) {
    if (!heap || !heap->next)
        return heap;

    ListNode *prev = NULL, *curr = heap, *next = heap->next;

    while (next) {
        // 如果当前树和下一棵树的 degree 不相等，跳过当前节点。
        // 如果下下一棵树的 degree 和当前树的 degree 相等，跳过当前节点。
        if ((curr->tree->degree != next->tree->degree) ||
            (next->next && next->next->tree->degree == curr->tree->degree)) {
            prev = curr;
            curr = next;
        } else {
            // 如果 curr 的值较小
            if (curr->tree->data <= next->tree->data) {
                curr->next = next->next;
                curr->tree = mergeBinomialTrees(curr->tree, next->tree);
            }
            // 如果 next 的值较小
            else {
                if (prev)
                    prev->next = next; // Delete curr

                // 如果 prev 不存在，curr 是頭節點
                else
                    heap = next;

                next->tree = mergeBinomialTrees(next->tree, curr->tree);
                curr = next;
            }
        }
        next = curr->next;
    }

    return heap;
}

// 插入一棵二项树到堆中
ListNode *insertTreeInHeap(ListNode *heap, Node *tree) {
    ListNode *temp = newListNode(tree);
    heap = unionBinomialHeap(heap, temp);
    return adjustHeap(heap);
}

// 插入一个元素到二项堆
ListNode *insert(ListNode *heap, int key) {
    Node *tree = newNode(key);
    return insertTreeInHeap(heap, tree);
}

// 移除二项树的最小根节点，返回新的堆
ListNode *removeMinTree(Node *tree) {
    ListNode *heap = NULL, *temp;

    Node *child = tree->child;
    while (child) {
        temp = newListNode(child);
        child = child->sibling;
        temp->tree->sibling = NULL;
        temp->next = heap;
        heap = temp;
    }

    return heap;
}

// 获取堆中最小根节点
Node *getMin(ListNode *heap) {
    ListNode *curr = heap;
    Node *minNode = heap->tree;

    while (curr) {
        if (curr->tree->data < minNode->data)
            minNode = curr->tree;
        curr = curr->next;
    }

    return minNode;
}

// 删除堆中最小节点
ListNode *extractMin(ListNode *heap) {
    ListNode *prevMin = NULL, *minNode = heap, *prev = NULL, *curr = heap;

    Node *minTree = getMin(heap);

    while (curr) {
        if (curr->tree == minTree) {
            if (prev)
                prev->next = curr->next;
            else
                heap = curr->next;

            minNode = curr;
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    ListNode *childHeap = removeMinTree(minTree);
    free(minNode);
    return adjustHeap(unionBinomialHeap(heap, childHeap));
}

// 打印二项树
void printTree(Node *tree) {
    while (tree) {
        printf("%d ", tree->data);
        if (tree->child) {
            printf("(Child: ");
            printTree(tree->child);
            printf(") ");
        }
        tree = tree->sibling;
    }
}

// 打印二项堆
void printHeap(ListNode *heap) {
    ListNode *curr = heap;
    int i = 1;

    while (curr) {
        printf("Tree %d: ", i++);
        printTree(curr->tree);
        printf("\n");
        curr = curr->next;
    }
}

// 主函数测试
int main() {
    ListNode *heap = NULL;

    // 插入数据
    heap = insert(heap, 10);
    heap = insert(heap, 20);
    heap = insert(heap, 30);

    printf("Heap elements after insertion:\n");
    printHeap(heap);

    Node *min = getMin(heap);
    printf("\nMinimum element of heap: %d\n", min->data);

    // 删除最小值
    heap = extractMin(heap);
    printf("\nHeap after deleting minimum element:\n");
    printHeap(heap);

    return 0;
}