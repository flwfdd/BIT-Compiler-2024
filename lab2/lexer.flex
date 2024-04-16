/* %option noyywrap 功能较为复杂，同学们自行了解 */
%option noyywrap
%{
/* Flex 源程序采样类 C 的语法和规则 */
/* 以下是声明部分，`%{` 和 `%}` 之间的内容会被原样复制到生成的 C 文件头部
    包括该条注释内容 */
#include <cstring>
#include <vector>
#include "lexer.h"

std::vector<Token> *tks;

%}

/* 以下是规则部分，在规则部分写注释不能顶格写 */
/* 每条规则由正则表达式和动作组成 */
%%
 /* 在规则部分，不要顶格写注释 */
 /* int and return*/
int|return|main { tks->push_back(Token(Token::TokenType::KEYWORD, yytext));}
[a-zA-Z_][a-zA-Z0-9_]* { tks->push_back(Token(Token::TokenType::IDENTIFIER, yytext));}
[0-9]+ { tks->push_back(Token(Token::TokenType::CONSTANT, yytext));}
=|\+|-|\*|\/|%|<|<=|>|>=|==|!=|&|[|]|\^ { tks->push_back(Token(Token::TokenType::OPERATOR, yytext));}
[,;(){}] { tks->push_back(Token(Token::TokenType::PUNCTUATOR, yytext));}
[ \t\n] { /* ignore whitespace */ }
%%

Lexer::Lexer(std::string src){
    tks = &tokens;
    yy_scan_string(src.c_str());
    yylex();
}
