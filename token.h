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

// 非终结符
#define NonTerminator -1

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
    BAD_token = -2,
    EOF_token = -3,

    // 终结符
    NON_int = -4,
    NON_dou = -5,
    NON_factor = -6,
    NON_polynomial = -7,
} token_type;

typedef union token_data
{
    char *text;
    int i_number;
    double d_number;
} token_data;


typedef struct token
{
    token_type type;  // token的类型，是数字、变量、关键字、符号
    enum {
        text,
        d_number,
        i_number,
    } data_type;  // data的数据类型
    token_data data;
} token;

// 解析token链表，FILO
typedef struct token_node
{
    int max_index;
    int seek;  // 现在读取的位置，当max_index == seek的时候调用lexical读取token
    int max_size;
    struct token *token_list;  // 记录token的FILO
} token_node;

struct token_node *make_token_node();
void add_node(struct token_node *, struct token);
struct token pop_node(struct token_node *);
void safe_get_token(int *, struct token_node *);


#endif