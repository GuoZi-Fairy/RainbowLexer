#include <stdio.h>
#include "RainbowLexer.h"
RainbowLexerPublic(int) RainbowStatusCheekOfStaticWordValidity(const char* token)
{
switch (*token++) 
 {
case 'F':
{
switch (*token++) 
 {
case 'L':
{
switch (*token++) 
 {
case 'O':
{
switch (*token++)
 {
case 'A':
{
switch (*token++) 
 {
case 'T':
{
switch (*token++)
 {
case '3':
{
return 4;break;
break;
}
case '6':
{
return 5;break;
break;
}

default:
{
return -1;
}
}

break;
}

default:
{
return -1;
}
}

break;
}

default:
{
return -1;
}
}

break;
}

default:
{
return -1;
}
}

break;
}
case 'O':
{
return 6;break;
break;
}

default:
{
return -1;
}
}

}
case 'I':
{
switch (*token++)
 {
case 'N':
{
switch (*token++) 
 {
case 'T':
{
switch (*token++)
 {
case '3':
{
return 2;break;
break;
}
case '6':
{
return 3;break;
break;
}

default:
{
return -1;
}
}

break;
}

default:
{
return -1;
}
}

break;
}

default:
{
return -1;
}
}

}
case 'W':
{
switch (*token++)
 {
case 'o':
{
switch (*token++)
 {
case 'r':
{
switch (*token++) 
 {
case 'l':
{
switch (*token++)
 {
case 'd':
{
return 1;break;
break;
}

default:
{
return -1;
}
}

break;
}

default:
{
return -1;
}
}

break;
}

default:
{
return -1;
}
}

break;
}
case 'H':
{
switch (*token++)
 {
case 'I':
{
switch (*token++)
 {
case 'L':
{
return 7;break;
break;
}

default:
{
return -1;
}
}

break;
}

default:
{
return -1;
}
}

break;
}

default:
{
return -1;
}
}

}
}
}

RainbowLexerPublic(int) RainbowStatusCheekOfStaticWordValiditySp(const char* token)
{
switch (*token++)
 {
case '\n':
{
return 18;break;
}
case ' ':
{
return 19;break;
}
case '(':
{
return 10;break;
}
case ')':
{
return 11;break;
}
case '*':
{
switch (*token++)
 {
case '*':
{
return 16;break;
break;
}

default:
{
return -1;
}
}

}
case '+':
{
return 9;break;
}
case '-':
{
return 8;break;
}
case '/':
{
return 13;break;
}
case '[':
{
return 14;break;
}
case ']':
{
return 15;break;
}
case 'x':
{
switch (*token++)
 {
case 'x':
{
return 17;break;
break;
}

default:
{
return -1;
}
}

}
}
}
RainbowLexerPrivate(int) RainbowStatuSperatorMatch(const char* token)
{
int id = RainbowStatusCheekOfStaticWordValiditySp(token);
switch (id)
{
case 18:
{
return 0;
break;
}
case 19:
{
return 0;
break;
}
case 10:
{
return 0;
break;
}
case 11:
{
return 0;
break;
}
case 16:
{
return 2;
break;
}
case 9:
{
return 1;
break;
}
case 8:
{
return 1;
break;
}
case 13:
{
return 0;
break;
}
case 14:
{
return 0;
break;
}
case 15:
{
return 0;
break;
}
case 17:
{
return 2;
break;
}

default:
{
return -1;
break;
}
}
}
int main(int argc, char const *argv[])
{
    printf("%d\n",RainbowStatusCheekOfStaticWordValidity("WHILE"));
    printf("%d\n",RainbowStatusCheekOfStaticWordValidity("INT64"));
    printf("%d\n",RainbowStatuSperatorMatch("++"));
    return 0;
}
