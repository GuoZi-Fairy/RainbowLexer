#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<ctype.h>
#ifdef _WIN32
    #include<Windows.h>
#endif
#include"RainbowLexer.h"
#define INIT_ID (-1) 
#define HASH_TABLE_SIZE 512
typedef struct RainbowStatusLine
{
    long long id;
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
static RainbowError MallocError = {"fail malloc","failed to alloc memory"};
#define RAINBOW_RAISE(ERROR) printf("ERROR:\n[%s]:%s\n",ERROR.errorMsg,ERROR.errorDoc);
static size_t RainBowLexer_id;
static size_t RainBowLexer_id_num;
static size_t RainBowLexer_id_var;
static char* TokenRule_Now;
static rStatu* compStatu;
static Rainbowqueue RainbowLexer_Ret;
#define WHITESPACE_SKIP(str) {while(*str==' ')str++;}
#define STATULINE_INIT(statuLine,ch) {\
    statuLine->id = INIT_ID;\
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
        if(statu== NULL)
        {
            RAINBOW_RAISE(MallocError);
            printf("[PARSE-MALLOC]\n");
            getchar();
            return;
        }
        lastestStatu = statu;
        STATULINE_INIT((*statu),*token++);
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
        if(statu->table == NULL || statu->id != INIT_ID )
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
        if(statu== NULL)
        {
            RAINBOW_RAISE(MallocError);
            printf("[PARSE-MALLOC-SP]\n");
            getchar();
            return;
        }
        lastestStatu = statu;
        STATULINE_INIT((*statu),*token++);
        statu = &((*statu)->table);
        *statu = NULL;
    }
    (*lastestStatu)->id = RainBowLexer_id++;
}
RainbowLexerPrivate(int) cheekStatusLineSp(unsigned int initCh) //检查以InitCh为索引的状态链是否存在
{
    rStatu* cheekRet = StatuLineTableSp(initCh);
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
RainbowLexerPrivate(int) RainbowStatuSperatorMatchRecurse(rStatu* statu,const char* token)
{
    while (statu != NULL && statu->initChar != *token)  statu = statu->Next;//条件一必须在前是因为若在后可能会引起segment fault
    if (statu == NULL) return -1;
    else if (statu != NULL && statu -> table == NULL && statu->id != INIT_ID) return 0;
    int probe = RainbowStatuSperatorMatchRecurse(statu->table,token+1);
    if (probe == -1 && statu->id == INIT_ID)return -1;
    else if (probe == -1 && statu->id != INIT_ID)return 0;
    else return 1 + probe;

}
RainbowLexerPrivate(int) RainbowStatuSperatorMatch(const char* token)//TODO 重写
{
    /*
        从参数位置开始匹配分隔符
        若不是分隔符 则返回-1
        若是 则范围从起始位置的偏移量 
    */
    if (!cheekStatusLineSp(*token))return -1;
    rStatu* statu = StatuLineTableSp(*token);
    return RainbowStatuSperatorMatchRecurse(statu,token);
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
    if(*token == '\0')return StatuLine->id;//单字符特判(若无此id则默认id返回值为-1)
    recursive_token:
        StatuLine = StatuLine->table;
        while (StatuLine->initChar != *token && StatuLine->Next != NULL) StatuLine = StatuLine->Next;
        if(StatuLine->Next == NULL && StatuLine->initChar != *token)return StatuLine->id;
        else if(StatuLine->table == NULL && *(token+1) =='\0')return StatuLine->id;
        else if(StatuLine->table != NULL && *(token+1) =='\0')
        {
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
    if(RainbowLexer_Ret.queue == NULL)
    {
        RAINBOW_RAISE(MallocError);
        printf("[QUEUE-INIT]\n");
        getchar();
        return;
    }
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

//Compile module
RainbowLexerPrivate(void) RainbowCompile(rStatu* statu)//用于编译一条状态链
{
    if (statu == NULL) return;
    if(statu->table == NULL)
    {
            printf("return %ld;break;",statu->id);
            return;
    }
    rStatu* status = statu;
    printf("switch (*token++) {\n");
    while (status != NULL)
    {
        if(status->initChar == '\n')printf("case \'\\n\':{");
        else printf("case \'%c\':{",status->initChar);
        RainbowCompile(status->table);
        printf("}");
        status = status->Next;
    }
    printf("\ndefault:{");
    printf("return -1;break;");
    printf("}}\n");
}
RainbowLexerPrivate(void) RainbowCompileSp(rStatu* statu,int defaultID)//用于编译一条状态链
{
    if (statu == NULL) return;
    rStatu* status = statu;
    printf("switch (*token++) {\n");
    while (status != NULL)
    {
        if(status->initChar == '\n')printf("case \'\\n\':{");
        else printf("case \'%c\':{",status->initChar);
        if(statu->table == NULL)
        {
            printf("return %ld;break;",statu->id);
        }
        RainbowCompileSp(status->table,(status->id==INIT_ID ? defaultID : status->id));
        printf(";}");
        status = status->Next;
    }
    printf("\ndefault:{");
    printf("return %d;break;",defaultID);
    printf("}}\n");
}
RainbowLexerPrivate(void) RainbowCompileAllStatusLine()
{
    printf("RainbowLexerPublic(int) RainbowStatusCheekOfStaticWordValidity(const char* token){\n");
    printf("switch (*token++){\n");
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
        if(cheekStatusLine(i))
        {
            if(StatuLineTable(i)->initChar == '\n')printf("case \'\\n\':\n{");
            else printf("case \'%c\':{",StatuLineTable(i)->initChar);
            if(StatuLineTable(i)->table == NULL)
            {
                printf("return %ld;break;}",StatuLineTable(i)->id);
                continue;
            }
            RainbowCompile(StatuLineTable(i)->table);
            printf("}");
        }
    printf("default:return -1;\nbreak;}}\n");
}
RainbowLexerPrivate(void) RainbowCompileAllStatusLineSp()
{
    printf("RainbowLexerPublic(int) RainbowStatusCheekOfStaticWordValiditySp(const char* token){\n");
    printf("switch (*token++){\n");
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
        if(cheekStatusLineSp(i))
        {
            if(StatuLineTableSp(i)->initChar == '\n')printf("case \'\\n\':{");
            else printf("case \'%c\':{",StatuLineTableSp(i)->initChar);
            if(StatuLineTableSp(i)->table == NULL)
            {
                printf("return %ld;break;}",StatuLineTableSp(i)->id);
                continue;
            }
            RainbowCompileSp(StatuLineTableSp(i)->table,(StatuLineTableSp(i)->id==INIT_ID ? -1 : (StatuLineTableSp(i)->id)));
            printf("}");
        }
    printf("default:return -1;\nbreak;}}\n");
}
RainbowLexerPrivate(void) RainbowCompileDeepthOfSp(rStatu* statu,int len)
{
    if (statu == NULL) return;
    rStatu* status = statu;
    while (status != NULL)
    {
        if(status->table == NULL)
        {
            printf("case %ld:{return %ld;break;}\n",status->id,len);
        }
        else if (status->id != INIT_ID)
        {
            printf("case %ld:{return %ld;break;}\n",status->id,len);
            RainbowCompileDeepthOfSp(status->table,len+1);
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
    printf("RainbowLexerPrivate(int) RainbowStatuSperatorMatch(const char* token){\n");
    printf("int id = RainbowStatusCheekOfStaticWordValiditySp(token);\n");
    printf("switch (id){\n");
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
    printf("\ndefault:{return -1;break;}");
    printf("}}\n");
}

RainbowLexerPrivate(void) RainbowLexerCompiler(const char* file_path) //总编译指令
{
    freopen(file_path,"w",stdout);
    printf("#include \"RainbowLexerSrc.c\"\n");
    RainbowCompileAllStatusLine();
    RainbowCompileAllStatusLineSp();
    RainbowCompileSpMatcher();
    #ifdef _WIN32
    freopen("CON","w",stdout);
    #else
    freopen("/dev/console","w",stdout);
    #endif
}
int test()
{
    RainBowLexer_id = 1;
    RainBowLexer_id_num = 2;
    RainBowLexer_id_var = 3;
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif
    if(compStatu == NULL)compStatu = StatuLineTable(HASH_TABLE_SIZE);
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
    // compStatu = StatuLineTable(HASH_TABLE_SIZE);//不能删
    test();
    putchar('\n');
    RainbowQueueINIT();
    char buf[5000] = {'\0'};
    // while(1)
    // {
    //     scanf("%[^~]",buf);
        // RainbowLex("asafa*WHILE++FOR as*****fgad");
    //     memset(buf,'\0',5000);
    // }
    // RainbowLexerCompiler("demoCompile____.c");
    // printf("compiled");
    // RainbowCompileAllStatusLine();
    // RainbowCompileAllStatusLineSp();
    // RainbowCompileSpMatcher();
    return 0;
}


//TODO
// 解决* ***的符号匹配
//目前问题 调试模式下可以正确匹配*  编译运行下无法匹配*