#pragma once
#include "parse/value_type.h"
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <string.h>
#include <string>
#include <time.h>
#include <unordered_map>
#include <map>
#include <vector>

namespace parse {

class Node;
typedef std::shared_ptr<Node> NodePtr;

typedef std::function<ValuePtr(const ValuePtr &)> BindFunc1;
typedef std::function<ValuePtr(const ValuePtr &, const ValuePtr &)> BindFunc2;
typedef std::function<ValuePtr(const ValuePtr &, const ValuePtr &,
                               const ValuePtr &)>
    BindFunc3;

class Node {
public:
  int op_count;
  OperateType operator_type;
  //由于使用中，大多数情况都是bool类型，特cache一个
  //测试发现，cache后，性能翻倍
  bool bool_value;
  ValuePtr value;
  //可能使用到的操作数
  NodePtr first;
  NodePtr second;
  NodePtr third;
  std::string result_varible;
};

// TODO: 要不要做范型？施加更强的计算约束换更高的性能

class ShuntingYard;
typedef std::shared_ptr<ShuntingYard> ShuntingYardPtr;
class ShuntingYard {
private:
  inline bool skip_char(char c) {
    if (c == ' ' || c == '\t')
      return true;
    return false;
  };
  inline bool is_parentheses(char c) { return c == '(' || c == ')'; };
  // 快速实现词法分析里的token分析
  inline int get_token_type(char c);
  inline bool is_terminate(char c) { return c == '\0'; };

  //是否左结合
  inline bool op_left_assoc(const std::string &op);
  //运算符优先级
  int op_preced(const std::string op);
  //运算符操作数
  size_t op_arg_count(const std::string &op);

  //词法分析
  std::string next_token(const char *input, const char **next_token);
  //语义转换
  bool is_const(const std::string &token);
  bool is_varible(const std::string &token);
  bool is_operator(const std::string &token);
  bool is_function(const std::string &token);
  bool is_inner_function(const std::string &token);
  ValuePtr paser_value(const std::string &token);

  bool build_visit_queue(std::queue<std::pair<std::string, int>> &queue);
  NodePtr build_expression_tree(std::queue<std::pair<std::string, int>> &queue);

  void op_varible(NodePtr &node);
  void op_varible_varible(NodePtr &node);
  void op_varible_varible_varible(NodePtr &node);

  void eval_expression(NodePtr &node);

public:
  ShuntingYard(const std::string &e);
  ~ShuntingYard(){};

  //返回表达式是否正常
  bool compile();

  bool eval_bool();

  ValuePtr eval();

  void set_varible(const std::string& key, ValuePtr& value) {
    const auto &it = varible_map.find(key);
    if (it == varible_map.end())
      return;
    *(it->second) = value;
  };

  void set_func1(std::string key, BindFunc1 f) { func1_map[key] = f; };
  void set_func2(std::string key, BindFunc2 f) { func2_map[key] = f; };
  void set_func3(std::string key, BindFunc3 f) { func3_map[key] = f; };
  const std::string& get_log() {return status_log;};

  /* TODO make it more easy to use
  static ShuntingYardPtr get_instance(const std::string &expression) {
    if (sy_instance_map.find(expression) == sy_instance_map.end()) {
      sy_instance_map[expression] = std::make_shared<ShuntingYard>(expression);
    }
    return sy_instance_map[expression];
  };
  */

private:
  //操作符
  std::unordered_map<std::string, OperateType> operator_map = {
      {"+", kADD},           {"-", kSUB},    {"*", kMUL},
      {"/", kDIV},           {"%", kMOD},    {"in", kIN},
      {">", kGREATE},        {"<", kLOWER},  {">=", kGREATEOREQUAL},
      {"<=", kLOWEROREQUAL}, {"==", kEQUAL}, {"not", kNOT},
      {"and", kAND},         {"or", kOR},
  };

  std::unordered_map<std::string, OperateType> inner_function_map = {
      {"VEC", kVEC}, {"SET", kSET},
  };

  // 函数类型
  std::unordered_map<int, OperateType> function_map = {
      {1, kFUNC1}, {2, kFUNC2}, {3, kFUNC3}
  };
  //源表达式
  std::string expression;

  //变量，初始化索引到表达式树上，以省去访问map的时间
  std::map<std::string, ValuePtr *> varible_map;

  //检索node->(token, node)
  std::unordered_map<std::string, NodePtr> varible_node_map;

  std::unordered_map<std::string, BindFunc1> func1_map;
  std::unordered_map<std::string, BindFunc2> func2_map;
  std::unordered_map<std::string, BindFunc3> func3_map;

  //表达式节点树
  NodePtr expression_root;

  //
  std::string status_log;
  //保存所有初始化实例，减少内存占用，这里需要考虑线程安全，以防止同时修改数据
  static std::unordered_map<std::string, ShuntingYardPtr> sy_instance_map;
};

}; // namespace parse
