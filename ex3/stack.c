#include "stack.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

/**
 * creates a new Stack object
 * @param elementSize : sizeof() the elements that this stack holds
 * @return : the stack
 */
Stack* stackAlloc(size_t elementSize)
{
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->_top = NULL;
    stack->_elementSize = elementSize;
    return stack;
}

/**
 * deletes the stack & frees the memory
 * @param stack: a Stack object
 */
void freeStack(Stack** stack)
{
    Node* p1;
    Node* p2;
    if (*stack != NULL)
    {
        p1 = (*stack)->_top;
        while(p1)
        {
            p2 = p1;
            p1 = p1->_next;
            free(p2->_data);
            free(p2);
        }
        free(*stack);
        *stack = NULL;
    }
}

/**
 * pushes the supplied data onto the given stack
 * @param stack : a Stack object
 * @param data : address of a data var with type consistent in size to _elementSize
 */
void push(Stack* stack, void *data)
{
    Node *node = (Node *) malloc(sizeof(Node));
    if (node != NULL)
    {
        node->_data = malloc(stack->_elementSize);
        if (node->_data != NULL)
        {
            memcpy(node->_data, data, stack->_elementSize);
            node->_next = stack->_top;
            stack->_top = node;
        }
    }
    printf("%s", MEM_SEG_ERR);
}

/**
 * pops the top of the stack & keeps the popped value in headData(allocated var).
 * if the stack us empty, prints an error and returns, headData is not changed.
 * @param stack : the stack object
 * @param headData : allocated var in size consistent to _element size.
 */
void pop(Stack* stack, void *headData) 
{
    assert(stack != NULL);
    if(stack->_top == NULL)
    {
        fprintf(stderr, "The stack is empty\n");
        return;
    }
    Node *node = stack->_top;
    memcpy(headData, node->_data, stack->_elementSize);
    stack->_top = node->_next;
    free(node->_data);
    free(node);
}

/**
 * @param stack : stack object
 * @return 0 if the stack isn't empty, 0 otherwise.
 */
int isEmptyStack(Stack* stack) 
{
    assert(stack != NULL);
    return stack->_top == NULL;
}

/**
 * @param stack a Stack object.
 * @return pointer containing the stack's top data.
 * notice! free this after calling.
 */
void* peek(Stack *stack)
{
    assert(stack != NULL);
    if(stack->_top != NULL)
    {
        Node *node = stack->_top;
        char *topData = (char *)malloc(stack->_elementSize + 1);
        memcpy(topData, node->_data, stack->_elementSize);
        return topData;
    }
    fprintf(stderr, "The stack is empty\n");
    return NULL;
}

