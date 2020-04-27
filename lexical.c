#include "lex.h"
#include "token.h"

#define malloc(size) safe_malloc(size)
#define free(p) p=safe_free(p)
#define realloc(p,size) safe_realloc(p,size)
#define memcpy(p1,p2,size) safe_memcpy(p1,p2,size)

void match_int(char, word_paser *);
void match_double(char, word_paser *);
void match_text(char, word_paser *, char *);

token get_token(int *paser_status){
    token return_token;
    int index = 0;  // 记录index和是否遇到了EOF
    *paser_status = paser(&index);  // 解析

    return_token.type = index;
    return_token.data_type = text;
    return_token.data.text = malloc(strlen(global_paser[index]->text));
    strcpy(return_token.data.text, global_paser[index]->text);
    set_start(global_paser);
    fprintf(debug, "[debug]token type = %d\n\n", index);
    return return_token;
}

int paser(int *index){
    char p;
    int status = 1;
    int is_eof = 0;
    while(1){
        p = read_p();
        if(p == EOF){  // 遇到EOF[先不break]
            fprintf(debug, "[info][lexical]  p = <EOF>\n\n");
            status = 0;
            is_eof = 1;
        }
        else{
            if(p == '\n'){
               fprintf(debug, "[info][lexical]  p = <Enter>\n\n"); 
            }
            else{
                fprintf(debug, "[info][lexical]  p = '"); 
                fputc(p, debug); 
                fputs("'\n\n", debug); 
            }
        }

        // 执行解析器
        match_int(p, global_paser[INT_PASER]);
        match_double(p, global_paser[DOUBLE_PASER]);
        match_text(p, global_paser[ENTER_PASER], "\n");
        match_text(p, global_paser[SPACE_PASER], " ");
        match_text(p, global_paser[ADD_PASER], "+");
        match_text(p, global_paser[SUB_PASER], "-");
        match_text(p, global_paser[MUL_PASER], "*");
        match_text(p, global_paser[DIV_PASER], "/");

        *index = check_list(global_paser);  // 检查解析结果

        if(*index >= 0){  // index >= 0表示找到解析的结果[存在一个解析器存在结果，其他解析器没有结果]
            fprintf(debug, "[info][lexical]  get value = '%s' len = %d from %d\n\n", global_paser[*index]->text, strlen(global_paser[*index]->text),*index);
            break;
        }
        else if(*index == -2){  // -2表示全部解析器没有结果
            fprintf(debug, "[error][lexical]  Paser Wrong!\n\n");
            break;
        }
        else if(is_eof){  // 以上状况均不是，如果是eof仍要推出
            fprintf(debug, "[error][lexical]  EOF Paser Wrong!\n\n");
            break;
        }
        else{
            fprintf(debug, "[debug][lexical]  continue to paser\n\n");
        }
        // else情况：继续匹配
    }
    return status;
}

char read_p(){  // 读取一个字符
    return getc(file_p);
}

void back_p(){  // 回退一个字符
    fseek(file_p, -1, 1);
    fprintf(debug, "[info][lexical]  back_p\n\n");
}

word_paser **login_paser(){
    word_paser **paser_list = malloc(sizeof(word_paser *) * MAX_PASER_SIZE);  // 指针数组的指针
    for(int i = 0;i < MAX_PASER_SIZE;i += 1){  // 申请内存并初始化
        paser_list[i] = malloc(sizeof(word_paser));
        paser_list[i]->status = START;
        paser_list[i]->text = NULL;
    }
    return paser_list;
}

void set_start(word_paser **paser_list){  // 初始化
    for(int i = 0;i < MAX_PASER_SIZE;i += 1){
        paser_list[i]->status = START;
        if(paser_list[i]->text != NULL){
            free(paser_list[i]->text);
        }
        paser_list[i]->text = NULL;
    }
    fprintf(debug, "[info][lexical]  set_start\n\n");
}

void free_list(word_paser **paser_list){  // 释放空间
    for(int i = 0;i < MAX_PASER_SIZE;i += 1){
        if(paser_list[i]->text != NULL){  // 释放text
            free(paser_list[i]->text);
            paser_list[i]->text = NULL;
        }
        free(paser_list[i]);  // 释放数组元素
    }
    free(paser_list);  // 释放数组本身
}

int check_list(word_paser **paser_list){  // 检查结果
    // 统计数据
    int end_count = 0;
    int not_count = 0;

    int end_index = 0;  // 最后一个匹配成功的解析器的index
    for(int i = 0;i < MAX_PASER_SIZE;i += 1){
        if(paser_list[i]->status == END){  // 统计END的次数
            end_count += 1;
            end_index = i;
        }
        if(paser_list[i]->status == NOTMATCH){  // 统计END的次数
            not_count += 1;
        }
        fprintf(debug, "[debug][lexical]  check list : paser_list[%d]->status = %d\n", i, paser_list[i]->status);
    }
    fprintf(debug, "[debug][lexical]  check list : end_count = %d\n", end_count);
    fprintf(debug, "[debug][lexical]  check list : not_count = %d\n", not_count);
    fprintf(debug, "[debug][lexical]  check list : count all = %d\n\n", MAX_PASER_SIZE);

    // 需要往回放一个字符
    if((MAX_PASER_SIZE - not_count - end_count) == 0 && end_count == 1){  // 除了不匹配就是匹配成功，且只有一个成功
        back_p();  // 回退一个字符[所有匹配成功的都必须吞一个字符，然后再这里统一回退]
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
    if(USE){  // USE是判断STATUS的条件
        GET_LEN(paser);
        if(p <= '9' && p >= '0'){  // 是数字
            SET_TEXT(paser);  // 设置text
            paser->status = 1;
        }
        else{
            NO_MATCH(paser);  // 没有成功匹配的处理
        }
    }
    UNUSE_SET;  // if(USE)的else语句
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


void match_text(char p, word_paser *paser, char *text){  // 匹配换行符
    char *match = text;  // 待匹配字符串
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