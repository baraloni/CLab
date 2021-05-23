#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/**
 * initializes a token, and returns it.
 * @param data : the data that the new token will hold.
 * @param size: number of chars to take from data.
 * @param type : the type that the new token will hold.
 * @return : the new initialized token.
 */
 Token *initializeToken(char *data, size_t size, enum ArithmeticTokens type)
{
    Token *newToken = (Token *) malloc(sizeof(Token));
    if (newToken != NULL)
    {
        char *newData = (char *) malloc(size + 1);
        if (newData != NULL)
        {
            newData = memcpy(newData, data, size);
            newData[size] = '\0';
            newToken->_data = newData;
            newToken->_type = type;
            return (newToken);
        }
    }
    printf("%s", MEM_SEG_ERR);
    return NULL;
}

/**
 * creates a new Token object from another one (without changing the other's attributes).
 * @param other: Token object
 * @return : the new Token object whose fields are identical to <other>'s fields.
 */
 Token *cloneToken(const Token* other)
{
    return initializeToken(other->_data, strlen(other->_data), other->_type);
}

/**
 * deletes the supplied token.
 * notice: the class frees the token's data although it did not assigned memory to hold it.
 * @param token: the address of the Token object.
 */
void freeToken(Token *token)
{
    free(token->_data);
    free(token);
}

/**
 * prints the token's data
 * @param token
 */
void printData(const Token* token)
{
    (token->_type == 1) ? printf(" %s ", token->_data) : printf("%s", token->_data);
}

/**
 * @param token : an address of a Token object.
 * @return: the given token's type.
 */
const enum ArithmeticTokens getType(const Token* token)
{
    return token->_type;
}
/**
 * @param token : an address of a Token object.
 * @return : the given token's data.
 */
const char* getData(const Token* token)
{
    return token->_data;
}
