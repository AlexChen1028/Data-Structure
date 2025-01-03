#include <stdio.h>
#define M 3
#define MAX M-1
#define MIN ((M/2)-1)

struct node {
    int count;
    int value[MAX + 1];
    struct node *child[MAX + 1];
};

// Searches value in the node
struct node *search(int val,
    struct node *root,
    int *pos) {

    // if root is Null then return
    if (root == NULL)
        return NULL;
    else {

        // if node is found
        if (searchnode(val, root, pos))
            return root;

        // if not then search in child nodes
        else
            return search(val,
                root->child[*pos],
                pos);
    }
}

// Searches the node
int searchnode(int val,
    struct node *n,
    int *pos) {
    // if val is less than node->value[1]
    if (val < n->value[1]) {
        *pos = 0;
        return 0;
    }

    // if the val is greater
    else {
        *pos = n->count;

        // check in the child array
        // for correct position
        while ((val < n->value[*pos])
            && *pos > 1)
            (*pos)--;
        if (val == n->value[*pos])
            return 1;
        else
            return 0;
    }
}

// Inserts a value in the m-Way tree
struct node *insert(int val,
    struct node *root) {
    int i;
    struct node *c, *n;
    int flag;

    // Function setval() is called which
    // returns a value 0 if the new value
    // is inserted in the tree, otherwise
    // it returns a value 1
    flag = setval(val, root, &i, &c);

    if (flag) {
        n = (struct node *)malloc(sizeof(struct node));
        n->count = 1;
        n->value[1] = i;
        n->child[0] = root;
        n->child[1] = c;
        return n;
    }
    return root;
}

// Sets the value in the node
int setval(int val,
    struct node *n,
    int *p,
    struct node **c) {
    int k;

    // if node is null
    if (n == NULL) {
        *p = val;
        *c = NULL;
        return 1;
    } else {

        // Checks whether the value to be
        // inserted is present or not
        if (searchnode(val, n, &k))
            printf("Key value already exists\n");

        // The if-else condition checks whether
        // the number of nodes is greater or less
        // than the maximum number. If it is less
        // then it inserts the new value in the
        // same level node, otherwise, it splits the
        // node and then inserts the value
        if (setval(val, n->child[k], p, c)) {

            // if the count is less than the max
            if (n->count < MAX) {
                fillnode(*p, *c, n, k);
                return 0;
            } else {

                // Insert by splitting
                split(*p, *c, n, k, p, c);
                return 1;
            }
        }
        return 0;
    }
}


// Adjusts the value of the node
void fillnode(int val,
    struct node *c,
    struct node *n,
    int k) {
    int i;

    // Shifting the node by one position
    for (i = n->count; i > k; i--) {
        n->value[i + 1] = n->value[i];
        n->child[i + 1] = n->child[i];
    }
    n->value[k + 1] = val;
    n->child[k + 1] = c;
    n->count++;
}

// Splits the node
void split(int val,
    struct node *c,
    struct node *n,
    int k, int *y,
    struct node **newnode) {
    int i, mid;
    if (k <= MIN)
        mid = MIN;
    else
        mid = MIN + 1;

    // Allocating the memory for a new node
    *newnode = (struct node *)
        malloc(sizeof(struct node));

    for (i = mid + 1; i <= MAX; i++) {
        (*newnode)->value[i - mid] = n->value[i];
        (*newnode)->child[i - mid] = n->child[i];
    }

    (*newnode)->count = MAX - mid;
    n->count = mid;

    // it checks whether the new value
    // that is to be inserted is inserted
    // at a position less than or equal
    // to minimum values required in a node
    if (k <= MIN)
        fillnode(val, c, n, k);
    else
        fillnode(val, c, *newnode, k - mid);

    *y = n->value[n->count];
    (*newnode)->child[0] = n->child[n->count];
    n->count--;
}

