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
static size_t RainBowLexer_id;

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
    if(*token == '\0' && (*statu) == NULL)printf("重复的词!");
    while(*token != '\0')
    {
        *statu = (rStatu*)malloc(sizeof(rStatu));
        lastestStatu = statu;
        printf("加入状态%c\n",*token);
        (*statu)->initChar = *token++;
        (*statu)->Next = NULL;
        statu = &((*statu)->table);
        *statu = NULL;
    }
    (*lastestStatu)->id = RainBowLexer_id++;
    printf("该词编号为%lld\n",(*lastestStatu)->id);
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
        putchar('-');\
        putchar('-');\
        putchar(CH);\
        putchar('\n');\
        putchar('|');\
        for (size_t i = 0; i < DEEP; i++) putchar(' ');\
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
int main(int argc, char const *argv[])
{
    SetConsoleOutputCP(65001);
    RainbowCreateStatusLine("helloWolrd1");
    RainbowCreateStatusLine("helloWolrd2");
    RainbowCreateStatusLine("helloWolrd3");
    RainbowCreateStatusLine("helloWolrd4");
    RainbowStatusShowRule();
    return 0;
}
