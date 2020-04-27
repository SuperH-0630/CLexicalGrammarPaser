#ifndef TOKN_INCLUED
#define TOKN_INCLUED

#define bool int
#define true 1
#define false 0

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
    token_type type;  // token的类型，是数字、变量、关键字、符号
    enum {
        text,
        eof,  // 表示结束符
    } data_type;  // data的数据类型
    token_data data;
} token;

#endif