// Helper function for del()
int delhelp(int val,
    struct node *root) {
    int i;
    int flag;
    if (root == NULL)
        return 0;
    else {

        // Again searches for the node
        flag = searchnode(val,
            root,
            &i);

        // if flag is true
        if (flag) {
            if (root->child[i - 1]) {
                copysucc(root, i);
                // delhelp() is called recursively
                flag = delhelp(root->value[i],
                    root->child[i]);
                if (!flag) {
                    printf("\n");
                    printf("value %d not found.\n", val);
                }
            } else
                clear(root, i);
        } else {
            // Recursion
            flag = delhelp(val, root->child[i]);
        }

        if (root->child[i] != NULL) {
            if (root->child[i]->count < MIN)
                restore(root, i);
        }
        return flag;
    }
}

// Deletes value from the node
struct node *del(int val,
    struct node *root) {
    struct node *temp;
    if (!delhelp(val, root)) {
        printf("\n");
        printf("value %d not found.\n", val);
    } else {
        if (root->count == 0) {
            temp = root;
            root = root->child[0];
            free(temp);
        }
    }
    return root;
}

// Removes the value from the
// node and adjusts the values
void clear(struct node *m, int k) {
    int i;
    for (i = k + 1; i <= m->count; i++) {
        m->value[i - 1] = m->value[i];
        m->child[i - 1] = m->child[i];
    }
    m->count--;
}

// Copies the successor of the
// value that is to be deleted
void copysucc(struct node *m, int i) {
    struct node *temp;
    temp = m->child[i];
    while (temp->child[0])
        temp = temp->child[0];
    m->value[i] = temp->value[i];
}

// Adjusts the values and children
// while shifting the value from
// parent to right child
void rightshift(struct node *m, int k) {
    int i;
    struct node *temp;

    temp = m->child[k];

    // Copying the nodes
    for (i = temp->count; i > 0; i--) {
        temp->value[i + 1] = temp->value[i];
        temp->child[i + 1] = temp->child[i];
    }
    temp->child[1] = temp->child[0];
    temp->count++;
    temp->value[1] = m->value[k];

    temp = m->child[k - 1];
    m->value[k] = temp->value[temp->count];
    m->child[k]->child[0]
        = temp->child[temp->count];
    temp->count--;
}

// Adjusts the values and children
// while shifting the value from
// parent to left child
void leftshift(struct node *m, int k) {
    int i;
    struct node *temp;

    temp = m->child[k - 1];
    temp->count++;
    temp->value[temp->count] = m->value[k];
    temp->child[temp->count]
        = m->child[k]->child[0];

    temp = m->child[k];
    m->value[k] = temp->value[1];
    temp->child[0] = temp->child[1];
    temp->count--;

    for (i = 1; i <= temp->count; i++) {
        temp->value[i] = temp->value[i + 1];
        temp->child[i] = temp->child[i + 1];
    }
}

// Adjusts the node
void restore(struct node *m, int i) {
    if (i == 0) {
        if (m->child[1]->count > MIN)
            leftshift(m, 1);
        else
            merge(m, 1);
    } else {
        if (i == m->count) {
            if (m->child[i - 1]->count > MIN)
                rightshift(m, i);
            else
                merge(m, i);
        } else {
            if (m->child[i - 1]->count > MIN)
                rightshift(m, i);
            else {
                if (m->child[i + 1]->count > MIN)
                    leftshift(m, i + 1);
                else
                    merge(m, i);
            }
        }
    }
}

// Merges two nodes
void merge(struct node *m, int k) {
    int i;
    struct node *temp1, *temp2;

    temp1 = m->child[k];
    temp2 = m->child[k - 1];
    temp2->count++;
    temp2->value[temp2->count] = m->value[k];
    temp2->child[temp2->count] = m->child[0];

    for (i = 0; i <= temp1->count; i++) {
        temp2->count++;
        temp2->value[temp2->count] = temp1->value[i];
        temp2->child[temp2->count] = temp1->child[i];
    }
    for (i = k; i < m->count; i++) {
        m->value[i] = m->value[i + 1];
        m->child[i] = m->child[i + 1];
    }
    m->count--;
    free(temp1);
}