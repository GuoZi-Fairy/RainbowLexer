#ifndef RAINBOWLEXER__h
    #define RAINBOWLEXER__h
    #ifdef _WIN32
        #define RainbowLexerPublic(type) extern type __cdecl
        #define RainbowLexerPrivate(type) static type __cdecl
    #else
        #define RainbowLexerPublic(type) extern type 
        #define RainbowLexerPrivate(type) static type
    #endif
#endif