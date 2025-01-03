#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUCKETS 100
#define MAX_SLOTS 20
#define EMPTY -1

typedef struct {
    int slots[MAX_SLOTS];
} Bucket;

Bucket hashTable[MAX_BUCKETS];
int n, m;

// 初始化哈希表
void initializeHashTable() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            hashTable[i].slots[j] = EMPTY;
        }
    }
}

// 插入鍵
void insertKey(int key) {
    int hashIndex = key % n;
    int originalIndex = hashIndex;
    do {
        for (int i = 0; i < m; i++) {
            if (hashTable[hashIndex].slots[i] == EMPTY) {
                hashTable[hashIndex].slots[i] = key;
                return;
            }
        }
        hashIndex = (hashIndex + 1) % n;
    } while (hashIndex != originalIndex);

    printf("Hash table is full. Cannot insert key %d.\n", key);
}

// 搜索鍵
void searchKey(int key) {
    int hashIndex = key % n;
    int originalIndex = hashIndex;
    do {
        for (int i = 0; i < m; i++) {
            if (hashTable[hashIndex].slots[i] == key) {
                printf("%d %d\n", hashIndex, i);
                return;
            }
        }
        hashIndex = (hashIndex + 1) % n;
    } while (hashIndex != originalIndex);
}

// 刪除鍵
void deleteKey(int key) {
    int hashIndex = key % n;
    int originalIndex = hashIndex;
    do {
        for (int i = 0; i < m; i++) {
            if (hashTable[hashIndex].slots[i] == key) {
                hashTable[hashIndex].slots[i] = EMPTY;
                return;
            }
        }
        hashIndex = (hashIndex + 1) % n;
    } while (hashIndex != originalIndex);
}

int main() {
    char input[50];
    char command[20];
    int key;

    // 讀取桶數
    fgets(input, sizeof(input), stdin);
    sscanf(input, "bucket %d", &n);

    // 讀取槽數
    fgets(input, sizeof(input), stdin);
    sscanf(input, "slot %d", &m);

    // 初始化哈希表
    initializeHashTable();

    while (1) {
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%s", command);

        if (strcmp(command, "insert") == 0) {
            sscanf(input, "%*s %d", &key);
            insertKey(key);
        } else if (strcmp(command, "search") == 0) {
            sscanf(input, "%*s %d", &key);
            searchKey(key);
        } else if (strcmp(command, "delete") == 0) {
            sscanf(input, "%*s %d", &key);
            deleteKey(key);
        } else if (strcmp(command, "exit") == 0) {
            break;
        }
    }

    return 0;
}