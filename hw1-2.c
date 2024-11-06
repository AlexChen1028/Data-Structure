#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100 //the maximum of the stack

//stack set
struct Stack {
    char data[MAX];
    int top;
};

//initialize the stack
void initStack(struct Stack *stack) {
    stack->top = -1;
}

//check if the stack is empty
int isEmpty(struct Stack *stack) {
    return stack->top == -1;
}

//get top
char peek(struct Stack *stack) {
    if (!isEmpty(stack)) {
        return stack->data[stack->top];
    }
    return '\0';//if empty, return \0
}

//push
void push(struct Stack *stack, char item) {
    if (stack->top < (MAX - 1)) {//if there still has space to push
        stack->data[++(stack->top)] = item;//change the top and add the character
    }
}

//pop
char pop(struct Stack *stack) {
    if (!isEmpty(stack)) {
        char popped = stack->data[stack->top];
        stack->top--;
        return popped;
    }
    return '\0';//if empty, return \0
}

//check the order of the operators
int precedence(char op) {
    if (op == '+' || op == '-') {
        return 1;
    } else if (op == '*' || op == '/') {
        return 2;
    } else {
        return 0;
    }
}

//check if the character is an operator
int isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

//infix to postfix convertion
char infixToPostfix(char *infix, char *postfix) {
    struct Stack stack;
    initStack(&stack);
    int j = 0;

    for (int i = 0; infix[i] != '\0'; i++) {
        //if it is the alphanumeric, push it into stack
        if (isalnum(infix[i])) {
            push(&stack, infix[i]);
            char popped = pop(&stack);
            postfix[j++] = popped;
        }
        //if the character is an operator
        else if (isOperator(infix[i])) {
            //if the order of current operator is smaller than the top one's, pop it
            while (!isEmpty(&stack) && precedence(peek(&stack)) >= precedence(infix[i])){
                char popped = pop(&stack);
                postfix[j++] = popped;
            }
            //if the order of current operator is bigger than the top one's, push it
            if (!isEmpty(&stack) && (precedence(peek(&stack)) < precedence(infix[i]))) {
                push(&stack, infix[i]);
            }else if (isEmpty(&stack)){//if the stack is empty push the operator
                push(&stack, infix[i]);
            }
        }
    }

    //pop out the rest of operators in the stack
    while (!isEmpty(&stack)) {
        char popped = pop(&stack);
        postfix[j++] = popped;
    }
    postfix[j++] = '\0'; //end up with \0
}

int main() {
    char infixExpr[MAX];
    char postfixExpr[MAX];

    scanf("%s", infixExpr);

    infixToPostfix(infixExpr, postfixExpr);
    printf(postfixExpr);
    return 0;
}