#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "paser.h"
#include "token.h"
#include "mem.c"
#include "lexical.c"

#define malloc(size) safe_malloc(size)
#define free(p) p=safe_free(p)
#define realloc(p,size) safe_realloc(p,size)
#define memcpy(p1,p2,size) safe_memcpy(p1,p2,size)

int paser_status = 1;
token_node *global_token_node = NULL;

void do_exit(void);
void setup();

void back_token();

void factor(int *status, token_node *list);
void number(int *status, token_node *list);

int main(){
    atexit(*do_exit);
    setup();
    safe_get_token(&paser_status, global_token_node);
    factor(&paser_status, global_token_node);
    return 0;
}

void setup(){
    global_paser = login_paser();  // 注册解释器[往global_paser写入数据]
    file_p = fopen("/home/songzihuan/lex.test", "r");  // 打开文件
    debug = fopen("./debug.log", "w");  // 设置debug的位置
    global_token_node = make_token_node();
}

void do_exit(void){
    free_list(global_paser);  // 释放解释器
    fprintf(debug, "[info][paser]  stop run\n");
}

token_node *make_token_node(){
    token_node *tmp = (token_node *)malloc(sizeof(tmp));
    tmp->max_index = 0;
    tmp->seek = 0;
    tmp->max_size = 1;
    tmp->token_list = malloc(sizeof(token) * tmp->max_size);
    return tmp;
}

void add_node(token_node *list, token the_token){
    // printf("max_size = %d, max_index = %d\n", list->max_size, list->max_index);
    if(list->max_size - list->max_index <= 1){
        list->max_size += 1;
        token *tmp = malloc(sizeof(token) * list->max_size);
        memcpy(tmp, list->token_list, sizeof(token) * (list->max_size));
        free(list->token_list);
        list->token_list = tmp;
    }

    list->token_list[list->max_index] = the_token;
    list->max_index += 1;
    list->seek += 1;
}

token pop_node(token_node *list){  // 弹出一个node
    list->max_index -= 1;
    list->seek -= 1;
    return list->token_list[list->max_index];
}

void safe_get_token(int *status, token_node *list){
    token tmp = get_token(status);
    fprintf(debug, "[debug][grammar]  get token type = %d\n\n", tmp.type);
    add_node(list, tmp);
}

/*
factor : number
       | factor MUL number
       | factor DIV number
*/
void factor(int *status, token_node *list){  // 因试分解
    token left, right, symbol, new_token;

    left = pop_node(list);  // 先弹出一个token   检查token的类型：区分是模式1,还是模式2/3
    if(left.type == NON_factor){  // 模式2/3
        safe_get_token(status, list);  // 获取一个token并压入栈
        symbol = pop_node(list);  // 获取一个token

        if(symbol.type == MUL_PASER || symbol.type == DIV_PASER){  // 模式2/3
            safe_get_token(status, list);
            number(status, list);
            right = pop_node(list);

            new_token.type = NON_factor;
            new_token.data_type = d_number;
            double l_num = left.data.d_number, r_num;
            if(right.type == NON_dou){
                r_num = right.data.d_number;
            }
            else if(right.type == NON_int){
                r_num = (double)right.data.i_number;
            }
            if(symbol.type == MUL_PASER){
                new_token.data.d_number = l_num * r_num;
            }
            else{
                new_token.data.d_number = l_num / r_num;
            }
            add_node(list, new_token);  // 压入节点[弹出3个压入1个]
            return factor(status, list);  // 回调自己
        }
        else{  // 递归跳出
            printf("left.data.i_number = %f\n", left.data.d_number);
            add_node(list, left);
            add_node(list, symbol);
            return;
        }
    }
    else{  // 模式1
        add_node(list, left);  // 先压入
        number(status, list);
        new_token = pop_node(list);
        if(new_token.type == NON_int){
            new_token.data.d_number = (double)new_token.data.i_number;
        }
        new_token.type = NON_factor;
        add_node(list, new_token);
        return factor(status, list);  // 回调自己
    }
}


void number(int *status, token_node *list){  // 数字归约器
    token gett, new_token;

    gett = pop_node(list);  // 取得一个token

    if(gett.type == INT_PASER){  // int类型
        new_token.type = NON_int;
        new_token.data_type = i_number;
        new_token.data.i_number = atoi(gett.data.text);
        free(gett.data.text);  // 释放字符串
    }
    else if(gett.type == DOUBLE_PASER){  // int类型
        new_token.type = NON_dou;
        new_token.data_type = d_number;
        new_token.data.i_number = atof(gett.data.text);
        free(gett.data.text);  // 释放字符串
    }
    else{  // 不是期望值
        add_node(list, gett);  // 回退一个token
        new_token.type = BAD_token;
        new_token.data_type = i_number;
        new_token.data.i_number = 0;
        return;
    }
    add_node(list, new_token);  // 压入节点
}