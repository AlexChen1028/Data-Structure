#include <iostream>
#include <cstring>
#include <vector>
using namespace std;

// Node class
class Node {
    static const int SYMBOLS = 26;
public:
    vector<Node *> children;
    vector<string> edgeLabel;
    bool isEnd;

    Node(bool isEnd) : isEnd(isEnd) {
        children.resize(SYMBOLS, nullptr);
        edgeLabel.resize(SYMBOLS);
    }
};

// Trie class
class Trie {
private:
    Node *root;
    char CASE;

    // Function that creates new string
    // from an existing string starting
    // from the given index
    string strCopy(string str, int index) {
        string result;
        for (int i = index; i < str.length(); ++i) {
            result += str[i];
        }
        return result;
    }

    // Function to print the word
    // starting from the given node
    void printUtil(Node *node, string str) {
        if (node->isEnd) {
            cout << str << endl;
        }

        for (int i = 0; i < node->edgeLabel.size(); ++i) {
            if (!node->edgeLabel[i].empty()) {
                string temp = str + node->edgeLabel[i];
                printUtil(node->children[i], temp);
            }
        }
    }

public:
    Trie(char CASE = 'a') : CASE(CASE) {
        root = new Node(false);
    }

    // Function to insert a word in
    // the compressed trie
    void insert(string word) {
        Node *trav = root;
        int i = 0;

        while (i < word.length() && !trav->edgeLabel[word[i] - CASE].empty()) {
            int index = word[i] - CASE;
            int j = 0;
            string label = trav->edgeLabel[index];

            while (j < label.length() && i < word.length() && label[j] == word[i]) {
                ++i;
                ++j;
            }

            if (j == label.length()) {
                trav = trav->children[index];
            } else {
                if (i == word.length()) {
                    Node *existingChild = trav->children[index];
                    Node *newChild = new Node(true);
                    string remainingLabel = label.substr(j);
                    trav->children[index] = newChild;
                    newChild->children[remainingLabel[0] - CASE] = existingChild;
                    newChild->edgeLabel[remainingLabel[0] - CASE] = remainingLabel;
                } else {
                    string remainingLabel = label.substr(j);
                    Node *newChild = new Node(false);
                    string remainingWord = word.substr(i);
                    Node *temp = trav->children[index];
                    trav->children[index] = newChild;
                    newChild->edgeLabel[remainingLabel[0] - CASE] = remainingLabel;
                    newChild->children[remainingLabel[0] - CASE] = temp;
                    newChild->edgeLabel[remainingWord[0] - CASE] = remainingWord;
                    newChild->children[remainingWord[0] - CASE] = new Node(true);
                }
                return;
            }
        }

        if (i < word.length()) {
            trav->edgeLabel[word[i] - CASE] = strCopy(word, i);
            trav->children[word[i] - CASE] = new Node(true);
        } else {
            trav->isEnd = true;
        }
    }

    // Function to print the Trie
    void print() {
        printUtil(root, "");
    }

    // Function to search a word
    bool search(string word) {
        int i = 0;
        Node *trav = root;

        while (i < word.length() && !trav->edgeLabel[word[i] - CASE].empty()) {
            int index = word[i] - CASE;
            string label = trav->edgeLabel[index];
            int j = 0;

            while (i < word.length() && j < label.length()) {
                if (word[i] != label[j]) {
                    return false;
                }
                i++;
                j++;
            }

            if (j == label.length() && i <= word.length()) {
                trav = trav->children[index];
            } else {
                return false;
            }
        }

        return i == word.length() && trav->isEnd;
    }

    // Function to search the prefix
    bool startsWith(string prefix) {
        int i = 0;
        Node *trav = root;

        while (i < prefix.length() && !trav->edgeLabel[prefix[i] - CASE].empty()) {
            int index = prefix[i] - CASE;
            string label = trav->edgeLabel[index];
            int j = 0;

            while (i < prefix.length() && j < label.length()) {
                if (prefix[i] != label[j]) {
                    return false;
                }
                i++;
                j++;
            }

            if (j == label.length() && i <= prefix.length()) {
                trav = trav->children[index];
            } else {
                return true;
            }
        }

        return i == prefix.length();
    }
};

// Driver code
int main() {
    Trie trie;

    // Insert words
    trie.insert("facebook");
    trie.insert("face");
    trie.insert("this");
    trie.insert("there");
    trie.insert("then");

    // Print inserted words
    trie.print();

    // Check if these words
    // are present or not
    cout << boolalpha;
    cout << trie.search("there") << endl;
    cout << trie.search("therein") << endl;
    cout << trie.startsWith("th") << endl;
    cout << trie.startsWith("fab") << endl;

    return 0;
}
//This code is contributed by Aman.