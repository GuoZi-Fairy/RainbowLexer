 //DOUBLE OPTION
 //SINGLE OPTION
#define COMMANDER__H
#define DOUBLE_STRING_OPTION
#define IGNORE_MIN (LONG_MAX-10000)
#define IGNORE_MAX (LONG_MAX)
#ifdef _WIN32
#define RainbowLexerPublic(type) extern type __cdecl
#define RainbowLexerPrivate(type) static type __cdecl
#else
#define RainbowLexerPublic(type) extern type
#define RainbowLexerPrivate(type) static type
#endif
typedef struct __Commandtoken__
{
char* token;
long long id;
}RainbowCommandToken;
RainbowLexerPublic(void) RainbowCommanderLex(const char* string);
RainbowLexerPublic(RainbowCommandToken*) RainbowCommanderNext();
RainbowLexerPublic(void) RainbowCommanderQueueClear();
#define ID_NUM (12)
#define ID_VAR (-1)
#define ID_STRING (11)
