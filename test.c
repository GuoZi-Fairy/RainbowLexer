#include<stdio.h>
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
int main(int argc, char const *argv[])
{
    char buf[2048] = {"hdas\0"};
    command_Show_Line1(50,'=');
    printf("|%32s%16s|\n","StaticWordTable"," ");
    command_Show_Line1(50,'=');
    command_Show_Line2(50,buf,23,25565);
    command_Show_Line1(50,'=');
    return 0;
}