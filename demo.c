#include "demo.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
typedef struct RainbowError
{
    char* errorMsg;
    char* errorDoc;
}RainbowError;
typedef struct queue
{
    RainbowToken* queue;
    unsigned long long size;
    unsigned long long rear;
    unsigned long long front;
}Rainbowqueue;
static long long RainBowLexer_id_num = ID_NUM;
static long long RainBowLexer_id_var = ID_VAR;
static long long RainBowLexer_id_string = ID_STRING;
#define BUF_SIZE 1024
#define QUEUE_INIT_SIZE 512;
static Rainbowqueue RainbowLexer_Ret = {NULL,0,0,0};
static RainbowError repeatedErr = {"Repeated tokenRule","Repeated token ->"};
static RainbowError UndefineToken = {"Undefine token","UNdefine Token ->"};
static RainbowError StringError = {"Expected \"","can not match \""};
static RainbowError MallocError = {"fail malloc","failed to alloc memory"};
#define RAINBOW_RAISE(ERROR) printf("ERROR:\n[%s]:%s\n",ERROR.errorMsg,ERROR.errorDoc);
#define WHITESPACE_SKIP(str) {while(*str==' ')str++;}
RainbowLexerPrivate(int) RainbowQueueInitedTest()
{
    return (RainbowLexer_Ret.front==0&&RainbowLexer_Ret.rear==0&&RainbowLexer_Ret.size==0&&RainbowLexer_Ret.queue==NULL);
}
RainbowLexerPrivate(void) RainbowQueueINIT()
{
    
    RainbowLexer_Ret.front = 0;
    RainbowLexer_Ret.rear = 0;
    RainbowLexer_Ret.size = QUEUE_INIT_SIZE;
    RainbowLexer_Ret.queue = (RainbowToken*)malloc(RainbowLexer_Ret.size*sizeof(RainbowToken));
}
RainbowLexerPrivate(void) RainbowRetAdd(char* token,long long id)
{
    // printf("[%s] with %ld\n",token,id);
    if(id >= IGNORE_MIN && id <= IGNORE_MAX)return;
    if(RainbowLexer_Ret.front + 1 >= RainbowLexer_Ret.size)
    {
        while(RainbowLexer_Ret.front + 1 >= RainbowLexer_Ret.size)RainbowLexer_Ret.size += RainbowLexer_Ret.size >> 1;
        RainbowLexer_Ret.queue = (RainbowToken*)realloc(RainbowLexer_Ret.queue,RainbowLexer_Ret.size*sizeof(RainbowToken));
    }
    RainbowToken* tokenElement = RainbowLexer_Ret.queue + RainbowLexer_Ret.front++;
    size_t len = strlen(token);
    tokenElement->id = id;
    tokenElement->token = (char*)malloc((len+1)*sizeof(char));
    if(tokenElement->token == NULL)
    {
        RAINBOW_RAISE(MallocError);
        printf("[TOKEN-MALLOC]\n");
        getchar();
        return;
    }
    for (size_t i = 0; i < len; i++)
    {
        tokenElement->token[i] = token[i];
    }
    tokenElement->token[len] = '\0';
}
RainbowLexerPrivate(long long) RainbowStatuSperatorMatch(const char* token);
RainbowLexerPrivate(long long) RainbowStatusCheekOfStaticWordValiditySp(const char* token);
RainbowLexerPrivate(long long) RainbowStatusCheekOfStaticWordValidity(const char* token);

