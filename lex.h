#ifndef WORDH_INCLUDE
#define WORDH_INCLUDE

// 定义lex需要使用的宏
#define NO_MATCH(paser) \
    do{ \
        if((paser->status  == START)){ \
            paser->status = NOTMATCH; \
        } \
        else{ \
            paser->status = END; \
        } \
    } while(0)

#define GET_LEN(paser) \
    int len = 0;\
    do{ \
    if(paser->text != NULL){ \
        len = strlen(paser->text); \
    } \
    } while(0)

#define SET_TEXT(paser) \
    do{ \
    if(paser->status == START){ \
        paser->text = (char *)malloc(sizeof(char)); \
    } \
    else{ \
        paser->text = (char *)realloc(paser->text, sizeof(char) * (len + 1)); \
    } \
    paser->text[len] = p; \
    } while(0)

#define CHECK_END(paser) \
    do{ \
    if(len == (strlen(match) - 1)){ \
        paser->status = WAIT_END; \
    } \
    else{ \
        paser->status = 1; \
    } \
    } while(0)

#define USE paser->status != NOTMATCH && paser->status != END && paser->status != WAIT_END

#define UNUSE_SET \
    else if(paser->status == END){ \
        paser->status = NOTMATCH; \
    } \
    else if(paser->status == WAIT_END){ \
        paser->status = END; \
    }

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


word_paser **login_paser();
void set_start(word_paser **);
void free_list(word_paser **);
char read_p();
int check_list(word_paser **);
int paser(int *);
struct token get_token(int *);

#endif