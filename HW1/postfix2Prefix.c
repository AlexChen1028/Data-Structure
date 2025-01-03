#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct StackNode {
    char *data;
    struct StackNode *next;
} StackNode;

void push(StackNode **top, char *data) {
    StackNode *newNode = malloc(sizeof(StackNode));
    newNode->data = data;
    newNode->next = *top;
    *top = newNode;
}

char *pop(StackNode **top) {
    if (*top == NULL) {
        return NULL;
    }
    StackNode *temp = *top;
    char *data = temp->data;
    *top = temp->next;
    free(temp);
    return data;
}

int isOperator(char c) {
    return (c == '*' || c == '/' || c == '+' || c == '-');
}

char *postfix2Prefix(char *postfix) {
    StackNode *top = NULL;

    for (int i = 0;i < strlen(postfix);i++) {
        char current = postfix[i];

        if (isOperator(current)) {
            char *secondOperand = pop(&top);
            char *firstOperand = pop(&top);

            char *expression = malloc(strlen(firstOperand) + strlen(secondOperand) + 2);
            sprintf(expression, "%c%s%s", current, firstOperand, secondOperand);

            push(&top, expression);
        } else {
            char *operand = malloc(2);
            operand[0] = current;
            operand[1] = '\0';
            push(&top, operand);
        }
    }

    char *prefix = pop(&top);
    return prefix;
}

int main() {
    char postfix[] = "ABC*+DE/-F+";
    char *prefix = postfix2Prefix(postfix);
    printf("Prefix: %s\n", prefix);
    free(prefix);
    return 0;
}
