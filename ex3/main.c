//-------------------INCLUDES----------------------------------------------------------------------
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <math.h>
#include "token.h"
#include "stack.h"

//----------------CONSTANTS------------------------------------------------------------------------
/**
 * represents the maximal number of chars in a valid mathematical expression (as string).
 */
#define MAX_LINE_LEN 100

//----Error syntax constants:
#define MEM_SEG_ERR "Error in memory allocation.\n"
#define DIV_BY_ZERO_ERR "Division by 0!\n"

//------------------------HELPERS------------------------------------------------------------------


//--------------general functionality:
/**
 * prints a mathematical expression (given as dynamic array of Token objects).
 * @param header: prefix string to declare the printing purpose.
 * @param exp: the dynamic array.
 * @param size: the size of the array.
 */
void printExp(char *header, Token **exp, const size_t size)
{
    printf("%s:", header);
    for(size_t idx = 0; idx < size; ++idx)
    {
        printData(exp[idx]);
    }
    printf("\n");
    fflush(stdout);
}

/**
 * deletes a dynamic array of tokens
 * @param tokens : a dynamic array of tokens.
 * @param size : the size of the array.
 */
void deleteArrayOfTokens(Token **tokens, const size_t size)
{
    for (int idx = 0; idx < size; ++idx)
    {
        freeToken(tokens[idx]);
        tokens[idx] = NULL;
    }
    free(tokens);
    tokens = NULL;
}

//--------------expression to Infix form conversion:


/**
 * @param c: char that is (, ), + , -, /, *, ^ or a digit! (assumed in ex. clarifications)
 * @return: enum ArithmeticTokens object that represents the type of <c>.
 */
const enum ArithmeticTokens parseType(const char c)
{
    switch (c)
    {
        case '(':
            return Left_Parenthesis;
        case ')':
            return Right_Parenthesis;
        case '+':
        case '-':
        case '/':
        case '*':
        case '^':
            return Operator;
        default:
            return Operand;
    }
}

/**
 * converts a mathematical expression given in infix form as string,
 * to an array of tokens (see token.h).
 * @param exp: string that holds a valid mathematical expression.
 *             (i.e: valid parenthesis alignment, and valid use of the operators: +, -, *, \, ^).
 *             no other chars but digit exists in the expression.
 * @param size: address of a counter to count the number of elements in list.
 * @return dynamic array that holds Token objects,
 *         representing the mathematical expression supplied (in infix form).
 */
Token **expToInfix(char *exp, size_t *size)
{
    //preparations:
    Token **tokens = NULL;
    Token *newToken;
    enum ArithmeticTokens newType;
    char *lp = exp, *rp = exp;

    while (*lp != '\0' && *lp != '\n')
    {
        //get the "substring" (by pointers) that holds the data
        ++rp;
        if (isdigit(*lp))
        {
            while (isdigit(*rp))
            {
                ++rp;
            }
        }
        //initializations newToken
        newType = parseType(*lp);
        newToken = initializeToken(lp, rp - lp, newType);
        // put newToken in tokens
        tokens = realloc(tokens, sizeof(Token*) * (*size + 1));
        if(tokens == NULL)
        {
            fprintf(stderr, "%s", MEM_SEG_ERR);
            exit(EXIT_FAILURE);
        }
        tokens[*size] = newToken;
        //next iter:
        ++(*size);
        lp = rp;
    }
    return tokens;
}

//-----------------Infix form to Postfix form conversion:

/**
 * @param operatorChar: char that is  '+' , '-', '/', '*', '^'.
 * @return: an int that represents the precedence of the operator supplied.
 *          if the supplied char is not an operator returns -1.
 */
const int precedence(const char operatorChar)
{
    switch (operatorChar)
    {
        case '+':
        case '-':
            return 1;

        case '*':
        case '/':
            return 2;

        case '^':
            return 3;
        default:
            return -1;
    }
}
/**
 * appends the supplied <currToken> to <postfix> at <size>.
 * @param postfix : a dynamic array of Token objects.
 * @param newToken : a new Token object.
 * @param size : the size of <postfix> before the append.
 */
