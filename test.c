#include<stdio.h>
int test()
{
    static int id = 0;
    return id++;
}
int main(int argc, char const *argv[])
{
    for(int i = 0;i<10;i++)
    printf("%d\n",test());
    return 0;
}
