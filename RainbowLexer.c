#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<ctype.h>
#include"RainbowLexer.h"
#include "RainbowLexerFront.h"
#ifdef _WIN32
    #include<Windows.h>
#endif
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
    long long id;
}RainbowToken;

typedef struct queue
{
    RainbowToken* queue;
    unsigned long long size;
    unsigned long long rear;
    unsigned long long front;
}Rainbowqueue;
static const char header[] = 
    "%s //DOUBLE OPTION\n"
    "%s //SINGLE OPTION\n"
    "#ifndef RAINBOWLEXER__h\n"
    "#define %s\n"
    "#define IGNORE_MIN (25526)\n"
    "#define IGNORE_MAX (35526)\n"
    "#ifdef _WIN32\n"
    "#define RainbowLexerPublic(type) extern type __cdecl\n"
    "#define RainbowLexerPrivate(type) static type __cdecl\n"
    "#else\n"
    "#define RainbowLexerPublic(type) extern type\n"
    "#define RainbowLexerPrivate(type) static type\n"
    "#endif\n"
    "#endif\n"
    "typedef struct __token__\n"
    "{\n"
    "char* token;\n"
    "int id;\n"
    "}RainbowToken;\n"
    "RainbowLexerPublic(void) RainbowLex(const char* string);\n"
    "RainbowLexerPublic(RainbowToken*) RainbowNext();\n"
    ;

static const char help[] = 
    "-c RL_File [C_object_File]\n\t[compile RL_File to C_objext_File]\n"
    "\t\t--if there is no C_object_File input, it will be compiled to the work path]\n"
    "-d [RL_File]\n\t[Debug the RL_File or get into Shell_MODE]\n"
    "\t\t--The Shell_MODE include command following:\n"
    "\t\t\tregSw [\"Sw_Word\"] [id/ignore] --This command will Register a Sw_Word with [id/be ignored]\n"
    "\t\t\tregSp [\"Sp_Word\"] [id/ignore] --This command will Register a Sp_Word wile [id/be ignored]\n"
    "\t\t\tLex [\"string\"] --This command will Lex the String by the rule you have Registered\n"
    "\t\t\tDel [id] --This command will Delete the id of rule\n"
    "\t\t\tShow --This command will display all the rules on screen\n";

