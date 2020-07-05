 //DOUBLE OPTION
 //SINGLE OPTION
#ifndef RAINBOWLEXER__h
#define TEST.RL__H
#define IGNORE_MIN (25526)
#define IGNORE_MAX (35526)
#ifdef _WIN32
#define RainbowLexerPublic(type) extern type __cdecl
#define RainbowLexerPrivate(type) static type __cdecl
#else
#define RainbowLexerPublic(type) extern type
#define RainbowLexerPrivate(type) static type
#endif
#endif
typedef struct __token__
{
char* token;
long long id;
}RainbowToken;
RainbowLexerPublic(void) RainbowLex(const char* string);
RainbowLexerPublic(RainbowToken*) RainbowNext();
