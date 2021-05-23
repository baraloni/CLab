#ifndef TOKEN_H
#define TOKEN_H
#include <stddef.h>
#define MEM_SEG_ERR "Error in memory allocation.\n"

/**
 * holds the types of the arithmetical token.
 */
typedef enum ArithmeticTokens
{    Operand = 1,
     Operator = 2,
     Left_Parenthesis = 3,
     Right_Parenthesis = 4
}ArithmeticTokens;

/**
 * a structure that represents an arithmetic token which has 2 fields:
 *    data: a string that holds the token's data.
 *    type: an Arithmetic Token the holds the token's type.
 */
typedef struct token
{
    char *_data;
    enum  ArithmeticTokens _type;
}Token;

/**
 * initializes a token, and returns it.
 * @param data : the data that the new token will hold.
 * @param size: number of chars to take from data.
 * @param type : the type that the new token will hold.
 * @return : the new initialized token.
 */
Token *initializeToken(char *data, size_t size, enum ArithmeticTokens type);

/**
 * creates a new Token object from another one (without changing the other's attributes).
 * @param other: Token object
 * @return : the new Token object whose fields are identical to <other>'s fields.
 */
 Token *const cloneToken(const Token* other);

/**
 * deletes the supplied token.
 * notice: the class frees the token's data although it did not assigned memory to hold it.
 * @param token: the address of the Token object.
 */
void freeToken(Token *token);

/**
 * prints the token's data
 * @param token
 */
void printData(const Token* token);

/**
 * @param token : an address of a Token object.
 * @return: the given token's type.
 */
const enum ArithmeticTokens getType(const Token* token);
/**
 * @param token : an address of a Token object.
 * @return : the given token's data.
 */
const char* getData(const Token* token);


#endif //TOKEN_H
