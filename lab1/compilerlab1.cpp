/*
 * @Author: flwfdd
 * @Date: 2024-03-19 15:47:58
 * @LastEditTime: 2024-03-19 20:17:49
 * @Description:
 * _(:з」∠)_
 */
#include <cstdio>
#include <regex>
#include <string>
#include <iostream>
#include <map>
#include <stack>

class Token
{
public:
    // 词法单元类型
    enum class TokenType
    {
        KEYWORD,
        IDENTIFIER,
        CONSTANT,
        OPERATOR,
        SEPARATOR
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
        case TokenType::SEPARATOR:
            os << "SEPARATOR";
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
    Lexer(std::string src)
    {
        // 正则表达式
        std::string s_reg_keyword = "int|return";
        std::regex reg_keyword(s_reg_keyword);
        std::string s_reg_identifier = "[a-zA-Z_][a-zA-Z0-9_]*";
        std::regex reg_identifier(s_reg_identifier);
        std::string s_reg_constant = "[0-9]+";
        std::regex reg_constant(s_reg_constant);
        std::string s_reg_operator = "[+\\-*/=()]";
        std::regex reg_operator(s_reg_operator);
        std::string s_reg_separator = "[;]";
        std::regex reg_separator(s_reg_separator);

        // 自动迭代匹配
        std::regex tokenRegex(s_reg_keyword + "|" + s_reg_identifier + "|" + s_reg_constant + "|" + s_reg_operator + "|" + s_reg_separator);
        std::sregex_iterator it(src.begin(), src.end(), tokenRegex);
        std::sregex_iterator end;
        while (it != end)
        {
            std::smatch match = *it++;
            std::string token = match.str();
            if (std::regex_match(token, reg_keyword))
            {
                tokens.push_back(Token(Token::TokenType::KEYWORD, token));
            }
            else if (std::regex_match(token, reg_identifier))
            {
                tokens.push_back(Token(Token::TokenType::IDENTIFIER, token));
            }
            else if (std::regex_match(token, reg_constant))
            {
                tokens.push_back(Token(Token::TokenType::CONSTANT, token));
            }
            else if (std::regex_match(token, reg_operator))
            {
                tokens.push_back(Token(Token::TokenType::OPERATOR, token));
            }
            else if (std::regex_match(token, reg_separator))
            {
                tokens.push_back(Token(Token::TokenType::SEPARATOR, token));
            }
        }
    }
};

class Parser
{
public:
    std::vector<Token> tokens;
    std::string out;
    int token_index = 0, ebp = 0;
    std::map<std::string, int> identifier_map;

    // 错误处理
    void Error(std::string msg)
    {
        std::cerr << "Error: " << msg << std::endl;
        exit(1);
    }

    // 设置标识符的ebp偏移
    int set_identifier_ebp(std::string identifier)
    {
        if (identifier_map.find(identifier) != identifier_map.end())
        {
            Error("Identifier already exists");
        }
        else
        {
            ebp += 4;
            identifier_map[identifier] = ebp;
            return ebp;
        }
    }

    // 获取标识符的地址
    std::string get_identifier_addr(std::string identifier)
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
    void push(Token token)
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
    void pop(Token token)
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
    void Expression(int left_token_index, int right_token_index)
    {
        // 栈中存放的是token的索引 -1为内存栈顶
        std::stack<int> operator_stack;
        std::stack<int> operand_stack;

        // 一次求值 取出栈顶两个操作数和一个操作符 放回栈顶一个操作数
        auto eval_once = [&]() {
            // 右操作数
            int right = operand_stack.top();
            operand_stack.pop();
            if(right == -1)
                out += "pop ebx  # right operand from stack\n";
            else if(tokens[right].type == Token::TokenType::IDENTIFIER)
                out += "mov ebx, " + get_identifier_addr(tokens[right].value) + "  # right operand " + tokens[right].value + "\n";
            else
                out += "mov ebx, " + tokens[right].value + "  # right operand " + tokens[right].value + "\n";

            // 左操作数
            int left = operand_stack.top();
            operand_stack.pop();
            if(left == -1)
                out += "pop eax  # left operand from stack\n";
            else if(tokens[left].type == Token::TokenType::IDENTIFIER)
                out += "mov eax, " + get_identifier_addr(tokens[left].value) + "  # left operand " + tokens[left].value + "\n";
            else
                out += "mov eax, " + tokens[left].value + "  # left operand " + tokens[left].value + "\n";
            
            // 操作符
            int op = operator_stack.top();
            operator_stack.pop();
            if(tokens[op].value == "+")
                out += "add eax, ebx\n";
            else if(tokens[op].value == "-")
                out += "sub eax, ebx\n";
            else if(tokens[op].value == "*")
                out += "imul eax, ebx\n";
            else if(tokens[op].value == "/")
            {
                out += "cdq\n";
                out += "idiv ebx\n";
            }

            operand_stack.push(-1);
            out += "push eax  # push result\n";
        };

        out += "\n# Expression\n";
        for (int i = left_token_index; i <= right_token_index; i++)
        {
            if (tokens[i].type == Token::TokenType::OPERATOR)
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
                else if (tokens[i].value == "+" || tokens[i].value == "-")
                {
                    while (!operator_stack.empty() && tokens[operator_stack.top()].value != "(")
                    {
                        eval_once();
                    }
                    operator_stack.push(i);
                }
                else if (tokens[i].value == "*" || tokens[i].value == "/")
                {
                    while (!operator_stack.empty() && (tokens[operator_stack.top()].value == "*" || tokens[operator_stack.top()].value == "/"))
                    {
                        eval_once();
                    }
                    operator_stack.push(i);
                }
                else
                {
                    Error("Unknown operator: " + tokens[i].value);
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
        if(operand_stack.size() == 1)
        {
            if(operand_stack.top() == -1)
                out += "pop eax  # expression result\n";
            else if(tokens[operand_stack.top()].type == Token::TokenType::IDENTIFIER)
                out += "mov eax, " + get_identifier_addr(tokens[operand_stack.top()].value) + "  # expression result " + tokens[operand_stack.top()].value + "\n";
            else
                out += "mov eax, " + tokens[operand_stack.top()].value + "  # expression result " + tokens[operand_stack.top()].value + "\n";
        }
        else
        {
            Error("Expression result error");
        }
    }

    Parser(std::vector<Token> tokens)
    {
        this->tokens = tokens;
        for (int i = 0; i < tokens.size(); i++)
        {
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
                if(tokens[i + 1].value == "=")
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
            else if (tokens[i].type == Token::TokenType::SEPARATOR)
            {
                if (tokens[i].value == ";")
                {
                    out += "\n";
                }
                else
                {
                    Error("Unexpected separator");
                }
            }
        }
    }
};

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
    Parser parser(lexer.tokens);
    std::cout<<parser.out;

    return 0;
}