# RainbowLexer
## 简介📃
RainbowLexer是一个可配置的词法分析器💾\
RainbowLexer接收一个RainbowLexer可识别的规则描述文档[*.RL文件📃]\
RainbowLexer可以基于 *RL文件所描述的规则* 调试你的 *被解析目标文件*\
RainbowLexer可以编译出对应 *RL文件所描述的规则* 对应的C文件，其中包含可以解析对应文档的C代码.
## 目录
- [RainbowLexer](#rainbowlexer)
  - [简介📃](#%e7%ae%80%e4%bb%8b%f0%9f%93%83)
  - [目录](#%e7%9b%ae%e5%bd%95)
  - [RL文件描述规则✏](#rl%e6%96%87%e4%bb%b6%e6%8f%8f%e8%bf%b0%e8%a7%84%e5%88%99%e2%9c%8f)
    - [例如](#%e4%be%8b%e5%a6%82)
    - [正则表达式描述](#%e6%ad%a3%e5%88%99%e8%a1%a8%e8%be%be%e5%bc%8f%e6%8f%8f%e8%bf%b0)
  - [使用方法](#%e4%bd%bf%e7%94%a8%e6%96%b9%e6%b3%95)
    - [编译文件使用说明](#%e7%bc%96%e8%af%91%e6%96%87%e4%bb%b6%e4%bd%bf%e7%94%a8%e8%af%b4%e6%98%8e)
  - [开发手册](#%e5%bc%80%e5%8f%91%e6%89%8b%e5%86%8c)
  - [开源声明](#%e5%bc%80%e6%ba%90%e5%a3%b0%e6%98%8e)
  - [帮助](#%e5%b8%ae%e5%8a%a9)
  - [](#)


## RL文件描述规则✏
每一个词法描述单元应以如下方式定义\
```
id-name-[sperator/static] {
    ...
}
```
### 例如
```
1-keyword_for-static {
    for
}
2-keyword_while-static {
    while
}
3-keyword_define-static {
    define
}
4-var-static {
    {__VAR__}
}
```
提供了一些已经定义的集合描述\
* __VAR__ 通用合法变量名集合
* __NUM_INT__ 通用合法整数集合
* __NUM_FLOAT__ 通用合法浮点数集合
* __STRING_DOUBLE__ 通用双引号包括字符串
* __STRING_SINGLE__ 通用单引号包括字符串
* __WORD__ 任意纯字母集合
若希望引用上述集合，请在描述中以`{}`包括上述集合
\
对单一单词的集合可以简写为
```
id-name-[sperator/static] token
```

### 正则表达式描述
暂未添加
## 使用方法
在命令行中使用\
参数列表\
* -DEBUG [file.RL] >>开启调试模式(**不编译，可以单句为单位解析，也可以读入文件**)
* -COMPILE file.RL outputFile.c >>编译模式(**将file.RL编译为outputfile.c**)

### 编译文件使用说明
对于编译所得的C文件,其中包含3个接口
1. (void)RainbowLex(const char* string)解析string文件
2. (token*)RainboNext() 顺序返回一个token结构,包含(**id**[size_t],**token**[char*])
3. FINAL() 放弃所有token,直接结束解析

## 开发手册
略

## 开源声明
MIT License

>  Copyright (c) 2020-* GuoZi
>
>  Permission is hereby granted, free of charge, to any person obtaining a copy
>  of this software and associated documentation files (the "Software"), to deal
>  in the Software without restriction, including without limitation the rights
>  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>  copies of the Software, and to permit persons to whom the Software is
>  furnished to do so, subject to the following conditions:
>
>  The above copyright notice and this permission notice shall be included in
>  all copies or substantial portions of the Software.
>
>  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
>  THE SOFTWARE.

## 帮助
如果在使用RainbowLexer的过程中遇到了问题，您可以在此处或issue中寻找解决方法
## 
- 果子 (original author)