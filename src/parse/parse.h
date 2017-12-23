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
#include <vector>

namespace parse {

class Node;
typedef std::shared_ptr<Node> NodePtr;

class Node {
public:
  int op_count;
  int result_varible_index;
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
class ShuntingYard {

public:
  ShuntingYard(const std::string &e);
  virtual ~ShuntingYard(){};
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
  ValuePtr paser_value(const std::string &token);

  bool build_visit_queue(std::queue<std::string> &queue);
  NodePtr build_expression_tree(std::queue<std::string> &queue);

  void op_varible(NodePtr &node);
  void op_varible_varible(NodePtr &node);
  void op_varible_varible_varible(NodePtr &node);

  virtual ValuePtr func_varible(ValuePtr &v) {
    // TODO Add error tip
    return std::make_shared<Value>();
  };
  virtual ValuePtr func_varible_varible(ValuePtr &v1, ValuePtr &v2) {
    // TODO Add error tip
    return std::make_shared<Value>();
  };
  virtual ValuePtr func_varible_varible_varible(ValuePtr &v1, ValuePtr &v2,
                                                ValuePtr &v3) {
    // TODO Add error tip
    return std::make_shared<Value>();
  };

  void eval_expression(NodePtr &node);

  bool eval_bool();

  ValuePtr eval();

  void set_varible(std::string key, ValuePtr value) {
    const auto &it = varible_map.find(key);
    if (it == varible_map.end())
      return;
    *(it->second) = value;
  };
  /*
  void set_varible(std::string& key, ValuePtr& value) {
      const auto& it = varible_map.find(key);
      if (it== varible_map.end()) return;
      *(it->second) = value;
  };
  */

private:
  //操作符
  std::unordered_map<std::string, OperateType> operator_map = {
      {"+", kADD},           {"-", kSUB},    {"*", kMUL},
      {"/", kDIV},           {"%", kMOD},    {"in", kIN},
      {">", kGREATE},        {"<", kLOWER},  {">=", kGREATEOREQUAL},
      {"<=", kLOWEROREQUAL}, {"==", kEQUAL}, {"not", kNOT},
      {"and", kAND},         {"or", kOR}};

  //源表达式
  std::string expression;

  typedef std::shared_ptr<ValuePtr> ValuePtrPtr;

  //变量，初始化索引到表达式树上，以省去访问map的时间
  std::unordered_map<std::string, ValuePtr *> varible_map;

  //检索node->(token, node)
  std::unordered_map<std::string, NodePtr> varible_node_map;

  //表达式节点树
  NodePtr expression_root;
};

template <typename RET_T> class ShuntingYardFun0 : public ShuntingYard {
  typedef std::function<RET_T()> Func;

public:
  ShuntingYardFun0(const std::string &e) : ShuntingYard(e){};
  ValuePtr eval(std::unordered_map<std::string, ValuePtr> &varibles,
                std::unordered_map<std::string, Func> &funcs);
};

template <typename RET_T, typename PARAM_T>
class ShuntingYardFun1 : public ShuntingYard {
  typedef std::function<RET_T(PARAM_T)> Func;

public:
  ShuntingYardFun1(const std::string &e) : ShuntingYard(e){};
  ValuePtr eval(std::unordered_map<std::string, ValuePtr> &varibles,
                std::unordered_map<std::string, Func> &funcs);
};

template <typename RET_T, typename PARAM1_T, typename PARAM2_T>
class ShuntingYardFun2 : public ShuntingYard {
  typedef std::function<RET_T(PARAM1_T, PARAM2_T)> Func;

public:
  ShuntingYardFun2(const std::string &e) : ShuntingYard(e){};
  ValuePtr eval(std::unordered_map<std::string, ValuePtr> &varibles,
                std::unordered_map<std::string, Func> &funcs);
};

template <typename RET_T, typename PARAM1_T, typename PARAM2_T,
          typename PARAM3_T>
class ShuntingYardFun3 : public ShuntingYard {
  typedef std::function<RET_T(PARAM1_T, PARAM2_T, PARAM3_T)> Func;

public:
  virtual void op_varible_varible_varible(NodePtr &node);
  ShuntingYardFun3(const std::string &e) : ShuntingYard(e){};
  ValuePtr eval(std::unordered_map<std::string, ValuePtr> &varibles,
                std::unordered_map<std::string, Func> &funcs);
};

} // namespace parse
