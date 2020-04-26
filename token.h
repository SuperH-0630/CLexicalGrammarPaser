#ifndef TOKN_INCLUED
#define TOKN_INCLUED

#define MAX_PASER_SIZE 8
#define INT_PASER 0
#define DOUBLE_PASER 1
#define ENTER_PASER 2
#define SPACE_PASER 3
#define ADD_PASER 4
#define SUB_PASER 5
#define MUL_PASER 6
#define DIV_PASER 7

typedef enum token_type
{
    INT = INT_PASER,
    DOUBLE = DOUBLE_PASER,
    ENTER = ENTER_PASER,
    SPACE = SPACE_PASER,
    ADD = ADD_PASER,
    SUB = SUB_PASER,
    MUL = MUL_PASER,
    DIV = DIV_PASER,
} token_type;

typedef union token_data
{
    char *text;
} token_data;


typedef struct token
{
    token_type type;
    token_data *data;
} token;

#endif