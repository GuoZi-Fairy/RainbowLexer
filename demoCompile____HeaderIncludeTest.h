#ifndef RAINBOWLEXER__h
#define DEMOCOMPILE____HEADERINCLUDETEST__H
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
size_t id;
}RainbowToken;
RainbowLexerPublic(void) RainbowLex(const char* string);
