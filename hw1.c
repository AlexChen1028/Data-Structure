#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//stack node set
struct StackNode {
    char *data;
    struct StackNode *next;
};

//stack operation
struct StackNode* createNode(char *data) {
    struct StackNode *node = (struct StackNode*)malloc(sizeof(struct StackNode));
    node->data = data;//initial setting
    node->next = NULL;//initial setting
    return node;
} 

void push(struct StackNode **top, char *data) {
    struct StackNode *node = createNode(data);
    node->next = *top;//set the value of the old top to be the next 
    *top = node;
}

char* pop(struct StackNode **top) {
    if (*top == NULL) return NULL;
    struct StackNode *temp = *top;
    *top = (*top)->next;
    char *popped = temp->data;
    free(temp);
    return popped;
}

int isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

//prefix to infix convertion
char* prefixToInfix(char *prefix) {
    int length = strlen(prefix);
    struct StackNode *stack = NULL;

    for (int i = length - 1; i >= 0; i--) {
        //if it is the alphanumeric, push it into stack
        if (isalnum(prefix[i])) {
            char *operand = (char*)malloc(2 * sizeof(char));
            operand[0] = prefix[i];
            operand[1] = '\0';
            push(&stack, operand);
        }
        //if it is an operator, the two stacked operands are popped out and formed an infix expression
        else if (isOperator(prefix[i])) {
            char *operand1 = pop(&stack);
            char *operand2 = pop(&stack);
            int size = strlen(operand1) + strlen(operand2) + 4;
            char *infix = (char*)malloc(size * sizeof(char));
            snprintf(infix, size, "(%s%c%s)", operand1, prefix[i], operand2);
            push(&stack, infix);
            free(operand1);
            free(operand2);
        }
    }
    char *result = pop(&stack);
    return result;
}

void printWithoutParentheses(char *input) {
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] != '(' && input[i] != ')') {
            putchar(input[i]); //output
        }
    }
    putchar('\n'); //change line
}

int main() {
    char prefixExpr[100];
    scanf("%s", prefixExpr);
    char *infixExpr = prefixToInfix(prefixExpr);
    printWithoutParentheses(infixExpr);//deal with the parentheses and print the result
    free(infixExpr);//delete
    return 0;
}