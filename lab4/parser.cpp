/*
 * @Author: flwfdd
 * @Date: 2024-04-16 20:31:13
 * @LastEditTime: 2024-06-08 14:30:23
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

##REPLACEME##

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
        if (identifier_map[identifier] > 0)
            return "DWORD PTR [ebp-" + std::to_string(identifier_map[identifier]) + "]";
        else
            return "DWORD PTR [ebp+" + std::to_string(-identifier_map[identifier]) + "]";
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
    priority["-"] = 2; // 一元负号
    priority["!"] = 3;
    priority["~"] = 3;
    priority["*"] = 5;
    priority["/"] = 5;
    priority["%"] = 5;
    priority["+"] = 6;
    priority["--"] = 6; // 二元减法
    priority["<"] = 9;
    priority["<="] = 9;
    priority[">"] = 9;
    priority[">="] = 9;
    priority["=="] = 10;
    priority["!="] = 10;
    priority["&"] = 11;
    priority["^"] = 12;
    priority["|"] = 13;
    priority["&&"] = 14;
    priority["||"] = 15;

    // 一次求值 取出栈顶两个操作数和一个操作符 放回栈顶一个操作数
    auto eval_once = [&]()
    {
        // std::cout<<tokens[operator_stack.top()]<<" "<<operand_stack.size()<<std::endl;
        // 一元操作符
        if (tokens[operator_stack.top()].value == "!" || tokens[operator_stack.top()].value == "~" || tokens[operator_stack.top()].value == "-")
        {
            // 取操作数
            int right = operand_stack.top();
            operand_stack.pop();
            if (right == -1)
                out += "pop eax  # right operand from stack\n";
            else if (tokens[right].type == Token::TokenType::IDENTIFIER)
                out += "mov eax, " + get_identifier_addr(tokens[right].value) + "  # right operand " + tokens[right].value + "\n";
            else
                out += "mov eax, " + tokens[right].value + "  # right operand " + tokens[right].value + "\n";

            // 操作符
            int op = operator_stack.top();
            operator_stack.pop();
            if (tokens[op].value == "!")
            {
                out += "test eax, eax\n";
                out += "setz al\n";
                out += "movzx eax, al\n";
            }
            else if (tokens[op].value == "~")
            {
                out += "not eax\n";
            }
            else if (tokens[op].value == "-")
            {
                out += "neg eax\n";
            }
            operand_stack.push(-1);
            out += "push eax  # push result\n";
            return;
        }

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
        else if (tokens[op].value == "--")
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
        else if (tokens[op].value == "&&")
        {
            out += "test eax, eax\n";
            out += "setnz al\n";
            out += "test ebx, ebx\n";
            out += "setnz bl\n";
            out += "and al, bl\n";
            out += "movzx eax, al\n";
        }
        else if (tokens[op].value == "||")
        {
            out += "test eax, eax\n";
            out += "setnz al\n";
            out += "test ebx, ebx\n";
            out += "setnz bl\n";
            out += "or al, bl\n";
            out += "movzx eax, al\n";
        }
        else
        {
            Error("Unknown operator: " + tokens[op].value);
        }

        operand_stack.push(-1);
        out += "push eax  # push result\n";
    };

    out += "# Expression ";
    for (int i = left_token_index; i <= right_token_index; i++)
    {
        out += tokens[i].value + " ";
    }
    out += "\n";
    for (int i = left_token_index; i <= right_token_index; i++)
    {
        if (tokens[i].type == Token::TokenType::OPERATOR)
        {
            if (priority.count(tokens[i].value) == 0)
            {
                Error("Unknown operator: " + tokens[i].value);
            }

            // 区分一元负号和二元减法
            if (tokens[i].value == "-" && i != left_token_index && (tokens[i - 1].type == Token::TokenType::CONSTANT || tokens[i - 1].type == Token::TokenType::IDENTIFIER || tokens[i - 1].value == ")"))
            {
                tokens[i].value = "--";
            }

            // 区分一元和二元操作符
            if (tokens[i].value != "-" && tokens[i].value != "!" && tokens[i].value != "~")
            {
                while (!operator_stack.empty() && priority[tokens[operator_stack.top()].value] <= priority[tokens[i].value] && tokens[operator_stack.top()].value != "(")
                {
                    eval_once();
                }
            }

            operator_stack.push(i);
            // std::cout<<tokens[i]<<std::endl;
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
        else if (tokens[i].type == Token::TokenType::IDENTIFIER)
        {
            if (i != right_token_index && tokens[i + 1].value == "(")
            {
                int j = i + 1, bracket_count = 0;
                while (1)
                {
                    if (tokens[j].value == "(")
                        bracket_count++;
                    if (tokens[j].value == ")")
                    {
                        bracket_count--;
                        if (bracket_count == 0)
                            break;
                    }
                    j++;
                }
                CallFunction(i, j);
                operand_stack.push(-1);
                out += "push eax  # push function result\n";
                i = j;
            }
            else
                operand_stack.push(i);
        }
        else if (tokens[i].type == Token::TokenType::CONSTANT)
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
        std::cout << out << std::endl;
        Error("Expression result error");
    }
}

void Parser::CallFunction(int left_token_index, int right_token_index)
{
    if (tokens[left_token_index].value == "println_int")
    {
        int j = left_token_index + 1, bracket_count = 0;
        while (1)
        {
            if (tokens[j].value == "(")
                bracket_count++;
            if (tokens[j].value == ")")
            {
                bracket_count--;
                if (bracket_count == 0)
                    break;
            }
            j++;
        }
        Expression(left_token_index + 2, j - 1);
        out += "push eax\n";
        out += "push offset format_str\n";
        out += "call printf\n";
        out += "add esp, 8\n";
        out += "pop eax\n";
    }
    else
    {
        out += "# call " + tokens[left_token_index].value + "\n";
        std::vector<std::pair<int, int>> args;
        int exp_start = left_token_index + 2, j = left_token_index + 1, bracket_count = 0;
        while (1)
        {
            if (tokens[j].value == "(")
                bracket_count++;
            if (bracket_count == 1 && (tokens[j].value == "," || tokens[j].value == ")"))
            {
                args.push_back({exp_start, j - 1});
                exp_start = j + 1;
            }
            if (tokens[j].value == ")")
            {
                bracket_count--;
                if (bracket_count == 0)
                    break;
            }
            j++;
        }
        int args_count = args.size();
        while (!args.empty())
        {
            Expression(args.back().first, args.back().second);
            args.pop_back();
            out += "push eax\n";
        }
        out += "call " + tokens[left_token_index].value + "\n";
        out += "add esp, " + std::to_string(args_count * 4) + "\n";
    }
}

// return true if find break
bool Parser::BlockBody(int left_token_index, int right_token_index)
{
    int block_count = 0;
    for (int i = left_token_index; i <= right_token_index; i++)
    {
        // std::cout << tokens[i] << std::endl;
        if (tokens[i].type == Token::TokenType::KEYWORD)
        {
            // 变量声明
            if (tokens[i].value == "int")
            {
                if (tokens[i + 1].type == Token::TokenType::IDENTIFIER)
                {
                    while (tokens[i].value != ";")
                    {
                        i++;
                        if (tokens[i + 1].value == "=") // 声明并赋值
                        {
                            int j = i + 2, bracket_count = 0; // 括号计数
                            while (bracket_count || (tokens[j].value != ";" && tokens[j].value != ","))
                            {
                                if (tokens[j].value == "(")
                                    bracket_count++;
                                if (tokens[j].value == ")")
                                    bracket_count--;
                                j++;
                            }
                            Expression(i + 2, j - 1);
                            ebp += 4;
                            identifier_map[tokens[i].value] = ebp;
                            out += "mov DWORD PTR [ebp-" + std::to_string(ebp) + "], eax  # int " + tokens[i].value + " = eax\n";
                            i = j;
                        }
                        else if (tokens[i + 1].value == "," || tokens[i + 1].value == ";") // 仅声明
                        {
                            ebp += 4;
                            identifier_map[tokens[i].value] = ebp;
                            out += "mov DWORD PTR [ebp-" + std::to_string(ebp) + "], 0  # int " + tokens[i].value + "\n";
                            i++;
                        }
                        else
                        {
                            Error("Expected = or , or ;");
                        }
                    }
                }
                else
                {
                    Error("Expected identifier after int");
                }
            }
            else if (tokens[i].value == "if")
            {
                int j = i + 1, bracket_count = 0, if_index = i;
                while (1)
                {
                    if (tokens[j].value == "(")
                        bracket_count++;
                    if (tokens[j].value == ")")
                    {
                        bracket_count--;
                        if (bracket_count == 0)
                            break;
                    }
                    j++;
                }
                Expression(i + 2, j - 1);
                out += "test eax, eax\n";
                out += "jz .if_else_" + std::to_string(if_index) + "  # if\n";
                i = j + 1, bracket_count = 0;
                while (1)
                {
                    if (tokens[j].value == "{")
                        bracket_count++;
                    if (tokens[j].value == "}")
                    {
                        bracket_count--;
                        if (bracket_count == 0)
                            break;
                    }
                    j++;
                }
                BlockBody(i, j);
                i = j;
                if (tokens[i + 1].value == "else")
                {
                    out += "jmp .if_end_" + std::to_string(if_index) + "  # if end\n";
                    out += ".if_else_" + std::to_string(if_index) + ":\n";
                    i += 2, j += 2, bracket_count = 0;
                    while (1)
                    {
                        if (tokens[j].value == "{")
                            bracket_count++;
                        if (tokens[j].value == "}")
                        {
                            bracket_count--;
                            if (bracket_count == 0)
                                break;
                        }
                        j++;
                    }
                    BlockBody(i, j - 1);
                    out += ".if_end_" + std::to_string(if_index) + ":\n";
                    i = j;
                }
                else
                {
                    out += ".if_else_" + std::to_string(if_index) + ":\n";
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
            if (tokens[i + 1].value == "(")
            {
                int j = i + 1, bracket_count = 0;
                while (1)
                {
                    if (tokens[j].value == "(")
                        bracket_count++;
                    if (tokens[j].value == ")")
                    {
                        bracket_count--;
                        if (bracket_count == 0)
                            break;
                    }
                    j++;
                }
                CallFunction(i, j);
                i = j + 1;
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
    return 0;
}

Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;

    for (std::vector<Token>::size_type i = 0; i < tokens.size(); i++)
    {
        // std::cout << tokens[i] << std::endl;
        if (tokens[i].type == Token::TokenType::KEYWORD && (tokens[i].value == "int" || tokens[i].value == "void"))
        {
            if (tokens[i + 1].type == Token::TokenType::IDENTIFIER)
            {
                ebp = 0;
                identifier_map.clear();

                // 识别参数
                int j = i + 3, args_count = 0;
                while (tokens[j].value != ")")
                {
                    if (tokens[j].value == "int")
                    {
                        if (tokens[j + 1].type == Token::TokenType::IDENTIFIER)
                        {
                            identifier_map[tokens[j + 1].value] = -(8 + args_count * 4);
                            args_count++;
                        }
                        else
                        {
                            Error("Expected identifier after int");
                        }
                        j += 2;
                    }
                    else if (tokens[j].value == ",")
                    {
                        j++;
                    }
                    else
                    {
                        Error("Expected int or ) or ,");
                    }
                }

                int body_start, body_end, block_count = 0;
                for (std::vector<Token>::size_type j = i + 2; j < tokens.size(); j++)
                {
                    if (tokens[j].type == Token::TokenType::PUNCTUATOR)
                    {
                        if (tokens[j].value == "{")
                        {
                            block_count++;
                            if (block_count == 1)
                            {
                                body_start = j + 1;
                            }
                        }
                        else if (tokens[j].value == "}")
                        {
                            block_count--;
                            if (block_count == 0)
                            {
                                body_end = j - 1;
                                break;
                            }
                        }
                    }
                }

                out += tokens[i + 1].value + ":\n";
                out += "push ebp\n";
                out += "mov ebp, esp\n";
                out += "sub esp, 0x100\n";
                BlockBody(body_start, body_end);
                out += "leave\n";
                out += "ret\n";
                i = body_end + 1;
            }
            else
            {
                Error("Expected identifier after int");
            }
        }
        else
        {
            Error("Expected int or void");
        }
    }

    // 插入汇编模板中
    out = asm_template.replace(asm_template.find("##REPLACEME##"), 13, out);
}
