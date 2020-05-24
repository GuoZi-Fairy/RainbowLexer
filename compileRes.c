#include <stdio.h>
int test(char* ch)
{
switch (*ch++)
 {
case 'W':
{
switch (*ch++)
 {
case 'o':
{
switch (*ch++)
 {
case 'r':
{
switch (*ch++)
 {
case 'l':
{
switch (*ch++)
 {
case 'd':
{
switch (*ch++)
 {
case '4':
{
return 1;
break;
}
default:
{return -1;
}
}
break;
}
default:
{return -1;
}
}
break;
}
default:
{return -1;
}
}
break;
}
default:
{return -1;
}
}
break;
}case 'H':
{
switch (*ch++)
 {
case 'I':
{
switch (*ch++)
 {
case 'L':
{
switch (*ch++)
 {
case 'E':
{
return 1;
break;
}
default:
{return -1;
}
}
break;
}
default:
{return -1;
}
}
break;
}
default:
{return -1;
}
}
break;
}
default:
{return -1;
}
}
break;
}
default:
{return -1;
}
}
}
int main(int argc, char const *argv[])
{
    printf("%d",test("WHILE"));
    return 0;
}
