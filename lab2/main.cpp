/*
 * @Author: flwfdd
 * @Date: 2024-04-16 17:07:34
 * @LastEditTime: 2024-04-16 22:44:47
 * @Description: 
 * _(:з」∠)_
 */
#include <cstdio>
#include <regex>
#include <string>
#include <iostream>
#include <map>
#include <stack>

#include "lexer.h"
#include "parser.h"

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        // 重定向文件输入
        freopen(argv[1], "r", stdin);
    }
    std::string src, tmp;
    while (std::getline(std::cin, tmp))
    {
        src += tmp + "\n";
    }
    Lexer lexer(src);
    // for(auto token : lexer.tokens)
    // {
    //     std::cout << token << std::endl;
    // }
    Parser parser(lexer.tokens);
    std::cout<<parser.out;
    return 0;
}