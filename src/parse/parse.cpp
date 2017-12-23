#include "parse/parse.h"
#include "parse/value_type.h"

namespace parse {
ShuntingYard::ShuntingYard(const std::string &e) : expression(e) {
  std::queue<std::string> queue;
  if (build_visit_queue(queue)) {
    expression_root = build_expression_tree(queue);
    if (expression_root) {
      std::cout << "init expression success:" << expression_root << "\n";
      for (const auto &it : varible_map) {
        std::cout << it.first << "|\n";
      }
    } else {
      std::cout << "init node tree failed:" << expression_root << "\n";
    }
  } else {
    std::cout << "init expression failed\n";
  }
}
int ShuntingYard::get_token_type(char c) {
  //暂时不考虑+2， -1这种case
  //这里简单实现同数字的字符可以互相结合，比如=====, +++++++  是一个运算符
  if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
    return 1;
  if (c == '>' || c == '<' || c == '=')
    return 2;
  if (isdigit(c) || c == '.')
    return 3;
  if (isalpha(c) || c == '_')
    return 4;
  return 5;
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
    int type = get_token_type(*start_input);
    while (*start_input && type == get_token_type(*start_input))
      ++start_input;
    *next_token = start_input;
    return std::string(start_token, start_input);
  }
};
bool ShuntingYard::is_const(const std::string &token) {
  if (token.empty())
    return false;
  if (token[0] == '"')
    return true;
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
  if (token[0] == '_' || isalpha(token[0]))
    return true;
  return false;
};

bool ShuntingYard::is_operator(const std::string &token) {
  return operator_map.find(token) != operator_map.end();
};

ValuePtr ShuntingYard::paser_value(const std::string &token) {
  // 调用parse_value 必须用is_const 检查
  if (token[0] == '"') {
    const char *start = token.c_str() + 1;
    const char *end = token.c_str() + token.size() - 1;
    return std::make_shared<StringValue>(std::string(start, end));
  } else if (token.find(".") != std::string::npos) {
    return std::make_shared<DoubleValue>(strtod(token.c_str(), NULL));
  }
  return std::make_shared<LongValue>(strtol(token.c_str(), NULL, 10));
};
bool ShuntingYard::build_visit_queue(std::queue<std::string> &queue) {
  const char *input = expression.c_str();
  const char *end = input + strlen(input);
  const char *start = input;
  std::stack<std::string> stack;
  while (start < end) {
    const char *p = nullptr;
    std::string token = next_token(start, &p);
    start = p;
    if (token.empty()) {
      break;
    } else if (is_const(token)) {
      queue.push(token);
    } else if (is_varible(token)) {
      queue.push(token);
    } else if (is_operator(token)) {
      while (!stack.empty() && is_operator(stack.top())) {
        if ((op_left_assoc(token) &&
             op_preced(token) <= op_preced(stack.top())) ||
            (!op_left_assoc(token) &&
             op_preced(token) < op_preced(stack.top()))) {
          queue.push(stack.top());
          stack.pop();
        } else {
          break;
        }
      }
      stack.push(token);
    } else if (token == "(") {
      stack.push(token);
    } else if (token == ")") {
      bool find = false;
      while (!stack.empty()) {
        if (stack.top() == "(") {
          find = true;
          stack.pop();
          break;
        } else {
          queue.push(stack.top());
          stack.pop();
        }
      }
      if (!find) {
        std::cout << "Error: parentheses mismatched 1\n";
        return false;
      }
    } else {
      std::cout << "error expression:" << token << "\n";
      return false;
    }
  }
  while (!stack.empty()) {
    if (stack.top() == "(" || stack.top() == ")") {
      std::cout << "Error: parentheses mismatched 2\n";
      return false;
    }
    queue.push(stack.top());
    stack.pop();
  }
  return true;
}