void appendToPostfix(Token **postfix, Token *const newToken, size_t *const size)
{
    postfix = (Token **)realloc(postfix, sizeof(Token*) * (*size + 1));
    postfix[*size] = newToken;
    ++(*size);
}

/**
 * Pops the stack and appends the popped value to postfix.
 * @param postfixStack: the stack that we'll pop.
 * @param postfix : a dynamic array of Token objects.
 * @param size : the size of <postfix> before the append.
 */

void popAndAppendToPostfix(Stack *postfixStack, Token **postfix, size_t *size)
{
    char *headData = (char*)malloc(sizeof(char) + 1);
    pop(postfixStack, headData);
    appendToPostfix(postfix, initializeToken(headData, strlen(headData), Operator), size);

}


/**
 * converts a mathematical expression given as an array of tokens in infix form,
 * to an array of tokens in postfix form. returns this new array.
 * @param infix: dynamic array that holds Token objects,
 *               representing mathematical expression in infix form.
 * @param inSize: the num of Token objects in the supplied <infix> array.
 * @param size: address of a counter to count the number of elements in the list we're returning.
 * @return dynamic array that holds Token objects,
 *         representing the mathematical expression supplied (in postfix form).
 */
Token **infixToPostfix(Token **infix, const size_t inSize, size_t *const size)
{
    //preparations:
    Token **postfix = NULL;
    Stack *postfixStack = stackAlloc(sizeof(char));
    char *headData = malloc(sizeof(char) + 1);
    char *toFree;

    for(size_t idx = 0; idx < inSize; ++idx)
    {
        const Token *const currToken = infix[idx];
        enum ArithmeticTokens currType = getType(currToken);
        switch (currType)
        {
            case Operand:
                postfix = (Token **)realloc(postfix, sizeof(Token*) * (*size + 1));
                postfix[*size] = cloneToken(currToken);
                ++(*size);
                break;

            case  Left_Parenthesis:
                push(postfixStack, (void *)getData(currToken));
                break;

            case Right_Parenthesis:
                while (!isEmptyStack(postfixStack))
                {
                    if(strcmp(toFree = peek(postfixStack), "(") == 0)//pop left parenthesis:
                    {
                        pop(postfixStack, headData);
                        break;
                    }
                    pop(postfixStack, headData);
                    postfix = (Token **)realloc(postfix, sizeof(Token*) * (*size + 1));
                    postfix[*size] = initializeToken(headData, 1, Operator);
                    ++(*size);
                    free(toFree);
                }
                break;

            case Operator:
                if (isEmptyStack(postfixStack) || strcmp(toFree = peek(postfixStack), "(") == 0)
                {
                    push(postfixStack, (void *)getData(currToken));
                }
                else
                {
                    while (!isEmptyStack(postfixStack) && strcmp(headData, "(")  != 0 &&\
                           precedence(getData(currToken)[0]) <= precedence(headData[0]))
                    {
                        pop(postfixStack, headData);
                        postfix = (Token **)realloc(postfix, sizeof(Token*) * (*size + 1));
                        postfix[*size] = initializeToken(headData, 1, Operator);
                        ++(*size);
                    }
                    push(postfixStack, (void *)getData(currToken));
                }
                free(toFree);
                toFree = NULL;
                break;
        }
    }
    while (!isEmptyStack(postfixStack))
    {
        pop(postfixStack, headData);
        postfix = (Token **)realloc(postfix, sizeof(Token*) * (*size + 1));
        postfix[*size] = initializeToken(headData, 1, Operator);
        ++(*size);
    }
    //restore memory
    freeStack(&postfixStack);
    free(headData);
    return postfix;
}

//-----------------Postfix expression evaluation:
/**
 * @param a: address of an int
 * @param b: address of an int
 * @return a + b.
 */
int add(const int *const a, const int *const b)
{
    return (*a) + (*b);
}

/**
 * @param a: address of an int
 * @param b: address of an int
 * @return a - b.
 */
int sub(const int *const a, const int *const b)
{
    return (*a) - (*b);
}

/**
 * @param a: address of an int
 * @param b: address of an int
 * @return a * b
 */
