#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定義 Binary Trie 節點結構
typedef struct TrieNode {
    struct TrieNode *children[2]; // 分別表示 0 和 1
    int isEndOfWord; // 是否為一個完整的數字
} TrieNode;

// 創建一個新的 Trie 節點
struct TrieNode *createNode() {
    TrieNode *node = (TrieNode *)malloc(sizeof(struct TrieNode));
    node->isEndOfWord = 0;
    node->children[0] = NULL;
    node->children[1] = NULL;
    return node;
}

// 插入二進位字串到 Binary Trie 中
void insert(struct TrieNode *root, const char *binaryStr) {
    TrieNode *current = root;
    for (int i = 0; binaryStr[i] != '\0'; i++) {  // Read until null character
        int index = binaryStr[i] - '0'; // 轉換 '0' 或 '1' 為整數
        if (current->children[index] == NULL) {
            current->children[index] = createNode();
        }
        current = current->children[index];
    }
    current->isEndOfWord = 1; // 標記為完整數字的結尾
}

// 搜尋二進位字串是否存在於 Binary Trie 中
int search(struct TrieNode *root, const char *binaryStr) {
    TrieNode *current = root;
    for (int i = 0; binaryStr[i] != '\0'; i++) {
        int index = binaryStr[i] - '0';
        if (current->children[index] == NULL) {
            return 0; // 未找到該二進位字串
        }
        current = current->children[index];
    }
    return current != NULL && current->isEndOfWord;
}

// 刪除二進位字串
int delete(struct TrieNode *root, const char *binaryStr) {
    if (root == NULL)
        return 0; // 如果 Trie 為空，直接返回

    if (*binaryStr == '\0') {
        root->isEndOfWord = 0; // 標記為非完整數字
        return (root->children[0] == NULL && root->children[1] == NULL); // 如果該節點沒有子節點，則可以刪除
    }

    int index = *binaryStr - '0';

    // 遞迴到下一層
    if (delete(root->children[index], binaryStr + 1)) {
        free(root->children[index]);
        root->children[index] = NULL;

        // 若該節點沒有子節點，且不是完整數字的結尾，則可以刪除
        return(!root->isEndOfWord && root->children[0] == NULL && root->children[1] == NULL);
    }
    return 0;
}

// 打印 Trie 中的所有二進位字串（輔助函數）
void printTrie(struct TrieNode *root, char *buffer, int depth) {
    if (root == NULL) return;

    // 如果該節點是完整數字的結尾，則打印當前的二進位字串
    if (root->isEndOfWord) {
        buffer[depth] = '\0'; // 結束字串
        printf("%s\n", buffer);
    }

    // 遞迴處理左右子樹
    for (int i = 0; i < 2; i++) {
        if (root->children[i] != NULL) {
            buffer[depth] = '0' + i; // 設置當前字符為 '0' 或 '1'
            printTrie(root->children[i], buffer, depth + 1);
        }
    }

}

// 主函數
int main() {
    TrieNode *root = createNode();

    // 插入一些二進位字串
    insert(root, "101");
    insert(root, "111");
    insert(root, "1001");
    insert(root, "110");

    // 刪除某些二進位字串
    delete(root, "101");

    // 搜尋某些二進位字串
    printf("Searching for 101: %s\n", search(root, "101") ? "Found" : "Not Found");
    printf("Searching for 111: %s\n", search(root, "111") ? "Found" : "Not Found");
    printf("Searching for 000: %s\n", search(root, "000") ? "Found" : "Not Found");

    // 打印 Binary Trie 中的所有二進位字串
    printf("Binary Trie contents:\n");
    char buffer[32]; // 假設字串最大長度為 32
    printTrie(root, buffer, 0);

    return 0;

}
