#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>
#include"RainbowLexer.h"
#define INIT_ID INT_MAX
#define HASH_TABLE_SIZE 512
typedef struct RainbowStatusLine
{
    size_t id;
    char initChar;
    struct RainbowStatusLine *table;//链表
    struct RainbowStatusLine *Next;
}rStatu;
typedef struct RainbowError
{
    char* errorMsg;
    char* errorDoc;
}RainbowError;
static RainbowError repeatedErr = {"Repeated tokenRule","Repeated token ->"};
#define RAINBOW_RAISE(ERROR) printf("ERROR:\n[%s]:%s\n",ERROR.errorMsg,ERROR.errorDoc);
static size_t RainBowLexer_id;
static char* TokenRule_Now;
RainbowLexerPrivate(rStatu*) StatuLineTable(int initCh) //从返回hash表中索引为initCh的状态链
{

    static rStatu StatuLineTable[HASH_TABLE_SIZE+1] = {0};//最后一个位置用于存储初始情况
    return &StatuLineTable[initCh];
}
RainbowLexerPrivate(rStatu) NewStatus(unsigned int initChar) //创建一个 字符为initchar的状态
{
    rStatu ret = {INIT_ID,initChar,NULL,NULL};
    return ret;
}
RainbowLexerPrivate(void) AddTableElement(rStatu* destn,rStatu* resouce)//为目录表添加一个新的状态分支
{
    rStatu* linkTable = destn->table;
    while (linkTable!=NULL)linkTable = linkTable->Next;
    linkTable = resouce;
    linkTable->Next = NULL;
}
RainbowLexerPrivate(int) cheekStatusLine(unsigned int initCh) //检查以InitCh为索引的状态链是否存在
{
    rStatu* cheekRet = StatuLineTable(initCh);
    rStatu* compStatu = StatuLineTable(HASH_TABLE_SIZE);
    if((cheekRet->id == compStatu->id)&&(cheekRet->initChar == compStatu->initChar)&&(cheekRet->Next==compStatu->Next)&&(cheekRet->table == compStatu->table))return 0;
    return 1;
    
}
RainbowLexerPrivate(void) RainbowStatuLineParse(rStatu** statu,const char* token)
{
    //递归的构建状态链
    //检查当前statu目录下是否有ch 
    //若有则往下一层
    //若无则添加并新建
    while (*statu!=NULL)
    {
        if((*statu)->initChar == *token)
        {
            RainbowStatuLineParse(&((*statu)->table),token+1);
            return;
        }
        else
            statu = &((*statu)->Next);
    }
    rStatu** lastestStatu = NULL;
    if(*token == '\0' && (*statu) == NULL)
    {
        RAINBOW_RAISE(repeatedErr);
        printf("%s of id:%lld\n",TokenRule_Now,RainBowLexer_id);
        return;
    }
    while(*token != '\0')
    {
        *statu = (rStatu*)malloc(sizeof(rStatu));
        lastestStatu = statu;
        (*statu)->initChar = *token++;
        (*statu)->Next = NULL;
        statu = &((*statu)->table);
        *statu = NULL;
    }
    (*lastestStatu)->id = RainBowLexer_id++;
}
#define STATULINE_INIT(statuLine,ch) {\
    statuLine->initChar = ch;\
    statuLine->Next = NULL;\
    statuLine->table = NULL;\
}
RainbowLexerPrivate(void) RainbowCreateStatusLine(const char* token) //将静态token转化为状态机并加入hash中
{
    const char* parser = token;
    rStatu* statuLine =  StatuLineTable(*parser);
    if (!cheekStatusLine(*parser)) STATULINE_INIT(statuLine,*parser);
    RainbowStatuLineParse(&(statuLine->table),parser+1);
}
RainbowLexerPublish(void) RainbowStatusBuild(const char* RuleContent)//从规则描述中解析状态机 
{
    const char* lexer = RuleContent;
    while (*lexer!='\0')
    {
        char buf[50] = {'\0'};
        memset(buf,'\0',50);
        size_t index = 0;
        while (*lexer != ',' || *lexer != '\0') buf[index++] = *lexer++;

    }
    
}
RainbowLexerPrivate(void) RainbowStatusShowLine(rStatu* statu,size_t deep)
{
    #define output(STATU,DEEP,CH) do{\
        putchar('|');\
        for (size_t i = 0; i < DEEP; i++) putchar(' ');\
        putchar(' ');\
        putchar('|');\
        putchar('-');\
        putchar(CH);\
        putchar('\n');\
        putchar('|');\
        for (size_t i = 0; i < DEEP; i++) putchar(' ');\
        putchar(' ');\
        putchar(' ');\
        putchar('|');\
        putchar('\n');\
    }while(0)
    while(statu != NULL)
    {
        output(statu,deep,statu->initChar);
        RainbowStatusShowLine(statu->table,deep+1);
        if(statu->table == NULL)
        {
            putchar('|');
            for (size_t i = 0; i < deep; i++) putchar(' ');
            printf("(id:%lld)\n",statu->id);
        }
        statu = statu->Next;
    }
    return;
}
RainbowLexerPrivate(void) RainbowStatusShowRule()
{
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
        if(cheekStatusLine(i))RainbowStatusShowLine(StatuLineTable(i),0);
}


/*
    静态实体的状态机** [数字检验 , 变量名检验]
*/

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
    else if ((ch>='a'&&ch<='z')||(ch=='_'))return 1;
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
#define WHITESPACE_SKIP(str) {while(*str==' ')str++;}
RainbowLexerPrivate(int) RainbowStatusCheekOfStaticWordValidity(const char* token)
{
    WHITESPACE_SKIP(token);
    if (!cheekStatusLine(*token)) return -1;
    rStatu* StatuLine = StatuLineTable(*token++);
    recursive_token:
        StatuLine = StatuLine->table;
        while (StatuLine->initChar != *token && StatuLine->Next != NULL) StatuLine = StatuLine->Next;
        if(StatuLine->Next == NULL && StatuLine->initChar != *token)return -1;
        else if(StatuLine->table == NULL && *(token+1) =='\0')return StatuLine->id;
        else if(StatuLine->table != NULL && *(token+1) =='\0')return -1;
        else if(StatuLine->table == NULL && *(token+1) !='\0')return -1;
        token++;
    goto recursive_token;
}
int main(int argc, char const *argv[])
{
    SetConsoleOutputCP(65001);
    RainbowCreateStatusLine("World4");
    RainbowCreateStatusLine("INT32");
    RainbowCreateStatusLine("INT64");
    RainbowCreateStatusLine("FLOAT32");
    RainbowCreateStatusLine("FLOAT64");
    RainbowCreateStatusLine("FOR");
    RainbowCreateStatusLine("WHILE");
    // RainbowStatusShowRule();

    /* Test of NumValidity
    // char* test = "12315";
    // printf("%s is %d",test,RainbowStatusCheekNumValidity(test));
    */

    /* Test of varNameValidity
    // char* test = "__safa_124fafg_adsdg";
    // printf("%s is %d",test,RainbowStatusCheekVarNameValidity(test));
    */

    /* Test of StaticWordValidity
    // char* test = "WHILE";
    // printf("%s is %d",test,RainbowStatusCheekOfStaticWordValidity(test));
    */
    return 0;
}
