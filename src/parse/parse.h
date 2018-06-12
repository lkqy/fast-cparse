#pragma once
#include "parse/value_type.h"
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <stack>
#include <string.h>
#include <string>
#include <time.h>
#include <unordered_map>
#include <vector>

namespace parse {

class Node;
typedef std::shared_ptr<Node> NodePtr;

typedef std::function<Value *(Value *)> BindFunc1;
typedef std::function<Value *(Value *, Value *)> BindFunc2;
typedef std::function<Value *(Value *, Value *, Value *)> BindFunc3;

class Node {
public:
  Node() : value(nullptr), first(nullptr), second(nullptr), third(nullptr){};
  ~Node() {
    if (value)
      delete value;
    if (first)
      delete first;
    if (second)
      delete second;
    if (third)
      delete third;
    value = nullptr;
    first = nullptr;
    second = nullptr;
    third = nullptr;
  };

public:
  int op_count;
  OperateType operator_type;
  //由于使用中，大多数情况都是bool类型，特cache一个
  //测试发现，cache后，性能翻倍
  bool bool_value;
  Value *value;
  //可能使用到的操作数
  Node *first;
  Node *second;
  Node *third;
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
  inline int get_token_type(char c, int pre_type);
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
  Value *paser_value(const std::string &token);
  Value *paser_varible(const std::string &token);

  bool build_visit_queue(std::queue<std::pair<std::string, int>> &queue);
  Node *build_expression_tree(std::queue<std::pair<std::string, int>> &queue);

  Value *op_varible(const std::string &result_varible,
                    OperateType operator_type, Value *value, std::string &log);
  inline Value *op_varible_varible(const std::string &result_varible,
                                   OperateType operator_type, Value *first,
                                   Value *second, std::string &log);
  inline Value *op_varible_varible_varible(const std::string &result_varible,
                                           OperateType operator_type,
                                           Value *first, Value *second,
                                           Value *third, std::string &log);

  inline Value *
  eval_expression(Node *node,
                  std::vector<std::pair<const char *, Value *>> &varibles,
                  std::string &log);

  Value *find_varible(std::vector<std::pair<const char *, Value *>> &varibles,
                      std::string &key, std::string &log);

public:
  ShuntingYard(const std::string &e);
  ~ShuntingYard() {
    if (expression_root)
      delete expression_root;
    expression_root = nullptr;
  };

  //返回表达式是否正常
  bool compile();

  bool eval_bool(std::vector<std::pair<const char *, Value *>> &varibles,
                 std::string &log);
  int eval_int(std::vector<std::pair<const char *, Value *>> &varibles,
               std::string &log);
  long eval_long(std::vector<std::pair<const char *, Value *>> &varibles,
                 std::string &log);
  float eval_float(std::vector<std::pair<const char *, Value *>> &varibles,
                   std::string &log);
  double eval_double(std::vector<std::pair<const char *, Value *>> &varibles,
                     std::string &log);

  Value *eval(std::vector<std::pair<const char *, Value *>> &varibles,
              std::string &log);

  void set_func1(std::string key, BindFunc1 f) { func1_map[key] = f; };
  void set_func2(std::string key, BindFunc2 f) { func2_map[key] = f; };
  void set_func3(std::string key, BindFunc3 f) { func3_map[key] = f; };
  const std::string &get_log() { return status_log; };

  std::string &get_str_const(std::string str) {
    const_strings.push_back(str);
    return const_strings[const_strings.size() - 1];
  };
  std::string get_expression() { return expression; };

private:
  void free_result(Value *v);
  Value *_eval(std::vector<std::pair<const char *, Value *>> &varibles,
               std::string &log);
  //操作符
  std::unordered_map<std::string, OperateType> operator_map = {
      {"+", kADD},           {"-", kSUB},    {"*", kMUL},
      {"/", kDIV},           {"%", kMOD},    {"in", kIN},
      {">", kGREATE},        {"<", kLOWER},  {">=", kGREATEOREQUAL},
      {"<=", kLOWEROREQUAL}, {"==", kEQUAL}, {"not", kNOT},
      {"!=", kNOTEQUAL},     {"and", kAND},  {"or", kOR},
  };

  std::unordered_map<std::string, OperateType> inner_function_map = {
      {"VEC", kVEC}, {"SET", kSET},
  };

  // 函数类型
  std::unordered_map<int, OperateType> function_map = {
      {1, kFUNC1}, {2, kFUNC2}, {3, kFUNC3}};
  //源表达式
  std::string expression;

  //变量，初始化索引到表达式树上，以省去访问map的时间
  std::vector<std::string> const_strings;

  std::unordered_map<std::string, BindFunc1> func1_map;
  std::unordered_map<std::string, BindFunc2> func2_map;
  std::unordered_map<std::string, BindFunc3> func3_map;

  //表达式节点树
  Node *expression_root;

  //
  std::string status_log;
};

// 非线程安全，只能在同一个线程中使用
class ShuntingYardManager {
public:
  ShuntingYardManager(){};
  ShuntingYardPtr &get_instance(const std::string expression) {
    if (sy_instance_map.find(expression) == sy_instance_map.end()) {
      sy_instance_map[expression] = std::make_shared<ShuntingYard>(expression);
      sy_instance_map[expression]->compile();
    }
    return sy_instance_map[expression];
  };

private:
  //保存所有初始化实例，减少内存占用，这里需要考虑线程安全，以防止同时修改数据
  std::unordered_map<std::string, ShuntingYardPtr> sy_instance_map;
};

}; // namespace parse
