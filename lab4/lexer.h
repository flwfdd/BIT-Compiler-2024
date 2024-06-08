/*
 * @Author: flwfdd
 * @Date: 2024-04-16 17:45:08
 * @LastEditTime: 2024-06-07 21:05:06
 * @Description:
 * _(:з」∠)_
 */
#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <vector>

class Token
{
public:
    // 词法单元类型
    enum class TokenType
    {
        KEYWORD, // void, int, return
        IDENTIFIER,
        CONSTANT,
        OPERATOR,   // =, +, -, *, /, %, <, <=, >, >=, ==, !=, &, |, ^
        PUNCTUATOR, // ;, {, }, (, )
    };

    // 重载输出运算符
    friend std::ostream &operator<<(std::ostream &os, const Token &token)
    {
        os << "Token: " << token.value << " Type: ";
        switch (token.type)
        {
        case TokenType::KEYWORD:
            os << "KEYWORD";
            break;
        case TokenType::IDENTIFIER:
            os << "IDENTIFIER";
            break;
        case TokenType::CONSTANT:
            os << "CONSTANT";
            break;
        case TokenType::OPERATOR:
            os << "OPERATOR";
            break;
        case TokenType::PUNCTUATOR:
            os << "PUNCTUATOR";
            break;
        default:
            os << "UNKNOWN";
            break;
        }
        return os;
    }

    TokenType type;
    std::string value;
    Token(TokenType type, std::string value)
    {
        this->type = type;
        this->value = value;
    }
};

// 词法分析器
class Lexer
{
public:
    std::vector<Token> tokens;
    Lexer(std::string src);
};

#endif // LEXER_H