#include "libparse2/parse.h"
#include "libparse2/value_type.h"

namespace libparse2 {
namespace gramma {

static std::unordered_map<std::string, OperateType> _operator_map = {
        {"+", kADD},           {"-", kSUB},     {"*", kMUL},
        {"/", kDIV},           {"%", kMOD},     {"in", kIN},
        {">", kGREATE},        {"<", kLOWER},   {">=", kGREATEOREQUAL},
        {"<=", kLOWEROREQUAL}, {"==", kEQUAL},  {"not", kNOT},
        {"!=", kNOTEQUAL},     {"and", kAND},   {"or", kOR},
        {"^", kPOW},           {"=", kASIGN},   {";", kBRANCH},
        {"?", kTERNARY},       {":", kTERNARY},
};

static std::unordered_map<std::string, OperateType> _inner_function_map = {
        {"SET", kSET}, {"ARRAY", kVEC},
};

bool is_inner_function(const std::string& token) {
    if (is_operator(token))
        return false;
    return _inner_function_map.find(token) != _inner_function_map.end();
}

OperateType get_operator(const std::string& token) {
    return _operator_map[token];
}

bool is_operator(const std::string& token) {
    return _operator_map.find(token) != _operator_map.end();
};

bool is_terminate(char c) {
    return c == '\0';
}

bool skip_char(char c) {
    if (c == ' ' || c == '\t')
        return true;
    return false;
}
bool is_parentheses(char c) {
    return c == '(' || c == ')';
}

bool is_number(char c, StrTokenType pre_type) {
    if ((pre_type == TYPE_ALGO_OPERATOR || pre_type == TYPE_CMP_OPERATOR || pre_type == TYPE_LEFT_PARENTHESES ||
         pre_type == TYPE_PRE || pre_type == TYPE_BRANCH || pre_type == TYPE_COMMA) and
        (c == '+' || c == '-' || isdigit(c) || c == '.')) {
        return true;
    }
    return false;
}

const char* get_last_pointer_from_number_str(const char* str) {
    // 参考 https://github.com/ochafik/LibCL/blob/master/src/main/resources/LibCL/strtof.c
    const char* start_input = str;
    if (*start_input == '-' || *start_input == '+')
        ++start_input;
    int dec_pt = -1;
    int mant_size = 0;
    for (mant_size = 0;; mant_size += 1) {
        char c = *start_input;
        if (!isdigit(c)) {
            if ((c != '.') || dec_pt >= 0)
                break;
            dec_pt = mant_size;
        }
        ++start_input;
    }
    if (dec_pt >= 0) {
        mant_size -= 1;
    }
    if (mant_size == 0) {
        return start_input;
    }
    if (*start_input == 'E' || *start_input == 'e') {
        ++start_input;
        if (*start_input == '-' || *start_input == '+') {
            ++start_input;
        }
        while (isdigit(*start_input))
            ++start_input;
    }
    return start_input;
}

StrTokenType get_token_type(char c, StrTokenType pre_type) {
    //这里简单实现同数字的字符可以互相结合，比如=====, +++++++  是一个运算符
    // 解析有顺序，不可以更换顺序
    if (isalpha(c) || c == '_' || (pre_type == TYPE_VARIBLE && isdigit(c))) {
        // 支持c类型变量
        return TYPE_VARIBLE;
    }
    if (pre_type != TYPE_ALGO_OPERATOR and (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^'))
        return TYPE_ALGO_OPERATOR;
    if (c == '>' || c == '<' || c == '=' || c == '!')
        return TYPE_CMP_OPERATOR;
    if (c == ':' || c == '?')
        return TYPE_OTERNARY;
    if (c == ',')
        return TYPE_COMMA;
    if (c == ';')
        return TYPE_BRANCH;
    return TYPE_OTHERS;
}

//是否左结合
bool op_left_assoc(const std::string& op) {
    return op == "+" || op == "-" || op == "/" || op == "*" || op == "%" || op == "^" || op == ">" || op == ">=" ||
           op == "<" || op == "<=" || op == "in" || op == "!=" || op == "==" || op == "and" || op == "or" ||
           op == "in" || op == ";" || op == "=";
};
//运算符优先级
int op_preced(const std::string& op) {
    if (op == "not")
        return 10;
    if (op == "^")
        return 9;
    if (op == "*" || op == "/" || op == "%")
        return 8;
    if (op == "+" || op == "-")
        return 7;
    if (op == "in")
        return 7;
    if (op == ">" || op == ">=" || op == "<" || op == "<=" || op == "!=")
        return 6;
    if (op == "==")
        return 5;
    if (op == "and")
        return 4;
    if (op == "or")
        return 2;
    if (op == "?")
        return 3;
    if (op == "=")
        return 1;
    if (op == ";")
        return 0;
    return 0;
};
//运算符操作数
size_t op_arg_count(const std::string& op) {
    if (op == "not")
        return 1;
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "^") {
        return 2;
    }
    if (op == ">" || op == ">=" || op == "<" || op == "<=" || op == "!=" || op == ":=") {
        return 2;
    }
    if (op == "==" || op == "in" || op == "and" || op == "or") {
        return 2;
    }
    if (op == ";" || op == "=") {
        return 2;
    }
    // 三目运算特殊设置的
    if (op == "?" || op == ":") {
        return 3;
    }
    return 0;
};

// 实现简单的二阶语法
std::string next_token(const char* input, const char** next_token, StrTokenType& cur_type,
                       const StrTokenType& pre_type) {
    const char* start_input = input;
    //跳过无效字符
    while (!is_terminate(*start_input) && skip_char(*start_input))
        ++start_input;
    if (is_terminate(*start_input))
        return std::string("");
    const char* start_token = start_input;

    //如果是字符串，需要提取正确的
    if (*start_input == '"' || *start_input == '\'') {
        const char t = *start_input;
        ++start_input;
        while (!is_terminate(*start_input)) {
            if ((*(start_input - 1) == '\\' && *start_input == t) || *start_input != t) {
                ++start_input;
            } else {
                ++start_input;
                break;
            }
        }
        *next_token = start_input;
        cur_type = TYPE_STRING;
        return std::string(start_token, start_input);
    } else if (is_parentheses(*start_input)) {
        if (*start_input == '(') {
            cur_type = TYPE_LEFT_PARENTHESES;
        } else {
            cur_type = TYPE_RIGHT_PARENTHESES;
        }
        ++start_input;
        *next_token = start_input;
        return std::string(start_token, start_input);
    } else if (is_number(*start_input, pre_type)) {
        cur_type = TYPE_NUMBER;
        *next_token = get_last_pointer_from_number_str(start_input);
        return std::string(start_token, *next_token);
    } else {
        StrTokenType type = get_token_type(*start_input, pre_type);
        if (*start_input)
            ++start_input;

        cur_type = type;

        while (*start_input && type == get_token_type(*start_input, type)) {
            ++start_input;
        }
        *next_token = start_input;
        std::string token(start_token, start_input);
        if (is_operator(token)) {
            cur_type = TYPE_ALGO_OPERATOR;
        } else if (is_const(token)) {
            cur_type = TYPE_CONST;
        }
        return token;
    }
};

bool is_const(const std::string& token) {
    if (token.empty())
        return false;
    if (token[0] == '"' || token[0] == '\'') {
        const auto c = token[token.size() - 1];
        if (token.size() == 1 || c != token[0]) {
            return false;
        }
        return true;
    }
    if (isdigit(token[0]))
        return true;
    if (token.size() >= 2 && isdigit(token[1]) && (token[0] == '+' || token[0] == '-'))
        return true;
    return false;
};
bool is_varible(const std::string& token) {
    if (is_operator(token))
        return false;
    if (token.empty())
        return false;
    if (token[0] == '_' || islower(token[0]))
        return true;
    return false;
};

bool is_function(const std::string& token) {
    if (is_operator(token))
        return false;
    if (token.empty())
        return false;
    if (isupper(token[0]))
        return true;
    return false;
};

ValueType get_value_type(const std::string& token) {
    if (token[0] == '"' || token[0] == '\'')
        return vString;
    if (token.find(".") != std::string::npos or token.find("e") != std::string::npos)
        return vDouble;
    return vLong;
}

Value* paser_const_value(const std::string& token) {
    // 调用parse_value 必须用is_const 检查
    ValueType vt = get_value_type(token);
    switch (vt) {
        case vString: {
            const char* start = token.c_str() + 1;
            const char* end = token.c_str() + token.size() - 1;
            return new StringValue(std::string(start, end));
        } break;
        case vDouble: {
            double v = strtod(token.c_str(), NULL);
            return new DoubleValue(v);
        } break;
        case vLong:
            return new LongValue(strtol(token.c_str(), NULL, 10));
            break;
        default:
            return nullptr;
    };
    return nullptr;
};

std::string to_string(OperateType op) {
    switch (op) {
        case kADD:
            return "kADD";
        case kSUB:
            return "kSUB";
        case kMUL:
            return "kMUL";
        case kDIV:
            return "kDIV";
        case kIN:
            return "kIN";
        case kGREATE:
            return "kGREATE";
        case kGREATEOREQUAL:
            return "kGREATEOREQUAL";
        case kLOWER:
            return "kLOWER";
        case kLOWEROREQUAL:
            return "kLOWEROREQUAL";
        case kEQUAL:
            return "kEQUAL";
        case kNOTEQUAL:
            return "kNOTEQUAL";
        case kNOT:
            return "kNOT";
        case kAND:
            return "kAND";
        case kOR:
            return "kOR";
        case kLEFTPARENTHESIS:
            return "kLEFTPARENTHESIS";
        case kRIGHTPARENTHESIS:
            return "kRIGHTPARENTHESIS";
        case kMOD:
            return "kMOD";
        case kFUNC1:
            return "kFUNC1";
        case kFUNC2:
            return "kFUNC2";
        case kFUNC3:
            return "kFUNC3";
        case kVEC:
            return "kVEC";
        case kSET:
            return "kSET";
        case kPOW:
            return "kPOW";
        case kASIGN:
            return "kASIGN";
        case kBRANCH:
            return "kBRANCH";
        case kTERNARY:
            return "kTERNARY";
        default:
            return "none";
    };
}

}  // namespace gramma
}  // namespace libparse2
