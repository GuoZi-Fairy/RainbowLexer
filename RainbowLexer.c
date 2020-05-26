#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<ctype.h>
#ifdef _WIN32
    #include<Windows.h>
#endif
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
typedef struct __token__
{
    char* token;
    size_t id;
}RainbowToken;

typedef struct queue
{
    RainbowToken* queue;
    unsigned long long size;
    unsigned long long rear;
    unsigned long long front;
}Rainbowqueue;

static RainbowError repeatedErr = {"Repeated tokenRule","Repeated token ->"};
static RainbowError UndefineToken = {"Undefine token","UNdefine Token ->"};
#define RAINBOW_RAISE(ERROR) printf("ERROR:\n[%s]:%s\n",ERROR.errorMsg,ERROR.errorDoc);
static size_t RainBowLexer_id;
static size_t RainBowLexer_id_num;
static size_t RainBowLexer_id_var;
static char* TokenRule_Now;
static Rainbowqueue RainbowLexer_Ret;
#define WHITESPACE_SKIP(str) {while(*str==' ')str++;}
#define STATULINE_INIT(statuLine,ch) {\
    statuLine->initChar = ch;\
    statuLine->Next = NULL;\
    statuLine->table = NULL;\
}
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
RainbowLexerPrivate(void) RainbowStatuLineParse(rStatu** statu,const char* token)//接受一个词
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
    if(*token == '\0' && (*statu) == NULL)//errorRaiser
    {
        RAINBOW_RAISE(repeatedErr);
        printf("TOKEN:%s of id:%ld\nIn id of %ld\n",TokenRule_Now,RainBowLexer_id,RainBowLexer_id);
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
RainbowLexerPrivate(void) RainbowCreateStatusLine(const char* token) //将静态token转化为状态机并加入hash中
{
    const char* parser = token;
    rStatu* statuLine =  StatuLineTable(*parser);
    if (!cheekStatusLine(*parser)) STATULINE_INIT(statuLine,*parser);
    if(*(parser+1)!='\0') RainbowStatuLineParse(&(statuLine->table),parser+1);
    else statuLine->id = RainBowLexer_id++;
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
            printf("(id:%ld)\n",statu->id);
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


/* sperator */
RainbowLexerPrivate(rStatu*) StatuLineTableSp(int initCh) //从返回hash表中索引为initCh的状态链
{

    static rStatu StatuLineTableSp[HASH_TABLE_SIZE+1] = {0};//最后一个位置用于存储初始情况
    return &StatuLineTableSp[initCh];
}
RainbowLexerPrivate(void) RainbowStatuLineParseSp(rStatu** statu,const char* token)//接受一个词
{
    //递归的构建状态链
    //检查当前statu目录下是否有ch 
    //若有则往下一层
    //若无则添加并新建
    while (*statu!=NULL)
    {
        if((*statu)->initChar == *token)
        {
            RainbowStatuLineParseSp(&((*statu)->table),token+1);
            return;
        }
        else
            statu = &((*statu)->Next);
    }
    rStatu** lastestStatu = NULL;
    if(*token == '\0' && (*statu) == NULL)//errorRaiser
    {
        RAINBOW_RAISE(repeatedErr);
        printf("SP:%s of id:%ld\nIn id of %ld\n",TokenRule_Now,RainBowLexer_id,RainBowLexer_id);
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
RainbowLexerPrivate(int) cheekStatusLineSp(unsigned int initCh) //检查以InitCh为索引的状态链是否存在
{
    rStatu* cheekRet = StatuLineTableSp(initCh);
    rStatu* compStatu = StatuLineTableSp(HASH_TABLE_SIZE);
    if((cheekRet->id == compStatu->id)&&(cheekRet->initChar == compStatu->initChar)&&(cheekRet->Next==compStatu->Next)&&(cheekRet->table == compStatu->table))return 0;
    return 1;
    
}
RainbowLexerPrivate(void) RainbowCreateStatusLineSp(const char* token) //将静态token转化为状态机并加入hash中
{
    const char* parser = token;
    rStatu* statuLine =  StatuLineTableSp(*parser);
    if (!cheekStatusLineSp(*parser)) STATULINE_INIT(statuLine,*parser);
    if(*(parser+1)!='\0') RainbowStatuLineParseSp(&(statuLine->table),parser+1);
    else statuLine->id = RainBowLexer_id++;
}
RainbowLexerPrivate(void) RainbowStatusShowRuleSp()
{
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
        if(cheekStatusLineSp(i))RainbowStatusShowLine(StatuLineTableSp(i),0);
}
RainbowLexerPrivate(int) RainbowStatuSperatorMatch(const char* token)
{
    /*
        从参数位置开始匹配分隔符
        若不是分隔符 则返回-1
        若是 则范围从起始位置的偏移量 
    */
    rStatu* compStatu = StatuLineTable(HASH_TABLE_SIZE);
    const char* ch = token;
    int result = 0;
    if(!cheekStatusLineSp(*ch))return -1;
    rStatu* SpStatu = StatuLineTableSp(*ch++);
    rStatu* lastStatu = SpStatu;
    SpStatu = SpStatu->table;
    if(SpStatu == NULL)return result;//单字符特判
    while(SpStatu != NULL && *ch!='\0')
    {
        lastStatu = SpStatu;
        if(SpStatu->initChar == *ch && SpStatu->table != NULL) 
        {
            SpStatu = SpStatu->table;
            ch++;
            result++;
        }
        else if(SpStatu->initChar != *ch && (SpStatu->table==NULL || SpStatu->id != compStatu->id)) return result;
        else if(SpStatu->initChar == *ch && SpStatu->table == NULL)return result+1;
        else SpStatu = SpStatu->Next;
    }
    return -1;
}
RainbowLexerPrivate(int) RainbowStatusCheekOfStaticWordValiditySp(const char* token)
{
    /*
        检验静态token的合法性
        若合法则返回对应id
        若不合法则返回-1 
    */
    // WHITESPACE_SKIP(token);
    if (!cheekStatusLineSp(*token)) return -1;
    rStatu* StatuLine = StatuLineTableSp(*token++);
    if(*token == '\0')return StatuLine->id;//单字符特判
    recursive_token:
        StatuLine = StatuLine->table;
        while (StatuLine->initChar != *token && StatuLine->Next != NULL) StatuLine = StatuLine->Next;
        if(StatuLine->Next == NULL && StatuLine->initChar != *token)return -1;
        else if(StatuLine->table == NULL && *(token+1) =='\0')return StatuLine->id;
        else if(StatuLine->table != NULL && *(token+1) =='\0')
        {
            rStatu* compStatu = StatuLineTableSp(HASH_TABLE_SIZE);
            if(StatuLine->id!=compStatu->id)return StatuLine->id;
            else return -1;
        }
        else if(StatuLine->table == NULL && *(token+1) !='\0')return -1;
        token++;
    goto recursive_token;
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
RainbowLexerPrivate(int) RainbowStatusCheekOfStaticWordValidity(const char* token)
{
    /*
        检验静态token的合法性
        若合法则返回对应id
        若不合法则返回-1 
    */
    WHITESPACE_SKIP(token);
    if (!cheekStatusLine(*token)) return -1;
    rStatu* StatuLine = StatuLineTable(*token++);
    if(*token == '\0')return StatuLine->id;
    recursive_token:
        StatuLine = StatuLine->table;
        while (StatuLine->initChar != *token && StatuLine->Next != NULL) StatuLine = StatuLine->Next;
        if(StatuLine->Next == NULL && StatuLine->initChar != *token)return -1;
        else if(StatuLine->table == NULL && *(token+1) =='\0')return StatuLine->id;
        else if(StatuLine->table != NULL && *(token+1) =='\0')
        {
            rStatu* compStatu = StatuLineTable(HASH_TABLE_SIZE);
            if(StatuLine->id!=compStatu->id)return StatuLine->id;
            else return -1;
        }
        else if(StatuLine->table == NULL && *(token+1) !='\0')return -1;
        token++;
    goto recursive_token;
}

/*
    从分隔文本
*/
#define QUEUE_INIT_SIZE 512;
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
#define BUF_SIZE 1024
RainbowLexerPrivate(void) RainbowLex(const char* string)
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
            if(spRet >= 0)
            {   
                int id = 0;
                id = RainbowStatusCheekOfStaticWordValidity(buf);
                if(id >= 0) RainbowRetAdd(buf,id);

                else if(isalpha(*buf)&&(RainbowStatusCheekVarNameValidity(buf) != -1))RainbowRetAdd(buf,RainBowLexer_id_var);

                else if(isdigit(*buf)&&(RainbowStatusCheekNumValidity(buf) != -1))RainbowRetAdd(buf,RainBowLexer_id_num);
                else if(*buf == '\0');//用于处理多个连续分隔符的情况
                else//错误处理
                {
                    RAINBOW_RAISE(UndefineToken);
                    printf("%s\n",buf);
                    getchar();
                    putchar('\n');
                }
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

//Compile module
RainbowLexerPrivate(void) RainbowCompile(rStatu* statu)
{
    if (statu == NULL) return;
    if(statu->table == NULL)
    {
            printf("return %ld;break;",statu->id);
            return;
    }
    rStatu* status = statu;
    printf("switch (*token++) \n {\n");
    while (status != NULL)
    {
        if(status->initChar == '\n')printf("case \'\\n\':\n{\n");
        else printf("case \'%c\':\n{\n",status->initChar);
        RainbowCompile(status->table);
        printf("\nbreak;\n}\n");
        status = status->Next;
    }
    printf("\ndefault:\n{");
    printf("\nreturn -1;\n}");
    printf("\n}\n");
}
RainbowLexerPrivate(void) RainbowCompileAllStatusLine()
{
    printf("RainbowLexerPublic(int) RainbowStatusCheekOfStaticWordValidity(const char* token)\n{\n");
    printf("switch (*token++) \n {\n");
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
        if(cheekStatusLine(i))
        {
            if(StatuLineTable(i)->initChar == '\n')printf("case \'\\n\':\n{\n");
            else printf("case \'%c\':\n{\n",StatuLineTable(i)->initChar);
            if(StatuLineTable(i)->table == NULL)
            {
                printf("return %ld;break;\n}\n",StatuLineTable(i)->id);
                continue;
            }
            RainbowCompile(StatuLineTable(i)->table);
            printf("\n}\n");
        }
    printf("}\n}\n");
}
RainbowLexerPrivate(void) RainbowCompileAllStatusLineSp()
{
    printf("\nRainbowLexerPublic(int) RainbowStatusCheekOfStaticWordValiditySp(const char* token)\n{\n");
    printf("switch (*token++) \n {\n");
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
        if(cheekStatusLineSp(i))
        {
            if(StatuLineTableSp(i)->initChar == '\n')printf("case \'\\n\':\n{\n");
            else printf("case \'%c\':\n{\n",StatuLineTableSp(i)->initChar);
            if(StatuLineTableSp(i)->table == NULL)
            {
                printf("return %ld;break;\n}\n",StatuLineTableSp(i)->id);
                continue;
            }
            RainbowCompile(StatuLineTableSp(i)->table);
            printf("\n}\n");
        }
    printf("}\n}\n");
}
RainbowLexerPrivate(void) RainbowCompileDeepthOfSp(rStatu* statu,int len)
{
    if (statu == NULL) return;
    rStatu* status = statu;
    while (status != NULL)
    {
        if(status->table == NULL)
        {
            printf("case %ld:\n{\nreturn %d;\nbreak;\n}\n",status->id,len);
        }
        else
        {
            RainbowCompileDeepthOfSp(status->table,len+1);
        }
        status = status->Next;
    }
}
RainbowLexerPrivate(void) RainbowCompileSpMatcher()
{
    printf("RainbowLexerPrivate(int) RainbowStatuSperatorMatch(const char* token)\n{\n");
    printf("int id = RainbowStatusCheekOfStaticWordValiditySp(token);\n");
    printf("switch (id)\n{\n");
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
    if(cheekStatusLineSp(i))
    {
        rStatu* statusLine = StatuLineTableSp(i);
        while(statusLine != NULL)
        {
            RainbowCompileDeepthOfSp(statusLine,0);
            statusLine = statusLine->Next;
        }
    }
    printf("\ndefault:\n{\nreturn -1;\nbreak;\n}");
    printf("\n}\n}\n");
}
int test()
{
    RainBowLexer_id = 1;
    RainBowLexer_id_num = 2;
    RainBowLexer_id_var = 3;
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif
    RainbowCreateStatusLine("World4");
    RainbowCreateStatusLine("INT32");
    RainbowCreateStatusLine("INT64");
    RainbowCreateStatusLine("FLOAT32");
    RainbowCreateStatusLine("FLOAT64");
    RainbowCreateStatusLine("FOR");
    RainbowCreateStatusLine("WHILE");
    RainbowCreateStatusLineSp("--");
    RainbowCreateStatusLineSp("++");
    RainbowCreateStatusLineSp("(");
    RainbowCreateStatusLineSp(")");
    RainbowCreateStatusLineSp("*");
    RainbowCreateStatusLineSp("/");
    RainbowCreateStatusLineSp("[");
    RainbowCreateStatusLineSp("]");
    RainbowCreateStatusLineSp("***");
    RainbowCreateStatusLineSp("xxx");
    RainbowCreateStatusLineSp("\n");
    RainbowCreateStatusLineSp(" ");
    RainbowStatusShowRule();
    printf("-------------------\n");
    RainbowStatusShowRuleSp();
    printf("-------------------\n");
    // /* Test of NumValidity
    // char* test_NumValidity = "12315";
    // printf("%s is %d",test_NumValidity,RainbowStatusCheekNumValidity(test_NumValidity));
    // // */

    // // /* Test of varNameValidity
    // char* test_varNameValidity = "__safa_124fafg_adsdg";
    // printf("%s is %d",test_varNameValidity,RainbowStatusCheekVarNameValidity(test_varNameValidity));
    // // */

    // // /* Test of StaticWordValidity
    // char* test_StaticWordValidity = "WHILE";
    // printf("%s is %d",test_StaticWordValidity,RainbowStatusCheekOfStaticWordValidity(test_StaticWordValidity));
    // // */

    // // /* Test of SpMatch
    // char* test_SpMatch = "xxxsafaf    ";
    // printf("%s is %d",test_SpMatch,RainbowStatuSperatorMatch(test_SpMatch));
    // // */
    return 0;
}
int main(int argc, char const *argv[])
{
    test();
    putchar('\n');
    RainbowQueueINIT();
    RainbowLex("hello World4+123e24***$$#%%world WHILE FOR 1234\n\nxxx+++++--][)\n");
    RainbowCompileAllStatusLine();
    RainbowCompileAllStatusLineSp();
    RainbowCompileSpMatcher();
    return 0;
}
