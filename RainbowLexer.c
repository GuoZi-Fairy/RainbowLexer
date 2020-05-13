#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>
#include"RainbowLexer.h"
#define INIT_ID INT_MAX
typedef struct RainbowStatusLine
{
    size_t id;
    char initChar;
    struct RainbowStatusLine *table;//链表
    struct RainbowStatusLine *Next;
}rStatu;


#define Next(obj) (obj->Next)
static size_t RainBowLexer_id;
RainbowLexerPrivate(rStatu*) StatuLineTable(char initCh) //从返回hash表中索引为initCh的状态链
{
    static rStatu StatuLineTable[512] = {0};
    return &StatuLineTable[initCh];
}

RainbowLexerPrivate(rStatu) NewStatus(char initChar) //创建一个 字符为initchar的状态
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

RainbowLexerPrivate(int) cheekStatusLine(char initCh) //检查以InitCh为索引的状态链是否存在
{
    rStatu* cheekRet = StatuLineTable(initCh);
    return cheekRet == 0 ? 0 : 1;
}
RainbowLexerPrivate(void) RainbowStatuLineParse(rStatu* statu,char* token)
{
    //递归的构建状态链
    //检查当前statu目录下是否有ch 
    //若有则往下一层
    //若无则添加并新建
    while (statu!=NULL)
    {
        if(statu->initChar == *token)
        {
            RainbowStatuLineParse(statu->table,token+1);
            return;
        }
        else
            statu = Next(statu);
    }
    rStatu* lastestStatu = NULL;
    if(token=='\0'&&statu==NULL)printf("重复的词!");
    while(*token != '\0')
    {
        statu = (rStatu*)malloc(sizeof(rStatu));
        lastestStatu = statu;
        printf("加入状态%c\n",*token);
        statu->initChar = *token++;
        statu = Next(statu);
        statu = NULL;
    }
    lastestStatu->id = RainBowLexer_id++;
    printf("该词编号为%lld\n",lastestStatu->id);
}
RainbowLexerPrivate(void) RainbowCreateStatusLine(const char* token,size_t id) //将静态token转化为状态机并加入hash中
{
    const char* parser = token;
    if (!cheekStatusLine(*parser))
    {
       rStatu* statuLine =  StatuLineTable(*parser);
       statuLine->initChar = *parser++;

    }
    
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
int main(int argc, char const *argv[])
{
    SetConsoleOutputCP(65001);
    rStatu test = {INIT_ID,'I',NULL,NULL};
    RainbowStatuLineParse(test.table,"NT32");
    RainbowStatuLineParse(test.table,"NT64");
    return 0;
}

