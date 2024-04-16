/*
 * @Author: flwfdd
 * @Date: 2024-04-16 20:31:13
 * @LastEditTime: 2024-04-16 23:51:16
 * @Description:
 * _(:з」∠)_
 */
#include <cstdio>
#include <regex>
#include <string>
#include <iostream>
#include <map>
#include <stack>

#include "parser.h"

std::string asm_template = R"(
.intel_syntax noprefix
.global main
.extern printf

.data
format_str:
    .asciz "%d\n"

.text
main:
    push ebp
    mov ebp, esp
    sub esp, 0x100

    ##REPLACEME##

    pop eax
    leave
    ret
)";

// 错误处理
void Parser::Error(std::string msg)
{
    std::cerr << "Error: " << msg << std::endl;
    exit(1);
}

// 设置标识符的ebp偏移
int Parser::set_identifier_ebp(std::string identifier)
{
    if (identifier_map.find(identifier) != identifier_map.end())
    {
        Error("Identifier already exists");
        return -1;
    }
    else
    {
        ebp += 4;
        identifier_map[identifier] = ebp;
        return ebp;
    }
}

// 获取标识符的地址
std::string Parser::get_identifier_addr(std::string identifier)
{
    if (identifier_map.find(identifier) != identifier_map.end())
    {
        return "DWORD PTR [ebp-" + std::to_string(identifier_map[identifier]) + "]";
    }
    else
    {
        Error("Identifier not found");
    }
    return "";
}

// 入栈
void Parser::push(Token token)
{
    if (token.type == Token::TokenType::CONSTANT)
    {
        out += "push " + token.value + "\n";
    }
    else if (token.type == Token::TokenType::IDENTIFIER)
    {
        out += "mov eax, " + get_identifier_addr(token.value) + "  # push " + token.value + "\n";
        out += "push eax\n";
    }
    else
    {
        Error("Expected push identifier or constant");
    }
}

// 出栈到标识符
void Parser::pop(Token token)
{
    if (token.type == Token::TokenType::IDENTIFIER)
    {
        out += "pop eax\n";
        out += "mov " + get_identifier_addr(token.value) + ", eax  # pop " + token.value + "\n";
    }
    else
    {
        Error("Expected pop identifier");
    }
}

