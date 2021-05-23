#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#define MEM_SEG_ERR "Error in memory allocation.\n"

/**
 * represents a node with the fields:
 * data: address to the data this Node holds. (void * enables generics)
 * next: the address of the next Node.
 */
typedef struct Node
{
  void * _data; // pointer to anything
  struct Node * _next;
} Node;

/**
 * represents a Stack with the fields:
 * top: address of the top Node.
 * element size: size of the elements this stack holds. (enables generics)
 */
typedef struct Stack
{
  Node * _top;
  size_t _elementSize;    // we need that for memcpy
} Stack;

/**
 * creates a new Stack object
 * @param elementSize : sizeof() the elements that this stack holds
 * @return : the stack
 */
Stack* stackAlloc(size_t elementSize);

/**
 * deletes the stack & frees the memory
 * @param stack: a Stack object
 */
void freeStack(Stack** stack);

/**
 * pushes the supplied data onto the given stack
 * @param stack : a Stack object
 * @param data : address of a data var with type consistent in size to _elementSize
 */
void push(Stack* stack, void *data);

/**
 * pops the top of the stack & keeps the popped value in headData(allocated var).
 * if the stack us empty, prints an error and returns, headData is not changed.
 * @param stack : the stack object
 * @param headData : allocated var in size consistent to _element size.
 */
void pop(Stack* stack, void *headData);

/**
 * @param stack : stack object
 * @return 0 if the stack isn't empty, 0 otherwise.
 */
int isEmptyStack(Stack* stack);

/**
 * @param stack a Stack object.
 * @return pointer containing the stack's top data.
 * notice! free this after calling.
 */
void* peek(Stack *stack);

#endif
