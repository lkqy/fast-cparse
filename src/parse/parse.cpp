#include "parse/parse.h"
#include "parse/value_type.h"

namespace parse {

ShuntingYard::ShuntingYard(const std::string &e)
    : expression(e), expression_root(nullptr) {}

bool ShuntingYard::compile() {
  status_log.clear();
  std::queue<std::pair<std::string, int>> queue;
  if (build_visit_queue(queue)) {
    expression_root = build_expression_tree(queue);
    if (expression_root) {
      status_log += "init expression success:";
      return true;
    } else {
      status_log += "init node tree failed";
      return false;
    }
  } else {
    status_log += "init expression failed";
    return false;
  }
}

int ShuntingYard::get_token_type(char c, int pre_type) {
  //暂时不考虑+2， -1这种case
  //这里简单实现同数字的字符可以互相结合，比如=====, +++++++  是一个运算符
  // 解析有顺序，不可以更换顺序
  if (isalpha(c) || c == '_' || (pre_type == 4 && isdigit(c))) {
    return 4;
  }
  if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
    return 1;
  if (c == '>' || c == '<' || c == '=' || c == '!')
    return 2;
  if (isdigit(c) || c == '.')
    return 3;
  if (c == ',')
    return 5;
  return 6;
}

//是否左结合
bool ShuntingYard::op_left_assoc(const std::string &op) {
  return op == "+" || op == "-" || op == "/" || op == "*" || op == "%" ||
         op == ">" || op == ">=" || op == "<" || op == "<=" || op == "in" ||
         op == "==" || op == "and" || op == "or" || op == "in";
};
//运算符优先级
int ShuntingYard::op_preced(const std::string op) {
  if (op == "not")
    return 9;
  if (op == "*" || op == "/" || op == "%")
    return 8;
  if (op == "+" || op == "-")
    return 7;
  if (op == "in")
    return 7;
  if (op == ">" || op == ">=" || op == "<" || op == "<=")
    return 6;
  if (op == "==")
    return 5;
  if (op == "and")
    return 4;
  if (op == "or")
    return 3;
  return 0;
};
//运算符操作数
size_t ShuntingYard::op_arg_count(const std::string &op) {
  if (op == "not")
    return 1;
  if (op == "+" || op == "-" || "*" || op == "/" || op == "%")
    return 2;
  if (op == ">" || op == ">=" || op == "<" || op == "<=")
    return 2;
  if (op == "==" || op == "in" || op == "and" || op == "or")
    return 2;
  return 0;
};

std::string ShuntingYard::next_token(const char *input,
                                     const char **next_token) {
  const char *start_input = input;
  //跳过无效字符
  while (!is_terminate(*start_input) && skip_char(*start_input))
    ++start_input;
  if (is_terminate(*start_input))
    return std::string("");
  const char *start_token = start_input;

  //如果是字符串，需要提取正确的
  if (*start_input == '"') {
    ++start_input;
    while (!is_terminate(*start_input)) {
      if ((*(start_input - 1) == '\\' && *start_input == '\"') ||
          *start_input != '\"') {
        ++start_input;
      } else {
        ++start_input;
        break;
      }
    };
    *next_token = start_input;
    return std::string(start_token, start_input);
  } else if (is_parentheses(*start_input)) {
    ++start_input;
    *next_token = start_input;
    return std::string(start_token, start_input);
  } else {
    int type = get_token_type(*start_input, -1000);
    while (*start_input && type == get_token_type(*start_input, type))
      ++start_input;
    *next_token = start_input;
    return std::string(start_token, start_input);
  }
};
bool ShuntingYard::is_const(const std::string &token) {
  if (token.empty())
    return false;
  if (token[0] == '"') {
    if (token[token.size() - 1] != '"' || token.size() == 1)
      return false;
    return true;
  }
  if (isdigit(token[0]))
    return true;
  if (token.size() >= 2 && isdigit(token[1]) &&
      (token[0] == '+' || token[0] == '-'))
    return true;
  return false;
};
bool ShuntingYard::is_varible(const std::string &token) {
  if (is_operator(token))
    return false;
  if (token.empty())
    return false;
  if (token[0] == '_' || islower(token[0]))
    return true;
  return false;
};

bool ShuntingYard::is_inner_function(const std::string &token) {
  if (is_operator(token))
    return false;
  return inner_function_map.find(token) != inner_function_map.end();
};

bool ShuntingYard::is_function(const std::string &token) {
  if (is_operator(token))
    return false;
  if (token.empty())
    return false;
  if (isupper(token[0]))
    return true;
  return false;
};

bool ShuntingYard::is_operator(const std::string &token) {
  return operator_map.find(token) != operator_map.end();
};

ValueType get_value_type(const std::string &token) {
  if (token[0] == '"')
    return vString;
  if (token.find(".") != std::string::npos)
    return vDouble;
  return vLong;
}

Value *ShuntingYard::paser_varible(const std::string &token) {
  size_t size = token.size();
  if (token[size - 1] == 'i')
    return new IntValue(0);
  if (token[size - 1] == 'l')
    return new LongValue(0);
  if (token[size - 1] == 'f')
    return new FloatValue(0.0);
  if (token[size - 1] == 'd')
    return new DoubleValue(0.0);
  if (token[size - 1] == 's')
    return new StringValue(get_str_const(std::string("")).c_str());
  return new Value();
}

Value *ShuntingYard::paser_value(const std::string &token) {
  // 调用parse_value 必须用is_const 检查
  ValueType vt = get_value_type(token);
  switch (vt) {
  case vString: {
    const char *start = token.c_str() + 1;
    const char *end = token.c_str() + token.size() - 1;
    return new StringValue(get_str_const(std::string(start, end)).c_str());
  } break;
  case vDouble: {
    double v = strtod(token.c_str(), NULL);
    return new DoubleValue(v);
  } break;
  case vLong:
    return new LongValue(strtol(token.c_str(), NULL, 10));
    break;
  default:
    status_log = "error value type";
  };
  return new Value();
};

bool ShuntingYard::build_visit_queue(
    std::queue<std::pair<std::string, int>> &queue) {
  const char *input = expression.c_str();
  const char *end = input + strlen(input);
  const char *start = input;

  //查函数对应的参数个数，比如a(1,2,3) => 3
  auto get_function_param_number = [](const char *p) {
    int left_parenthesis = -1;
    int comma_number = 0;
    bool has_letter = false;
    while (*p) {
      if (*p == '(') {
        ++left_parenthesis;
      } else if (*p == ',') {
        if (left_parenthesis == 0)
          ++comma_number;
      } else if (*p == ')') {
        --left_parenthesis;
        if (left_parenthesis == -1)
          break;
      } else if (*p == '_' || isalpha(*p) || isdigit(*p)) {
        has_letter = true;
      }
      ++p;
    }
    if (comma_number > 0)
      return comma_number + 1;
    if (has_letter)
      return 1;
    return 0;
  };

  std::stack<std::pair<std::string, int>> stack;
  while (start < end) {
    const char *p = nullptr;
    std::string token = next_token(start, &p);
    start = p;
    if (token.empty()) {
      break;
    } else if (is_operator(token)) {
      while (!stack.empty() && is_operator(stack.top().first)) {
        if ((op_left_assoc(token) &&
             op_preced(token) <= op_preced(stack.top().first)) ||
            (!op_left_assoc(token) &&
             op_preced(token) < op_preced(stack.top().first))) {
          queue.push(stack.top());
          stack.pop();
        } else {
          break;
        }
      }
      stack.push(std::make_pair(token, tOperator));
    } else if (is_const(token)) {
      queue.push(std::make_pair(token, tConst));
    } else if (is_inner_function(token)) {
      //数一下逗号个数，来对应参数数个
      int param_number = get_function_param_number(p);
      stack.push(std::make_pair(token, tInnerFunction + param_number));
    } else if (is_function(token)) {
      //数一下逗号个数，来对应参数数个
      int param_number = get_function_param_number(p);
      stack.push(std::make_pair(token, tFunctionBeginIndex + param_number));
    } else if (is_varible(token)) {
      queue.push(std::make_pair(token, tVarible));
    } else if (token == ",") {
      bool find = false;
      while (!stack.empty()) {
        if (stack.top().first == "(") {
          find = true;
          break;
        } else {
          queue.push(stack.top());
          stack.pop();
        }
      }
      if (!find) {
        status_log += "Error: function parentheses mismatched";
        return false;
      }
    } else if (token == "(") {
      stack.push(std::make_pair(token, tLeftParenthesis));
    } else if (token == ")") {
      bool find = false;
      while (!stack.empty()) {
        if (stack.top().first == "(") {
          find = true;
          stack.pop();
          break;
        } else {
          queue.push(stack.top());
          stack.pop();
        }
      }
      if (!find) {
        status_log += "Error: parentheses mismatched";
        return false;
      }
      if (!stack.empty()) {
        int token_type = stack.top().second;
        if (token_type >= tInnerFunction || token_type >= tFunctionBeginIndex) {
          queue.push(stack.top());
          stack.pop();
        }
      }
    } else {
      status_log += "error expression: unkonw token:" + token;
      return false;
    }
  }
  while (!stack.empty()) {
    if (stack.top().first == "(" || stack.top().first == ")") {
      status_log += "Error: parentheses mismatched";
      return false;
    }
    queue.push(stack.top());
    stack.pop();
  }
  return true;
}

Node *ShuntingYard::build_expression_tree(
    std::queue<std::pair<std::string, int>> &queue) {
  std::stack<Node *> stack;
  int temporal_varible_index = 0;

  auto process_const = [&](const auto &token) {
    Node *node = new Node();
    node->op_count = -1;
    node->result_varible = token;
    node->value = this->paser_value(token);
    //特化下bool，防止输入常量bool时，缓存失效
    node->bool_value = node->value->get_bool();
    stack.push(node);
    return true;
  };
  auto process_varible = [&](const auto &token) {
    //如果变量是同一个，就索引
    Node *node = new Node();
    node->op_count = 0;
    // node->value = paser_varible(token);
    node->result_varible = token;
    stack.push(node);
    return true;
  };
  auto process_operator = [&](const auto &token) {
    size_t count = op_arg_count(token);
    if (stack.size() < count)
      return false;
    Node *node = new Node();
    node->op_count = count;
    node->operator_type = operator_map[token];
    //不存结果
    // node->value = std::make_shared<BoolValue>(false);
    std::stack<Node *> temp_stack;
    for (size_t i = 0; i < count; ++i) {
      temp_stack.push(stack.top());
      stack.pop();
    }
    int _count = 0;
    while (!temp_stack.empty()) {
      ++_count;
      if (_count == 1) {
        node->first = temp_stack.top();
      } else if (_count == 2) {
        node->second = temp_stack.top();
      } else if (_count == 3) {
        node->third = temp_stack.top();
      }
      // node->varibles.push_back(temp_stack.top());
      temp_stack.pop();
    }
    stack.push(node);
    return true;
  };

  //内置函数都是常量表达式
  auto process_inner_function = [&](auto &token, int param_number) {
    long stack_size = stack.size();
    if (stack_size < param_number)
      return false;

    std::stack<Node *> temp_stack;
    for (int i = 0; i < param_number; ++i) {
      temp_stack.push(stack.top());
      stack.pop();
    }
    std::vector<Value *> values;
    while (!temp_stack.empty()) {
      values.push_back(temp_stack.top()->value);
      temp_stack.pop();
    }
    if (values.size() == 0)
      return false;
    Node *node = new Node();
    node->op_count = -1;
    if (token == "VEC") {
      //
      node->value = to_vec_value(values);
      if (node->value->type() == vNull) {
        status_log = "all value not the same type in VEC";
        return false;
      }
    } else if (token == "SET") {
      node->value = to_set_value(values);
      if (node->value->type() == vNull) {
        status_log = "all value not the same type in SET";
        return false;
      }
    } else {
      status_log = "not implemented";
      return false;
    }
    stack.push(node);
    return true;
  };

  auto process_function = [&](auto &token, int param_number) {
    long stack_size = stack.size();
    if (stack_size < param_number)
      return false;
    Node *node = new Node();
    node->op_count = param_number;
    node->result_varible = token;
    node->operator_type = function_map[param_number];
    std::stack<Node *> temp_stack;
    for (int i = 0; i < param_number; ++i) {
      temp_stack.push(stack.top());
      stack.pop();
    }
    int _count = 0;
    while (!temp_stack.empty()) {
      ++_count;
      if (_count == 1) {
        node->first = temp_stack.top();
      } else if (_count == 2) {
        node->second = temp_stack.top();
      } else if (_count == 3) {
        node->third = temp_stack.top();
      } else {
        status_log = "error function params number";
      }
      temp_stack.pop();
    }
    stack.push(node);
    return true;
  };

  while (!queue.empty()) {
    ++temporal_varible_index;
    const std::pair<std::string, int> &token = queue.front();
    if (token.second == tConst) {
      if (!process_const(token.first))
        return nullptr;
    } else if (token.second == tOperator) {
      if (!process_operator(token.first))
        return nullptr;
    } else if (token.second == tVarible) {
      if (!process_varible(token.first))
        return nullptr;
    } else if (token.second >= tFunctionBeginIndex) { //按函数处理
      if (!process_function(token.first, token.second - tFunctionBeginIndex))
        return nullptr;
    } else if (token.second >= tInnerFunction) {
      //内置函数，转换常量
      if (!process_inner_function(token.first, token.second - tInnerFunction))
        return nullptr;
    } else {
      status_log =
          "eror_type:" + token.first + " " + std::to_string(token.second);
    }
    queue.pop();
  }
  if (stack.size() == 1) {
    return stack.top();
  }
  return nullptr;
}

Value *ShuntingYard::op_varible(const std::string &result_varible,
                                OperateType operator_type, Value *value,
                                std::string &log) {
  switch (operator_type) {
  case kNOT: {
    return new BoolValue(value->_not());
  } break;
  case kFUNC1:
    return func1_map[result_varible](value);
    break;
  default:
    log += "not support operator";
    return new Value();
  };
};
Value *ShuntingYard::op_varible_varible(const std::string &result_varible,
                                        OperateType operator_type, Value *first,
                                        Value *second, std::string &log) {
  switch (operator_type) {
  case kAND: {
    return new BoolValue(first->_and(second));
  } break;
  case kOR: {
    return new BoolValue(first->_or(second));
  } break;
  case kADD: {
    if (first->type() >= second->type()) {
      Value *result = first->clone();
      first->add(second, result);
      return result;
    } else {
      Value *result = second->clone();
      second->add(first, result);
      return result;
    }
  } break;
  case kSUB: {
    if (first->type() >= second->type()) {
      Value *result = first->clone();
      first->sub(second, result);
      return result;
    } else {
      Value *result = second->clone();
      second->sub(first, result);
      result->negative();
      return result;
    }
  } break;
  case kMUL: {
    if (first->type() >= second->type()) {
      Value *result = first->clone();
      first->mul(second, result);
      return result;
    } else {
      Value *result = second->clone();
      second->mul(first, result);
      return result;
    }
  } break;
  case kDIV: {
    if (first->type() >= second->type()) {
      Value *result = first->clone();
      first->div(second, result);
      return result;
    } else {
      Value *result = new DoubleValue(0);
      first->s_div(second, result);
      return result;
    }
  } break;
  case kMOD: {
    Value *result = first->clone();
    first->mod(second, result);
    return result;
  } break;
  case kIN: {
    return new BoolValue(second->in(first));
  } break;
  case kGREATE:
    return new BoolValue(first->gt(second));
    break;
  case kGREATEOREQUAL:
    return new BoolValue(first->gt_or_equal(second));
    break;
  case kLOWER:
    return new BoolValue(first->lt(second));
    break;
  case kLOWEROREQUAL:
    return new BoolValue(first->lt_or_equal(second));
    break;
  case kEQUAL:
    return new BoolValue(first->equal(second));
    break;
  case kNOTEQUAL:
    return new BoolValue(!first->equal(second));
    break;
  case kFUNC2:
    return func2_map[result_varible](first, second);
    break;
  default:
    log += "not support operator";
  };
  return new Value();
}

Value *ShuntingYard::op_varible_varible_varible(
    const std::string &result_varible, OperateType operator_type, Value *first,
    Value *second, Value *third, std::string &log) {
  switch (operator_type) {
  case kFUNC3:
    return func3_map[result_varible](first, second, third);
    break;
  default:
    log += "not support operator";
  };
  return new Value();
};

Value *ShuntingYard::find_varible(
    std::vector<std::pair<const char *, Value *>> &varibles, std::string &key,
    std::string &log) {
  const char *_key = key.c_str();
  for (auto &it : varibles) {
    if (strcmp(it.first, _key) == 0)
      return it.second;
  }
  log += "can find varible:" + key;
  return nullptr;
}
Value *ShuntingYard::eval_expression(
    Node *node, std::vector<std::pair<const char *, Value *>> &varibles,
    std::string &log) {
  switch (node->op_count) {
  case 2: {
    Value *first = eval_expression(node->first, varibles, log);
    if (!first)
      return nullptr;
    Value *second = eval_expression(node->second, varibles, log);
    if (!second)
      return nullptr;
    Value *result = op_varible_varible(node->result_varible,
                                       node->operator_type, first, second, log);
    if (node->second->op_count > 0)
      delete second;
    if (node->first->op_count > 0)
      delete first;
    return result;
  } break;
  case 1: {
    Value *first = eval_expression(node->first, varibles, log);
    if (!first)
      return nullptr;
    Value *result =
        op_varible(node->result_varible, node->operator_type, first, log);
    if (node->first->op_count > 0)
      delete first;
    return result;
  } break;
  case 0: {
    //赋值的时候已经初始化完
    Value *v = find_varible(varibles, node->result_varible, log);
    if (v != nullptr)
      return v;
    return nullptr;
  } break;
  case -1: //常量在初始化就做好
    return node->value;
    break;
  case 3: {
    Value *first = eval_expression(node->first, varibles, log);
    if (!first)
      return nullptr;
    Value *second = eval_expression(node->second, varibles, log);
    if (!second)
      return nullptr;
    Value *third = eval_expression(node->third, varibles, log);
    if (!third)
      return nullptr;
    Value *result = op_varible_varible_varible(
        node->result_varible, node->operator_type, first, second, third, log);
    if (node->third->op_count > 0)
      delete third;
    if (node->second->op_count > 0)
      delete second;
    if (node->first->op_count > 0)
      delete first;
    return result;
  }
  default:
    log += "error count";
  };
  return nullptr;
}

void free_map(std::vector<std::pair<const char *, Value *>> &map) {
  for (auto &it : map) {
    delete it.second;
  }
}

void ShuntingYard::free_result(Value *v) {
  if (expression_root->op_count != 0 && expression_root->op_count != -1 && v) {
    delete v;
  }
}

bool ShuntingYard::eval_bool(
    std::vector<std::pair<const char *, Value *>> &varibles, std::string &log) {
  Value *v = eval_expression(expression_root, varibles, log);

  free_map(varibles);
  if (v == nullptr) {
    log += " return pointer is nullptr";
    return false;
  }
  auto _r = (BoolValue *)v;
  bool ret = _r->val;
  free_result(v);
  return ret;
}

Value *
ShuntingYard::_eval(std::vector<std::pair<const char *, Value *>> &varibles,
                    std::string &log) {
  Value *v = eval_expression(expression_root, varibles, log);
  if (v == nullptr) {
    log += " return pointer is nullptr";
  }
  return v;
}
Value *
ShuntingYard::eval(std::vector<std::pair<const char *, Value *>> &varibles,
                   std::string &log) {
  Value *v = _eval(varibles, log);
  free_map(varibles);
  return v;
}

int ShuntingYard::eval_int(
    std::vector<std::pair<const char *, Value *>> &varibles, std::string &log) {
  Value *v = _eval(varibles, log);
  int ret = 0;
  if (v != nullptr) {
    if (v->type() == vInt) {
      ret = ((IntValue *)v)->val;
    } else if (v->type() == vDouble) {
      ret = ((DoubleValue *)v)->val;
    } else if (v->type() == vLong) {
      ret = ((LongValue *)v)->val;
    } else if (v->type() == vFloat) {
      ret = ((FloatValue *)v)->val;
    }
    free_result(v);
  }
  free_map(varibles);
  return ret;
}

long ShuntingYard::eval_long(
    std::vector<std::pair<const char *, Value *>> &varibles, std::string &log) {
  Value *v = _eval(varibles, log);
  long ret = 0;
  if (v != nullptr) {
    if (v->type() == vInt) {
      ret = ((IntValue *)v)->val;
    } else if (v->type() == vDouble) {
      ret = ((DoubleValue *)v)->val;
    } else if (v->type() == vLong) {
      ret = ((LongValue *)v)->val;
    } else if (v->type() == vFloat) {
      ret = ((FloatValue *)v)->val;
    }
    free_result(v);
  }
  free_map(varibles);
  return ret;
}

float ShuntingYard::eval_float(
    std::vector<std::pair<const char *, Value *>> &varibles, std::string &log) {
  Value *v = _eval(varibles, log);
  float ret = 0;
  if (v != nullptr) {
    if (v->type() == vInt) {
      ret = ((IntValue *)v)->val;
    } else if (v->type() == vDouble) {
      ret = ((DoubleValue *)v)->val;
    } else if (v->type() == vLong) {
      ret = ((LongValue *)v)->val;
    } else if (v->type() == vFloat) {
      ret = ((FloatValue *)v)->val;
    }
    free_result(v);
  }
  free_map(varibles);
  return ret;
}

double ShuntingYard::eval_double(
    std::vector<std::pair<const char *, Value *>> &varibles, std::string &log) {
  Value *v = _eval(varibles, log);
  double ret = 0;
  if (v != nullptr) {
    if (v->type() == vInt) {
      ret = ((IntValue *)v)->val;
    } else if (v->type() == vDouble) {
      ret = ((DoubleValue *)v)->val;
    } else if (v->type() == vLong) {
      ret = ((LongValue *)v)->val;
    } else if (v->type() == vFloat) {
      ret = ((FloatValue *)v)->val;
    }
    free_result(v);
  }
  free_map(varibles);
  return ret;
}

} // namespace parse
