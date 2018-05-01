# TigerComplier
====================================

##flex
* flex tiger.lex            -->  lex.yy.c
* yywrap() 遇到EOF时返回1表示结束,返回0表示继续 默认自己实现一个返回1的就好
* yylex()   词法分析入口，根据词法规则，解析出一个符号就会返回一次，返回值是定义好的类型枚举值。（需要用到bison生成的符号头文件y.tab.h）
* 大部分符号不用特殊处理，但是数字和变量需要将值解析到yylval这个全局union变量中（yylval这个变量的类型也是bison根据语法规则生成的）

##bison
* bison tiger.grm -ydv      --> y.output | y.tab.h | y.tab.c
* yyparse() 语法分析入口, 内部实现会调用yylex()
