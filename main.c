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
int need_token = true;  // 需要获取新的token还是回退token
token last_token;  // 上一个token

void do_exit(void);
void setup();

token safe_get_token(int *status);
void back_token();

int main(){
    atexit(*do_exit);
    setup();
    int paser_status = 1;  // 设置循环条件
    return 0;
}

void setup(){
    global_paser = login_paser();  // 注册解释器[往global_paser写入数据]
    file_p = fopen("/home/songzihuan/lex.test", "r");  // 打开文件
    debug = fopen("./debug.log", "w");  // 设置debug的位置
}

void do_exit(void){
    free_list(global_paser);  // 释放解释器
    fprintf(debug, "[info]  stop run\n");
}

token safe_get_token(int *status){
    if(need_token){
        last_token = get_token(status);
        fprintf(debug, "[info] get token : type = %d, data.str = '%s'\n\n", last_token.type, last_token.data.text);
        return last_token;
    }
    else{
        need_token = true;
        return last_token;
    }
}

void back_token(){  // 回退一个token
    need_token = false;
}