static const RainbowError repeatedErr = {"Repeated tokenRule","Repeated token ->"};
static const RainbowError UndefineToken = {"Undefine token","UNdefine Token ->"};
static const RainbowError MallocError = {"fail malloc","failed to alloc memory"};
static const RainbowError StringError = {"Expected \"","can not match \""};
static const RainbowError SrcFileUnExist_Error = {"can not find the \"RainbowLexerSrc.c\"","please cheek the srcFile"};
#define RAINBOW_RAISE(ERROR) printf("ERROR:\n[%s]:%s\n",ERROR.errorMsg,ERROR.errorDoc);
static long long RainBowLexer_id;
static long long RainBowLexer_id_num;
static long long RainBowLexer_id_var;
static long long RainBowLexer_id_string;
static long long RainBowLexer_id_ignore = 25526;
#define RB_ignore (RainBowLexer_id_ignore++)
//ignore的id范围为[25526,35526]
#define IGNORE_MIN (25526)
#define IGNORE_MAX (35526)
static  const char* TokenRule_Now;
static int double_option;
static int single_option;
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
            if(*(token+1)=='\0' && (*statu)->id == INIT_ID)//提前判断 防止递归后错误判断重复情况
            {
                //该条件下 token已经完结 但statu无对应id 则该情况可创建
                (*statu)->id = RainBowLexer_id;
                return;
            }
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
    (*lastestStatu)->id = RainBowLexer_id;
}
RainbowLexerPrivate(void) RainbowCreateStatusLine(const char* token,int id) //将静态token转化为状态机并加入hash中
{
    TokenRule_Now = token;
    const char* parser = token;
    RainBowLexer_id = id;
    rStatu* statuLine =  StatuLineTable(*parser);
    if (!cheekStatusLine(*parser)) STATULINE_INIT(statuLine,*parser);
    if(*(parser+1)!='\0') RainbowStatuLineParse(&(statuLine->table),parser+1);
    else statuLine->id = RainBowLexer_id;
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
    (*lastestStatu)->id = RainBowLexer_id;
}
RainbowLexerPrivate(int) cheekStatusLineSp(unsigned int initCh) //检查以InitCh为索引的状态链是否存在
{
    rStatu* cheekRet = StatuLineTableSp(initCh);
    if((cheekRet->id == compStatu->id)&&(cheekRet->initChar == compStatu->initChar)&&(cheekRet->Next==compStatu->Next)&&(cheekRet->table == compStatu->table))return 0;
    return 1;
    
}
RainbowLexerPrivate(void) RainbowCreateStatusLineSp(const char* token,int id) //将静态token转化为状态机并加入hash中
{
    TokenRule_Now = token;
    const char* parser = token;
    RainBowLexer_id = id;
    rStatu* statuLine =  StatuLineTableSp(*parser);
    if (!cheekStatusLineSp(*parser)) STATULINE_INIT(statuLine,*parser);
    if(*(parser+1)!='\0') RainbowStatuLineParseSp(&(statuLine->table),parser+1);
    else statuLine->id = RainBowLexer_id;
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
RainbowLexerPrivate(RainbowToken*) RainbowNext()
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
            if((*strptr == '\"'&&double_option)||(*strptr == '\''&& single_option))//处理字符串
            {
                if(*buf!='\0')
                {
                    int id = 0;
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
                char* string = RainbowStatusCheekOfString(strptr+1,*strptr=='\"' ? '\"' : '\'');
                if(string == NULL)return;
                int len = strlen(string);
                strptr+=len+2;
                RainbowRetAdd(string,RainBowLexer_id_string);
                continue;
            }
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
        else if(status->initChar == '\'')printf("case \'\\\'\':{");
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
        else if(status->initChar == '\'')printf("case \'\\\'\':{");
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
    printf("RainbowLexerPrivate(int) RainbowStatusCheekOfStaticWordValidity(const char* token){\n");
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
    printf("RainbowLexerPrivate(int) RainbowStatusCheekOfStaticWordValiditySp(const char* token){\n");
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
RainbowLexerPrivate(int) RainbowCompileFilePathCheek(const char* file_path)
{
    int len = strlen(file_path);
    if((file_path[len-1]=='c'||file_path[len-1]=='c')&&(file_path[len-2]=='.'))return 1;
    else return 0;
}
RainbowLexerPrivate(void) RainbowCompileHeader(const char* file_path)
{
    char header_file_path[100] =  {'\0'};
    memcpy(header_file_path,file_path,strlen(file_path)* sizeof(char));
    for (size_t i = 0; i < 100; i++) header_file_path[i] = (header_file_path[i]==' ' ? '_':toupper(header_file_path[i]));
    strcat(header_file_path,"__H");
    printf(header,double_option?"#define DOUBLE_STRING_OPTION": "",single_option?"#define SINGLE_STRING_OPTION": "",header_file_path);
}
RainbowLexerPrivate(int) RainbowLexerCopySrc(const char* src_path)
{
    //将资源文件中的所有内容拷贝到编译文件中
    FILE* fp;
    if( (fp=fopen(src_path,"rt")) == NULL)
    {
        RAINBOW_RAISE(SrcFileUnExist_Error);
        return -1;
    }
    char ch;
    while( (ch=fgetc(fp)) != EOF ){
        putchar(ch);
    }
    return 0;
}
RainbowLexerPrivate(int) RainbowLexerCompiler(const char* file_path) //总编译指令
{
    char file[200] = {'\0'};
    strcpy(file,file_path);
    if (!RainbowCompileFilePathCheek(file_path)) strcat(file,".c");
    int len = strlen(file);
    freopen(file,"w",stdout);
    file[len - 1] = 'h';//头文件名
    printf("#include \"%s\"",file);

    if (RainbowLexerCopySrc("RainbowLexerSrc.c"))return -1;
    RainbowCompileAllStatusLine();
    RainbowCompileAllStatusLineSp();
    RainbowCompileSpMatcher();
    freopen(file,"w",stdout);
    file[len-2] = '\0';
    RainbowCompileHeader(file);
    #ifdef _WIN32
    freopen("CON","w",stdout);
    #else
    freopen("/dev/console","w",stdout);
    #endif
}


int FrontCompileConfig()
{
    RainBowLexer_id_num = 21;
    RainBowLexer_id_var = 22;
    //ignore的id范围为[25526,35526]
    RainBowLexer_id_string = 23;
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif
    RainbowCreateStatusLine("staticWord",1);
    RainbowCreateStatusLine("sw",1);
    RainbowCreateStatusLine("sperator",2);
    RainbowCreateStatusLine("sp",2);
    RainbowCreateStatusLine("IGNORE",3);
    RainbowCreateStatusLine("__VAR__",11);
    RainbowCreateStatusLine("STRING_SINGLE",12);
    RainbowCreateStatusLine("STRING_DOUBLE",13);
    RainbowCreateStatusLine("NUMBER",14);
    RainbowCreateStatusLineSp("{",11);
    RainbowCreateStatusLineSp("}",12);
    RainbowCreateStatusLineSp("[",13);
    RainbowCreateStatusLineSp("]",14);
    RainbowCreateStatusLineSp("-",15);
    RainbowCreateStatusLineSp(",",16);
    RainbowCreateStatusLineSp(":",17);
    RainbowCreateStatusLineSp("\"",18);
    RainbowCreateStatusLineSp(" ",RB_ignore);
    RainbowCreateStatusLineSp("\n",RB_ignore);
    RainbowCreateStatusLineSp("\t",RB_ignore);
    
    return 0;
}
// int mainForFront(int argc, char const *argv[])
// {
//         if(compStatu == NULL)compStatu = StatuLineTable(HASH_TABLE_SIZE);
//         FrontCompileConfig();
//         RainbowQueueINIT();
//         double_option = 1;
//         single_option = 1;
//         RainbowLexerCompiler("RainbowLexerFront");
//         printf("compiled");
//     return 0;
// }


#include "RainbowLexerFront.c"
//TODO : 完成解析文件和Shell操作
RainbowLexerPrivate(void) ParseFile(const char* file)
{

}

RainbowLexerPrivate(void) CompileFile(const char* file)
{

}

RainbowLexerPrivate(void) Shell()
{

}


int main(int argc, char const *argv[])
{
    if(argc == 1)//无参数时输出help菜单
    {
        printf("%s",help);
        return 0;
    }
    switch (argv[1][0])
    {
        case '-':
        {        
            switch (argv[1][1])
            {
                case 'C':
                case 'c':
                {
                    if(argc <= 2)
                    {
                        printf("There is no input file ^");
                        return 0;
                    }
                    CompileFile(argv[2]);
                    return 0;
                    break;
                }
                case 'D':
                case 'd':
                {
                    if(argc <=2)
                    {
                        Shell();
                        return 0;
                    }
                    ParseFile(argv[2]);
                    return 0;
                    break;
                }
                default:
                {
                    printf("Invaild Parameter %s",argv[1]);
                    return 0;
                    break;
                }
            }
            break;
        }
    }
    return 0;
}
