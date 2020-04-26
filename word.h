#ifndef WORDH_INCLUDE
#define WORDH_INCLUDE

// 定义匹配器的状态
#define STATUS int
#define NOTMATCH -1
#define END -2
#define WAIT_END -3
#define START 0

// 设置mode
#define STEUP 1
#define PASER 0

// 记录匹配器的状态
typedef struct word_paser
{
    STATUS status;
    char *text;
}word_paser;

typedef enum token_type
{
    INT = 1,
    DOUBLE,
    STRING,
} token_type;

typedef struct token
{
    token_type type;
    char *text;
} token;


word_paser **login_paser();
void set_start(word_paser **);
void free_list(word_paser **);
char read_p();
int check_list(word_paser **);
int paser();

#endif