RainbowLexerPrivate(int) GetStatusOfNum(char ch)
{
    switch (ch)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return 4;
        break;
    case '-':
    case '+':
        return 1;
        break;
    case ' ':
        return 0;
        break;
    case '.':
        return 2;
        break;
    case 'e':
    case 'E':
        return 3;
        break;
    case '\0':
        return 6;
        break;
    default:
        return 5;
        break;
    }
}
RainbowLexerPrivate(int) RainbowStatusCheekNumValidity(const char* token)//用于检查一个字符串是否为合法数字表示
{

    static int numStatuGra[10][7] = {//-1为否 -2为小数  -3为整数 -4为科学计数
      //blank signal  .  e  0 alphabet end
        {0 ,     1 ,  2,-1, 8,   -1   , -1},//begin:0
        {-1,     -1,  2,-1, 8,   -1   , -1},//Signal:1
        {-1,     -1, -1,-1, 9,   -1   , -1},//Dot [before num]:2
        {-1,     -1,  4, 5, 3,   -1   , -4},//number[before E]:3
        { 7,     -1, -1, 5, 9,   -1   , -2},//Dot [after num]:4
        {-1,      1, -1,-1, 6,   -1   , -1},//e:5
        { 7,     -1, -1,-1, 6,   -1   , -4},//number[after e]:6
        { 7,     -1, -1,-1,-1,   -1   , -5},//blank:7
        { 7,     -1,  9, 5, 8,   -1   , -3},//number[before dot]:8
        { 7,     -1, -1, 5, 9,   -1   , -2}//number[after dot]:9
    };
    int status = numStatuGra[0][GetStatusOfNum(*token++)];
    while (status >= 0)
    {
        status = numStatuGra[status][GetStatusOfNum(*token++)];
    }
    return status;
}
RainbowLexerPrivate(int) GetStatusOfVarName(char ch)
{
    if (ch>='0'&&ch<='9')return 2;
    else if ((isalpha(ch))||(ch=='_'))return 1;
    else if (ch == ' ')return 0;
    else if (ch == '\0')return 3;
    else return 4;
}
RainbowLexerPrivate(int) RainbowStatusCheekVarNameValidity(const char* token)
{
    static int varNameStatus[4][5] = 
    {//whitespace , ch , num , end  undefine
        { 0       ,  1 ,  -1  ,  -1 , -1},//start : 0
        { 3       ,  1 ,   2  ,  -2 , -1},//ch : 1
        { 3       ,  1 ,   2  ,  -2 , -1},//num:2
        { 3       , -1 ,  -1  ,  -2 , -1},//whitespace : 3
    };
    int status = varNameStatus[0][GetStatusOfVarName(*token++)];
    while (status >= 0)
    {
        status = varNameStatus[status][GetStatusOfVarName(*token++)];
    }
    return status;
}
RainbowLexerPrivate(char*) RainbowStatusCheekOfString(const char* token,char singleORdouble)
{
    const char* ptr = token;
    int len = 0;
    while (ptr[len] != singleORdouble)
    {
        if(ptr[len]=='\n'||ptr[len]=='\0')
        {
            RAINBOW_RAISE(StringError);
            return NULL;
        }
        else if(ptr[len]=='\\' && ptr[len+1]==singleORdouble)len+=1;
        len++;
    }
    char* string = (char*)malloc(len*sizeof(char));
    for (size_t i = 0; i < len; i++)
    {
        string[i] = token[i];
    }
    string[len] = '\0';
    return string;
}
RainbowLexerPublic(void) RainbowLex(const char* string)
{
    /* 将string解析
        并加入到ret列表中
    */
    if(RainbowLexer_Ret.queue == NULL)RainbowQueueINIT();
    const char* strptr = string;
    WHITESPACE_SKIP(strptr);
    char buf[BUF_SIZE] = {'\0'};
    size_t index = 0;
    while(*strptr != '\0')
    {
        #if defined(DOUBLE_STRING_OPTION) || defined(SINGLE_STRING_OPTION) 
            #if !defined(DOUBLE_STRING_OPTION) && defined(SINGLE_STRING_OPTION) 
                    char *string = RainbowStatusCheekOfString(strptr + 1, '\'');
            #endif
            #if defined(DOUBLE_STRING_OPTION) && !defined(SINGLE_STRING_OPTION)
                    char *string = RainbowStatusCheekOfString(strptr + 1, '\"');
            #endif
            #if defined(DOUBLE_STRING_OPTION) && defined(SINGLE_STRING_OPTION)
                    char *string = RainbowStatusCheekOfString(strptr + 1, *strptr == '\"' ? '\"' : '\'');
            #endif
            {
                if(*buf!='\0')
                {
                    long long id = 0;
                    id = RainbowStatusCheekOfStaticWordValidity(buf);
                    if(id >= 0) RainbowRetAdd(buf,id);
                    else if(isalpha(*buf)&&(RainbowStatusCheekVarNameValidity(buf) != -1))RainbowRetAdd(buf,RainBowLexer_id_var);
                    else if(isdigit(*buf)&&(RainbowStatusCheekNumValidity(buf) != -1))RainbowRetAdd(buf,RainBowLexer_id_num);
                    else//错误处理
                    {
                        RAINBOW_RAISE(UndefineToken);
                        printf("%s\n",buf);
                        getchar();
                        putchar('\n');
                    }
                }
                #if !defined(DOUBLE_STRING_OPTION) && defined(SINGLE_STRING_OPTION) 
                char* string = RainbowStatusCheekOfString(strptr+1,'\'');
                #endif
                #if defined(DOUBLE_STRING_OPTION) && !defined(SINGLE_STRING_OPTION)
                char* string = RainbowStatusCheekOfString(strptr+1,'\"');
                #endif
                #if defined(DOUBLE_STRING_OPTION) && defined(SINGLE_STRING_OPTION)
                char* string = RainbowStatusCheekOfString(strptr+1,*strptr== '\"' ? '\"' : '\'');
                #endif
                if(string == NULL)return;
                int len = strlen(string);
                strptr+=len+2;
                RainbowRetAdd(string,RainBowLexer_id_string);
                continue;
            }
            #endif
            int spRet = RainbowStatuSperatorMatch(strptr);
            if(buf[0] == '\0' && spRet >= 0) goto BUF_EMPTY_CASE_;//处理第一个字符是分隔符的情况
            if(spRet >= 0)
            {   
                long long id = 0;
                id = RainbowStatusCheekOfStaticWordValidity(buf);
                if(id >= 0) RainbowRetAdd(buf,id);

                else if(isalpha(*buf)&&(RainbowStatusCheekVarNameValidity(buf) != -1))RainbowRetAdd(buf,RainBowLexer_id_var);
                else if(isdigit(*buf)&&(RainbowStatusCheekNumValidity(buf) != -1))RainbowRetAdd(buf,RainBowLexer_id_num);
                else if(*buf == '\0')goto BUF_EMPTY_CASE_;//用于处理多个连续分隔符的情况
                else//错误处理
                {
                    RAINBOW_RAISE(UndefineToken);
                    printf("%s\n",buf);
                    getchar();
                    putchar('\n');
                }

                BUF_EMPTY_CASE_:
                memset(buf,'\0',BUF_SIZE);
                index = 0;
                for (size_t i = 0; i <= spRet; i++)buf[i] = *strptr++;
                RainbowRetAdd(buf,RainbowStatusCheekOfStaticWordValiditySp(buf));
                memset(buf,'\0',BUF_SIZE);
            }
            else
                buf[index++] = *strptr++;
    }
    
    if(*buf == '\0')return;//buf已空  情况出现在最后一个字符是分隔符时

    //清理buf
    long long id = 0;
    id = RainbowStatusCheekOfStaticWordValidity(buf);
    if(id >= 0) RainbowRetAdd(buf,id);
    else if(isalpha(*buf)&&(RainbowStatusCheekVarNameValidity(buf) != -1))RainbowRetAdd(buf,RainBowLexer_id_num);
    else if(isdigit(*buf)&&(RainbowStatusCheekNumValidity(buf) != -1))RainbowRetAdd(buf,RainBowLexer_id_var);
    else//错误处理
    {
        RAINBOW_RAISE(UndefineToken);
        printf("%s\n",buf);
        getchar();
        putchar('\n');
    }
}
RainbowLexerPublic(RainbowToken*) RainbowNext()
{
    if(RainbowLexer_Ret.rear == RainbowLexer_Ret.front) return NULL;
    else return RainbowLexer_Ret.queue + RainbowLexer_Ret.rear++;
}
RainbowLexerPrivate(void) RainbowQueueClear()
{
    RainbowLexer_Ret.rear = 0;
    int front = RainbowLexer_Ret.front -1;
    while (front >= 0)
    {
        free((RainbowLexer_Ret.queue + front)->token);
        front--;
    }
    RainbowLexer_Ret.front = 0;
    RainbowLexer_Ret.size = QUEUE_INIT_SIZE;
    RainbowLexer_Ret.queue = (RainbowToken*)realloc(RainbowLexer_Ret.queue,RainbowLexer_Ret.size);
}

