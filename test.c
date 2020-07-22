#include<stdio.h>
#include<string.h>
void command_Show_Line2(int length,const char* token,int maxLen,int id) 
{
    int tokenLength = (length) >> 1; //计算字符格占据的长度
    int idLength = length - tokenLength - 1;
    if(maxLen > tokenLength)maxLen = tokenLength;
    putchar('|');
    size_t i;
    for (i = 0; i < (maxLen-3) && token[i]!='\0';i++)
    {
        switch (token[i])
        {
        case '\n':
            putchar('\\');putchar('n');
            i++;
            break;
        
        default:
            putchar(token[i]);
            break;
        }
    }
    if(token[i] != '\0')
    {
        putchar('.');putchar('.');putchar('.');
    }
    else
    {
        int time = maxLen - i;
        for (size_t j = 0; j < time; j++)
        {
            putchar(' ');
        }
        
    }
    putchar('|');
    char idFormat[200] = {'\0'};
    sprintf(idFormat,"%%%d.d|\n",idLength);
    printf(idFormat,id);
}
void command_Show_Line1(int length,int ch) 
{
    for(size_t i = 0;i < length;i++)
        putchar(ch);
    putchar('\n');
}
#define min(a,b) (((a) < (b)) ? (a) : (b))
int command_Del_similarity(const char* str1, const char* str2)
{
    //若认为二者相似则返回1
    //不相似则返回0
    //使用edit distance算法
    static int ed_dp[200][200] = {0};

    int len1 = strlen(str1);
    int len2 = strlen(str2);
    //init
    for (int i = 1; i <= len1; i++)
        for (int j = 1; j <= len2; j++)
            ed_dp[i][j] = 0x9ffffff;

    for (int i = 1; i <= len1; i++)
        ed_dp[i][0] = i;

    for (int j = 1; j <= len2; j++)
        ed_dp[0][j] = j;

    for (int i = 1; i <= len1; i++)
    {
        for (int j = 1; j <= len2; j++)
        {
            int flag;
            if (str1[i] == str2[j])
                flag = 0;
            else
                flag = 1;
            ed_dp[i][j] = min(ed_dp[i - 1][j] + 1, min(ed_dp[i][j - 1] + 1, ed_dp[i - 1][j - 1] + flag));
        }
    }
    return ed_dp[len1][len2];
}
int main(int argc, char const *argv[])
{
    // char buf[2048] = {"hdas\0"};
    // command_Show_Line1(50,'=');
    // printf("|%32s%16s|\n","StaticWordTable"," ");
    // command_Show_Line1(50,'=');
    // command_Show_Line2(50,buf,23,25565);
    // command_Show_Line1(50,'=');
    printf("%d",command_Del_similarity("12asfasd3","13"));
    return 0;
}