int mul(const int *const a, const int *const b)
{
    return (*a) * (*b);
}

/**
 * @param a: address of an int
 * @param b: address of an int
 * @return a / b.
 *         if b is 0:  throws an error and exits.
 */
int divide(const int *const a, const int *const b)
{
    if(*b == 0)
    {
        fprintf(stderr, "%s", DIV_BY_ZERO_ERR);
        exit(EXIT_FAILURE);
    }
    return (*a) / (*b);
}

/**
 *
 * @param a: address of an int
 * @param b: address of an int
 * @return: a ^ b
 */
int power(const int *const a, const int *const b)
{
    return (int) pow((*a), (*b));
}

/**
 * @param a: address of an int
 * @param b: address of an int
 * @param f: a function.
 * @return f(a,b)
 */
int computeExp(const int *const a, const int *const b, \
               int (*f)(const int *const, const int *const))
{
    return f(a, b);
}

/**
 * @param a: address of an int.
 * @param b : address of an int.
 * @param op : char that is one of the followings: '+', '-', '*', '/', '^'.
 * @return : the result of: <op>(<a>,<b>).
 */
int computeRes(const int *const a, const int *const b, const char op)
{
    int res;
    switch (op)
    {
        case '+':
            res = computeExp(a, b, add);
            break;
        case '-':
            res = computeExp(a, b, sub);
            break;
        case '*':
            res = computeExp(a, b, mul);
            break;
        case '/':
            res = computeExp(a, b, divide);
            break;
        case '^':
            res = computeExp(a, b, power);
            break;
        default:
            res = 0;
    }
    return res;
}



/**
 * evaluates the expression supplied in <postfix>, and returns the result.
 * @param postfix: dynamic array that holds Token objects,
 *               representing mathematical expression in postfix form.
 * @param size: the number of elements in <postfix>.
 * @return the evaluation's result.
 */
int evaluateExp(Token **postfix, const size_t size)
{
    //preparations:
    Stack *evalStack = stackAlloc(sizeof(int));
    int *aPtr = (int *)malloc(sizeof(int) + 1);
    int *bPtr = (int *)malloc(sizeof(int) + 1);

    for(size_t idx = 0; idx < size; ++idx)
    {
        const Token *currToken = postfix[idx];
        const enum ArithmeticTokens currType = getType(currToken);
        if(currType ==  Operand)
        {
            //pushes to stack as int
            char *ptr;
            int operand = (int)strtol(getData(currToken), &ptr, 10);
            push(evalStack, (void *)&operand);
        }
        else if (currType == Operator)
        {
            pop(evalStack, bPtr);
            pop(evalStack, aPtr);
            int res = computeRes(aPtr, bPtr, *getData(currToken));
            push(evalStack, (void *)&res);
        }
    }
    pop(evalStack, aPtr);
    int res = *aPtr;
    //restore memory:
    freeStack(&evalStack);
    free(aPtr);
    free(bPtr);
    return res;
}


//------------------------RUN THE PROGRAM----------------------------------------------------------

/**
 * runs the program.
 * @return: 0 i succeed, 1 otherwise.
 */
int main()
{
    char buff[MAX_LINE_LEN + 1]; //including '\n'
    // reads infix expression as string from user:
    char *result = fgets(buff, MAX_LINE_LEN + 1, stdin);
    while (result != NULL)
    {
        //translates the exp to infix expression & prints exp in infix form:
        size_t inSize = 0;
        Token **infix = expToInfix(result, &inSize);
        printExp("infix", infix, inSize);

        //translates from infix to postfix & prints the exp in postfix form:
        size_t postSize = 0;
        Token** postfix = infixToPostfix(infix, inSize, &postSize);
        printExp("postfix", postfix, postSize);

        deleteArrayOfTokens(infix, inSize); // restore memory

        //evaluate the exp in postfix form & print the evaluation:
        int evaluation = evaluateExp(postfix, postSize);
        printf("The value is %d\n", evaluation);

        deleteArrayOfTokens(postfix, postSize); //restore memory

        //reads next line from user:
        result = fgets(buff, MAX_LINE_LEN + 1, stdin);
    }
    return 0;
}