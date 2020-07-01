#include "demoCompile____HeaderIncludeTest.h"
#include "RainbowLexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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
static size_t RainBowLexer_id;
static size_t RainBowLexer_id_num;
static size_t RainBowLexer_id_var;
#define BUF_SIZE 1024
#define QUEUE_INIT_SIZE 512;
static Rainbowqueue RainbowLexer_Ret = {NULL,0,0,0};
static RainbowError repeatedErr = {"Repeated tokenRule","Repeated token ->"};
static RainbowError UndefineToken = {"Undefine token","UNdefine Token ->"};
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
RainbowLexerPrivate(void) RainbowRetAdd(char* token,size_t id)
{
    printf("[%s] with %ld\n",token,id);
    if(RainbowLexer_Ret.front + 1 >= RainbowLexer_Ret.size)
    {
        while(RainbowLexer_Ret.front + 1 >= RainbowLexer_Ret.size)RainbowLexer_Ret.size += RainbowLexer_Ret.size >> 1;
        RainbowLexer_Ret.queue = (RainbowToken*)realloc(RainbowLexer_Ret.queue,RainbowLexer_Ret.size*sizeof(RainbowToken));
    }
    RainbowToken* tokenElement = RainbowLexer_Ret.queue + RainbowLexer_Ret.front;
    size_t len = strlen(token);
    tokenElement->id = id;
    tokenElement->token = (char*)malloc((len+1)*sizeof(char));
    for (size_t i = 0; i < len; i++)
    {
        tokenElement->token[i] = token[i];
    }
    tokenElement->token[len] = '\0';
}
RainbowLexerPrivate(int) RainbowStatuSperatorMatch(const char* token);
RainbowLexerPrivate(int) RainbowStatusCheekOfStaticWordValiditySp(const char* token);
RainbowLexerPrivate(int) RainbowStatusCheekOfStaticWordValidity(const char* token);

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
RainbowLexerPublic(void) RainbowLex(const char* string)
{
    /* 将string解析
        并加入到ret列表中
    */
    const char* strptr = string;
    WHITESPACE_SKIP(strptr);
    char buf[BUF_SIZE] = {'\0'};
    size_t index = 0;
    while(*strptr != '\0')
    {
            int spRet = RainbowStatuSperatorMatch(strptr);
            if(buf[0] == '\0' && spRet >= 0) goto BUF_EMPTY_CASE_;//处理第一个字符是分隔符的情况
            if(spRet >= 0)
            {   
                int id = 0;
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
    int id = 0;
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
RainbowLexerPrivate(int) RainbowStatusCheekOfStaticWordValidity(const char* token){
switch (*token++){
case 'F':{switch (*token++) {
case 'L':{switch (*token++) {
case 'O':{switch (*token++) {
case 'A':{switch (*token++) {
case 'T':{switch (*token++) {
case '3':{return 4;break;}case '6':{return 5;break;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}case 'O':{return 6;break;}
default:{return -1;break;}}
}case 'I':{switch (*token++) {
case 'N':{switch (*token++) {
case 'T':{switch (*token++) {
case '3':{return 2;break;}case '6':{return 3;break;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}case 'W':{switch (*token++) {
case 'o':{switch (*token++) {
case 'r':{switch (*token++) {
case 'l':{switch (*token++) {
case 'd':{return 1;break;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}case 'H':{switch (*token++) {
case 'I':{switch (*token++) {
case 'L':{return 7;break;}
default:{return -1;break;}}
}
default:{return -1;break;}}
}
default:{return -1;break;}}
}default:return -1;
break;}}
RainbowLexerPrivate(int) RainbowStatusCheekOfStaticWordValiditySp(const char* token){
switch (*token++){
case '\n':{return 18;break;}case ' ':{return 19;break;}case '(':{return 10;break;}case ')':{return 11;break;}case '*':{switch (*token++) {
case '*':{switch (*token++) {
case '*':{return 16;break;;}
default:{return 12;break;}}
;}
default:{return 12;break;}}
}case '+':{switch (*token++) {
case '+':{return 9;break;;}
default:{return -1;break;}}
}case '-':{switch (*token++) {
case '-':{return 8;break;;}
default:{return -1;break;}}
}case '/':{return 13;break;}case '[':{return 14;break;}case ']':{return 15;break;}case 'x':{switch (*token++) {
case 'x':{switch (*token++) {
case 'x':{return 17;break;;}
default:{return -1;break;}}
;}
default:{return -1;break;}}
}default:return -1;
break;}}
RainbowLexerPrivate(int) RainbowStatuSperatorMatch(const char* token){
int id = RainbowStatusCheekOfStaticWordValiditySp(token);
switch (id){
case 18:{return 0;break;}
case 19:{return 0;break;}
case 10:{return 0;break;}
case 11:{return 0;break;}
case 12:{return 0;break;}
case 16:{return 2;break;}
case 9:{return 1;break;}
case 8:{return 1;break;}
case 13:{return 0;break;}
case 14:{return 0;break;}
case 15:{return 0;break;}
case 17:{return 2;break;}

default:{return -1;break;}}}
