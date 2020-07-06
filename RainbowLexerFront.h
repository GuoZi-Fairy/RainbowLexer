#ifdef _WIN32
#define RainbowLexerPublic(type) extern type __cdecl
#define RainbowLexerPrivate(type) static type __cdecl
#else
#define RainbowLexerPublic(type) extern type
#define RainbowLexerPrivate(type) static type
#endif
RainbowLexerPublic(long long) RainbowFrontStatuSperatorMatch(const char *token);
RainbowLexerPublic(long long) RainbowFrontStatusCheekOfStaticWordValiditySp(const char *token);
RainbowLexerPublic(long long) RainbowFrontStatusCheekOfStaticWordValidity(const char *token);
RainbowLexerPublic(void) RainbowFrontLex(const char* string);