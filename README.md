# RainbowLexer
## 简介📃
RainbowLexer是一个可配置的词法分析器💾\
RainbowLexer接收一个RainbowLexer可识别的规则描述文档[*.RL文件📃]\
RainbowLexer可以基于 *RL文件所描述的规则* 调试你的 *被解析目标文件*\
RainbowLexer可以编译出对应 *RL文件所描述的规则* 对应的C文件，其中包含可以解析对应文档的C代码.
## 目录
- [RainbowLexer](#rainbowlexer)
  - [简介📃](#简介)
  - [目录](#目录)
  - [RL文件描述规则✏](#rl文件描述规则)
    - [例如](#例如)
  - [使用方法](#使用方法)
    - [编译文件使用说明](#编译文件使用说明)
  - [开发手册](#开发手册)
  - [开源声明](#开源声明)
  - [帮助](#帮助)
  - [](#)


## RL文件描述规则✏
每一个词法描述单元应以如下方式定义\
```C
type union index_series;
// 说明: 
// type 为 静态词和分隔词
//    静态词type为 sw/staticWord
//    分隔词type为 sp/sperator
// union 为 集合单词
// index_series 为id序列
//    例如[1,2,3,4]对应4个词素id分别为1，2，3，4
//        [4:]表示任意数量词素 id 从4递增
//        [:4]表示任意数量词素 id 从4逆向递减
//        [:]表示任意数量词素  id 从0递增
//        IGNORE 表示该词素不会进入结果列表
```
### 例如
```
sw {"hello","world","test","hel"} [1,2,3,4];
sw {"dididi","hahaha","ppp",__VAR__} [4:];
sp {"%","/","*"} [20:];
sp {" ","\n","\t"} IGNORE;
}
```
需要注意的是 任何词素都需要用" "包裹\
提供了一些已经定义的集合描述\
* __VAR__ 通用合法变量名集合
* NUMBER 通用合法数集合
* __STRING_DOUBLE__ 通用双引号包括字符串
* __STRING_SINGLE__ 通用单引号包括字符串
若希望引用上述集合，请在描述中以`{}`包括上述集合
\
对单一单词的集合可以简写为
```
type "token" id
```

## 使用方法
在命令行中使用\
参数列表\
* -D [file.RL] >>开启调试模式(**不编译，可以单句为单位解析，也可以读入文件**)
* -C file.RL >>编译模式(**将file.RL编译为outputfile.c**)

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