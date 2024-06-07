/*
 * @Author: flwfdd
 * @Date: 2024-06-07 13:45:45
 * @LastEditTime: 2024-06-07 20:56:44
 * @Description: 
 * _(:з」∠)_
 */
#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "lexer.h"

class Parser
{
public:
    std::vector<Token> tokens;
    std::string out;
    int ebp = 0;
    std::map<std::string, int> identifier_map;

    void Error(std::string msg);
    int set_identifier_ebp(std::string identifier);
    std::string get_identifier_addr(std::string identifier);
    void push(Token token);
    void pop(Token token);
    void Expression(int left_token_index, int right_token_index);
    void FunctionBody(int left_token_index, int right_token_index);

    Parser(std::vector<Token> tokens);
};

#endif // PARSER_H