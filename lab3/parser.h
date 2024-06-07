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
    int token_index = 0, ebp = 0;
    std::map<std::string, int> identifier_map;

    void Error(std::string msg);
    int set_identifier_ebp(std::string identifier);
    std::string get_identifier_addr(std::string identifier);
    void push(Token token);
    void pop(Token token);
    void Expression(int left_token_index, int right_token_index);

    Parser(std::vector<Token> tokens);
};

#endif // PARSER_H