RainbowLexerPrivate(long long) RainbowStatusCheekOfStaticWordValidity(const char* token){
switch (*token++){
case 'd':{switch (*token++) {
case 'i':{switch (*token++) {
case 'd':{switch (*token++) {
case 'i':{switch (*token++) {
case 'd':{switch (*token++) {
case 'i':{if(*token == '\0')return 4;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}case 'h':{switch (*token++) {
case 'e':{switch (*token++) {
case 'l':{if(*token == '\0')return 4;switch (*token++) {
case 'l':{switch (*token++) {
case 'o':{if(*token == '\0')return 1;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}case 'a':{switch (*token++) {
case 'h':{switch (*token++) {
case 'a':{switch (*token++) {
case 'h':{switch (*token++) {
case 'a':{if(*token == '\0')return 5;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}case 'p':{switch (*token++) {
case 'p':{switch (*token++) {
case 'p':{if(*token == '\0')return 6;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}case 't':{switch (*token++) {
case 'e':{switch (*token++) {
case 's':{switch (*token++) {
case 't':{if(*token == '\0')return 3;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}case 'w':{switch (*token++) {
case 'o':{switch (*token++) {
case 'r':{switch (*token++) {
case 'l':{switch (*token++) {
case 'd':{if(*token == '\0')return 2;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}default:return -1;
break;}}
RainbowLexerPrivate(long long) RainbowStatusCheekOfStaticWordValiditySp(const char* token){
switch (*token++){
case '\t':{return 2147473649;break;}case '\n':{return 2147473648;break;}case ' ':{return 2147473647;break;}case '%':{return 20;break;}case '*':{return 22;break;}case '/':{return 21;break;}default:return -1;
break;}}
RainbowLexerPrivate(long long) RainbowStatuSperatorMatch(const char* token){
long long id = RainbowStatusCheekOfStaticWordValiditySp(token);
switch (id){
case 2147473649:{return 0;break;}
case 2147473648:{return 0;break;}
case 2147473647:{return 0;break;}
case 20:{return 0;break;}
case 22:{return 0;break;}
case 21:{return 0;break;}

default:{return -1;break;}}}
