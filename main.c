#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "word.h"

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

#define MAX_PASER_SIZE 4
#define INT_PASER 0
#define DOUBLE_PASER 1
#define WHILE_PASER 2
#define ENTER_PASER 3

word_paser **global_paser;
char *file_address = "/home/songzihuan/lex.test";
static FILE *file_p = NULL;

void match_int(char, word_paser *);
void match_double(char, word_paser *);
void match_while(char, word_paser *);
void match_enter(char, word_paser *);

int main(){
    global_paser = login_paser();  // 注册解释器
    file_p = fopen(file_address, "r");

    int paser_status = 1;
    do{
        paser_status = paser();
        set_start(global_paser);
    }while(paser_status);

    free_list(global_paser);  // 释放解释器
    return 0;
}

int paser(){
    char p;
    int status = 1;
    int is_eof = 0;
    while(1){
        p = read_p();
        if(p == EOF){  // 还没有解析完成就结束了
            printf("p = 'EOF'\n");
            is_eof = 1;
        }
        else{
            if(p == '\n'){
                printf("p = <ENTER>\n");
            }
            else{
                printf("p = '");
                putchar(p);
                puts("'");
            }
        }

        match_int(p, global_paser[INT_PASER]);
        match_double(p, global_paser[DOUBLE_PASER]);
        match_while(p, global_paser[WHILE_PASER]);
        match_enter(p, global_paser[ENTER_PASER]);
        int index = check_list(global_paser);
        if(is_eof){
            status = 0;
        }
        if(index >= 0){
            printf("=============================== get value = '%s' from %d\n", global_paser[index]->text, index);
            break;
        }
        else if(index == -2){
            printf("Paser Wrong!\n");
            break;
        }
        else if(is_eof){
            break;
        }
    }
    return status;
}

char read_p(){
    return getc(file_p);
}

void back_p(){  // 回退
    fseek(file_p, -1, 1);
}

word_paser **login_paser(){
    word_paser **paser_list = malloc(sizeof(word_paser *) * MAX_PASER_SIZE);  // 指针数组的指针
    for(int i = 0;i < MAX_PASER_SIZE;i += 1){
        paser_list[i] = malloc(sizeof(word_paser));
        paser_list[i]->status = START;
        paser_list[i]->text = NULL;
    }
    return paser_list;
}

void set_start(word_paser **paser_list){
    for(int i = 0;i < MAX_PASER_SIZE;i += 1){
        paser_list[i]->status = START;
        paser_list[i]->text = NULL;
    }
}

void free_list(word_paser **paser_list){
    for(int i = 0;i < MAX_PASER_SIZE;i += 1){
        if(paser_list[i]->text != NULL){
            free(paser_list[i]->text);
            paser_list[i]->text = NULL;
        }
        free(paser_list[i]);
    }
    free(paser_list);
}

int check_list(word_paser **paser_list){
    int end_count = 0;
    int not_count = 0;
    int end_index = 0;  // the last end index
    for(int i = 0;i < MAX_PASER_SIZE;i += 1){
        if(paser_list[i]->status == END){  // 统计END的次数
            end_count += 1;
            end_index = i;
        }
        if(paser_list[i]->status == NOTMATCH){  // 统计END的次数
            not_count += 1;
        }
        printf("paser_list[%d]->status = %d\n", i, paser_list[i]->status);
    }
    printf("end_count = %d\n", end_count);

    // 需要往回放一个字符
    if((MAX_PASER_SIZE - not_count - end_count) == 0 && end_count == 1){  // 除了不匹配就是匹配成功，且只有一个成功
        back_p();
        return end_index;
    }
    if(MAX_PASER_SIZE == not_count){  // 全部匹配不正确，没有一个成功
        return -2;  // 错误
    }
    else{
        return -1;  // 需要继续匹配
    }
}

void match_int(char p, word_paser *paser){  // 匹配一个int
    if(USE){
        GET_LEN(paser);
        if(p <= '9' && p >= '0'){  // 是数字
            SET_TEXT(paser);
            paser->status = 1;
        }
        else{
            NO_MATCH(paser);
        }
    }
    UNUSE_SET;
}

void match_double(char p, word_paser *paser){  // 匹配一个int
    if(USE){
        GET_LEN(paser);
        if((p <= '9' && p >= '0') || (p == '.' && paser->status == 1)){  // 是数字
            if(paser->status == START){
                paser->text = (char *)malloc(sizeof(char));
                paser->status = 1;
            }
            else{
                paser->text = (char *)realloc(paser->text, sizeof(char) * (strlen(paser->text) + 1));
                if(p == '.'){
                    paser->status = 2; 
                }
            }
            paser->text[len] = p;
        }
        else{
            if(paser->status == 2){  // 必须进入小数模式才可以
                paser->status = END;
            }
            else{  // 没有匹配上
                paser->status = NOTMATCH;
            }
        }
    }
    UNUSE_SET;
}

void match_while(char p, word_paser *paser){  // 匹配一个while
    char *match = "while";  // 待匹配字符串
    // printf("[while] paser->status = %d\n", paser->status);
    if(USE){
        GET_LEN(paser);  // 设置len并且处理NULL
        // printf("[while] = '");
        // putchar(p);
        // printf("',  '");
        // putchar(match[len]);
        // printf("' len = %d\n", len);
        if(p == match[len]){  // 匹配成功
            SET_TEXT(paser);  // 设置text
            CHECK_END(paser);  // 设置是否完全匹配
        }
        else{
            // 匹配不成功处理
            NO_MATCH(paser);  // 如果是START模式则设置为不匹配，否则设置为匹配结束
        }
    }
    UNUSE_SET;
}

void match_enter(char p, word_paser *paser){  // 匹配一个while
    char *match = "\n";  // 待匹配字符串
    if(USE){
        GET_LEN(paser);  // 设置len并且处理NULL
        if(p == match[len]){  // 匹配成功
            SET_TEXT(paser);  // 设置text
            CHECK_END(paser);  // 设置是否完全匹配
        }
        else{
            // 匹配不成功处理
            NO_MATCH(paser);  // 如果是START模式则设置为不匹配，否则设置为匹配结束
        }
    }
    UNUSE_SET;
}