NodePtr ShuntingYard::build_expression_tree(std::queue<std::string> &queue) {
  std::stack<NodePtr> stack;
  int temporal_varible_index = 0;

  while (!queue.empty()) {
    ++temporal_varible_index;
    const std::string &token = queue.front();
    if (is_const(token)) {
      NodePtr node;
      if (varible_node_map.find(token) != varible_node_map.end()) {
        node = varible_node_map[token];
      } else {
        node = std::make_shared<Node>();
        node->op_count = -1;
        // Not used, then cancel
        // node->result_varible = token;
        node->result_varible_index = temporal_varible_index;
        node->value = paser_value(token);
        //特化下bool，防止输入常量bool时，缓存失效
        node->bool_value = node->value->get_bool();
      }
      varible_node_map[token] = node;
      stack.push(node);
    } else if (is_operator(token)) {
      size_t count = op_arg_count(token);
      NodePtr node = std::make_shared<Node>();
      node->op_count = count;
      node->operator_type = operator_map[token];
      std::stack<NodePtr> temp_stack;
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
    } else if (is_varible(token)) {
      //如果变量是同一个，就索引
      NodePtr node;
      if (varible_node_map.find(token) != varible_node_map.end()) {
        node = varible_node_map[token];
      } else {
        node = std::make_shared<Node>();
        node->op_count = 0;
        node->result_varible = token;
        varible_map[node->result_varible] = &(node->value);
        std::cout << "init varible:" << node->result_varible
                  << ", loc:" << &(node->value) << "\n";
        varible_node_map[token] = node;
      }
      // Not used, then cancel
      // node->result_varible_index = temporal_varible_index;
      stack.push(node);
    }
    queue.pop();
  }
  if (stack.size() == 1) {
    return stack.top();
  }
  return nullptr;
}

void ShuntingYard::op_varible(NodePtr &node) {

  switch (node->operator_type) {
  case kNOT:
    node->bool_value = !node->first->value->get_bool();
    break;
  case kFUNC1:
    node->value = func_varible(node->first->value);
    break;
  default:
    // ADD ERROR TIP
    node->value = std::make_shared<Value>();
  };
};
void ShuntingYard::op_varible_varible(NodePtr &node) {
  const NodePtr &first = node->first;
  const NodePtr &second = node->second;
  switch (node->operator_type) {
  case kAND: {
    node->bool_value = first->bool_value && second->bool_value;
  } break;
  case kOR: {
    node->bool_value = first->bool_value || second->bool_value;
  } break;
  case kADD:
    node->value = first->value->add(second->value);
    break;
  case kSUB:
    node->value = first->value->sub(second->value);
    break;
  case kMUL:
    node->value = first->value->mul(second->value);
    break;
  case kDIV:
    node->value = first->value->div(second->value);
    break;
  case kMOD:
    node->value = first->value->mod(second->value);
    break;
  case kIN:
    node->bool_value = first->value->in(second->value);
    break;
  case kGREATE:
    node->bool_value = first->value->gt(second->value);
    break;
  case kGREATEOREQUAL:
    node->bool_value = first->value->gt_or_equal(second->value);
    break;
  case kLOWER:
    node->bool_value = first->value->lt(second->value);
    break;
  case kLOWEROREQUAL:
    node->bool_value = first->value->lt_or_equal(second->value);
    break;
  case kEQUAL:
    node->bool_value = first->value->equal(second->value);
    break;
  case kFUNC2:
    node->value = func_varible_varible(node->first->value, node->second->value);
    break;
  default:
    std::cout << "not support operator\n";
    node->value = std::make_shared<Value>();
  };
}

void ShuntingYard::op_varible_varible_varible(NodePtr &node) {
  switch (node->operator_type) {
  case kFUNC3:
    node->value = func_varible_varible_varible(
        node->first->value, node->second->value, node->third->value);
    break;
  default:
    // ADD ERROR TIP
    node->value = std::make_shared<Value>();
  };
};

void ShuntingYard::eval_expression(NodePtr &node) {
  switch (node->op_count) {
  case 3: {
    eval_expression(node->first);
    eval_expression(node->second);
    eval_expression(node->third);
    op_varible_varible_varible(node);
  }
  case 2: {
    eval_expression(node->first);
    eval_expression(node->second);
    op_varible_varible(node);
    return;
  } break;
  case 1: {
    eval_expression(node->first);
    op_varible(node);
    return;
  } break;
  case 0: //变量取值
    //赋值的时候已经初始化完
    // node->value = varibles[node->result_varible];
    return;
    break;
  case -1: //常量在初始化就做好
    // node->value = const_varibles[node->result_varible_index];
    return;
    break;
  default:
    std::cout << "error count\n";
  };
}

bool ShuntingYard::eval_bool() {
  eval_expression(expression_root);
  return expression_root->bool_value;
}

ValuePtr ShuntingYard::eval() {
  eval_expression(expression_root);
  return expression_root->value;
}

} // namespace parse