// 表达式求值
void Parser::Expression(int left_token_index, int right_token_index)
{
    // 栈中存放的是token的索引 -1为内存栈顶
    std::stack<int> operator_stack;
    std::stack<int> operand_stack;

    // 优先级表 https://zh.cppreference.com/w/cpp/language/operator_precedence
    std::map<std::string, int> priority;
    priority["*"] = 5;
    priority["/"] = 5;
    priority["%"] = 5;
    priority["+"] = 6;
    priority["-"] = 6;
    priority["<"] = 9;
    priority["<="] = 9;
    priority[">"] = 9;
    priority[">="] = 9;
    priority["=="] = 10;
    priority["!="] = 10;
    priority["&"] = 11;
    priority["^"] = 12;
    priority["|"] = 13;

    // 一次求值 取出栈顶两个操作数和一个操作符 放回栈顶一个操作数
    auto eval_once = [&]()
    {
        // 右操作数
        int right = operand_stack.top();
        operand_stack.pop();
        if (right == -1)
            out += "pop ebx  # right operand from stack\n";
        else if (tokens[right].type == Token::TokenType::IDENTIFIER)
            out += "mov ebx, " + get_identifier_addr(tokens[right].value) + "  # right operand " + tokens[right].value + "\n";
        else
            out += "mov ebx, " + tokens[right].value + "  # right operand " + tokens[right].value + "\n";

        // 左操作数
        int left = operand_stack.top();
        operand_stack.pop();
        if (left == -1)
            out += "pop eax  # left operand from stack\n";
        else if (tokens[left].type == Token::TokenType::IDENTIFIER)
            out += "mov eax, " + get_identifier_addr(tokens[left].value) + "  # left operand " + tokens[left].value + "\n";
        else
            out += "mov eax, " + tokens[left].value + "  # left operand " + tokens[left].value + "\n";

        // 操作符
        int op = operator_stack.top();
        operator_stack.pop();
        if (tokens[op].value == "+")
            out += "add eax, ebx\n";
        else if (tokens[op].value == "-")
            out += "sub eax, ebx\n";
        else if (tokens[op].value == "*")
            out += "imul eax, ebx\n";
        else if (tokens[op].value == "/")
        {
            out += "cdq\n";
            out += "idiv ebx\n";
        }
        else if (tokens[op].value == "%")
        {
            out += "cdq\n";
            out += "idiv ebx\n";
            out += "mov eax, edx\n";
        }
        else if (tokens[op].value == "<")
        {
            out += "cmp eax, ebx\n";
            out += "setl al\n";
            out += "movzx eax, al\n";
        }
        else if (tokens[op].value == "<=")
        {
            out += "cmp eax, ebx\n";
            out += "setle al\n";
            out += "movzx eax, al\n";
        }
        else if (tokens[op].value == ">")
        {
            out += "cmp eax, ebx\n";
            out += "setg al\n";
            out += "movzx eax, al\n";
        }
        else if (tokens[op].value == ">=")
        {
            out += "cmp eax, ebx\n";
            out += "setge al\n";
            out += "movzx eax, al\n";
        }
        else if (tokens[op].value == "==")
        {
            out += "cmp eax, ebx\n";
            out += "sete al\n";
            out += "movzx eax, al\n";
        }
        else if (tokens[op].value == "!=")
        {
            out += "cmp eax, ebx\n";
            out += "setne al\n";
            out += "movzx eax, al\n";
        }
        else if (tokens[op].value == "&")
        {
            out += "and eax, ebx\n";
        }
        else if (tokens[op].value == "|")
        {
            out += "or eax, ebx\n";
        }
        else if (tokens[op].value == "^")
        {
            out += "xor eax, ebx\n";
        }
        else
        {
            Error("Unknown operator: " + tokens[op].value);
        }

        operand_stack.push(-1);
        out += "push eax  # push result\n";
    };

    out += "\n# Expression\n";
    for (int i = left_token_index; i <= right_token_index; i++)
    {
        // std::cout<<tokens[i]<<std::endl;
        if (tokens[i].type == Token::TokenType::OPERATOR)
        {
            if (priority.count(tokens[i].value) == 0)
            {
                Error("Unknown operator: " + tokens[i].value);
            }

            while (!operator_stack.empty() && priority[tokens[operator_stack.top()].value] <= priority[tokens[i].value] && tokens[operator_stack.top()].value != "(")
            {
                eval_once();
            }
            operator_stack.push(i);
        }
        else if (tokens[i].type == Token::TokenType::PUNCTUATOR)
        {
            if (tokens[i].value == "(")
            {
                operator_stack.push(i);
            }
            else if (tokens[i].value == ")")
            {
                while (tokens[operator_stack.top()].value != "(")
                {
                    eval_once();
                }
                operator_stack.pop();
            }
            else
            {
                Error("Unknown punctuator: " + tokens[i].value);
            }
        }
        else if (tokens[i].type == Token::TokenType::IDENTIFIER || tokens[i].type == Token::TokenType::CONSTANT)
        {
            operand_stack.push(i);
        }
        else
        {
            Error("Expected operator or identifier or constant");
        }
    }
    while (!operator_stack.empty())
    {
        eval_once();
    }
    if (operand_stack.size() == 1)
    {
        if (operand_stack.top() == -1)
            out += "pop eax  # expression result\n";
        else if (tokens[operand_stack.top()].type == Token::TokenType::IDENTIFIER)
            out += "mov eax, " + get_identifier_addr(tokens[operand_stack.top()].value) + "  # expression result " + tokens[operand_stack.top()].value + "\n";
        else
            out += "mov eax, " + tokens[operand_stack.top()].value + "  # expression result " + tokens[operand_stack.top()].value + "\n";
    }
    else
    {
        Error("Expression result error");
    }
}

Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;
    int block_count = 0;
    for (std::vector<Token>::size_type i = 0; i < tokens.size(); i++)
    {
        // std::cout << tokens[i] << std::endl;
        if (tokens[i].type == Token::TokenType::KEYWORD)
        {
            if (tokens[i].value == "int")
            {
                if (tokens[i + 1].type == Token::TokenType::IDENTIFIER && tokens[i + 2].value == ";")
                {
                    ebp += 4;
                    identifier_map[tokens[i + 1].value] = ebp;
                    out += "mov DWORD PTR [ebp-" + std::to_string(ebp) + "], 0  # int " + tokens[i + 1].value + "\n";
                    i += 2;
                }
                else if (tokens[i + 1].type == Token::TokenType::KEYWORD && tokens[i + 1].value == "main")
                {
                    while (i + 1 < tokens.size() && tokens[i + 1].value != "{")
                        i++;
                }
                else
                {
                    Error("Expected identifier after int");
                }
            }
            else if (tokens[i].value == "return")
            {
                int j = i + 1;
                while (tokens[j].value != ";")
                    j++;
                Expression(i + 1, j - 1);
                i = j;
                out += "# return eax\n";
            }
            else
            {
                std::cerr << "Unknown keyword: " << tokens[i].value << std::endl;
                exit(1);
            }
        }
        else if (tokens[i].type == Token::TokenType::IDENTIFIER)
        {
            if (tokens[i + 1].value == "(" && tokens[i + 3].value == ")")
            {
                if(tokens[i].value == "println_int")
                {
                    Expression(i + 2, i + 2);
                    out += "push eax\n";
                    out += "push offset format_str\n";
                    out += "call printf\n";
                    out += "add esp, 8\n";
                    out += "pop eax\n";
                    i += 3;
                }
                else
                {
                    Error("Unknown function: " + tokens[i].value);
                }
            }
            else if (tokens[i + 1].value == "=")
            {
                int j = i + 2;
                while (tokens[j].value != ";")
                    j++;
                Expression(i + 2, j - 1);
                out += "mov " + get_identifier_addr(tokens[i].value) + ", eax  # " + tokens[i].value + " = eax\n";
                i = j;
            }
            else
            {
                Error("Expected = after identifier");
            }
        }
        else if (tokens[i].type == Token::TokenType::CONSTANT)
        {
            Error("Unexpected constant");
        }
        else if (tokens[i].type == Token::TokenType::OPERATOR)
        {
            Error("Unexpected operator");
        }
        else if (tokens[i].type == Token::TokenType::PUNCTUATOR)
        {
            if (tokens[i].value == ";")
            {
                out += "\n";
            }
            else if (tokens[i].value == "{")
            {
                block_count++;
            }
            else if (tokens[i].value == "}")
            {

                if (block_count > 0)
                {
                    block_count--;
                }
                else
                {
                    Error("Unexpected }");
                }
            }
            else
            {
                Error("Unexpected separator");
            }
        }
    }

    // 插入汇编模板中
    out = asm_template.replace(asm_template.find("##REPLACEME##"), 12, out);
}
