%option noyywrap
%{
#include <cstring>
#include <vector>
#include "lexer.h"
std::vector<Token> *tks;
%}

%%
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
