#include<stdio.h>
int main(int argc, char const *argv[])
{
    char buf[2048] = {'\0'};
    scanf("%[regSw|regSp|Del|Show|Lex|Help|Quit]",buf);
    printf("%s\n",buf);
    return 0;
}
