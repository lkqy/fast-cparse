#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <string.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace parse {

enum OperateType {
  kADD = 1,
  kSUB = 2,
  kMUL = 3,
  kDIV = 4,
  kIN = 5,
  kGREATE = 6,
  kGREATEOREQUAL = 7,
  kLOWER = 8,
  kLOWEROREQUAL = 9,
  kEQUAL = 10,
  kNOT = 11,
  kAND = 12,
  kOR = 13,
  kLEFTPARENTHESIS = 14,  //(
  kRIGHTPARENTHESIS = 15, //)
  kMOD = 16,
  kFUNC1 = 17, // 1参数，一个返回值
  kFUNC2 = 18, // 2参数，一个返回值
  kFUNC3 = 19, // 3参数，一个返回值
  kVEC = 20,   // 一个数组，暂时只支持常量
  kSET = 20,   // 一个集合，暂时只支持常量
};

enum ValueType {
  vNull = 0,
  vInt = 1,
  vLong = 2,
  vFloat = 3,
  vDouble = 4,
  vString = 5,
  vBool = 6,
  vUserType = 7,
  vVecBool = 8,
  vVecInt = 9,
  vVecLong = 10,
  vVecFloat = 11,
  vVecDouble = 12,
  vVecString = 13,
  vMapBool = 14,
  vMapInt = 15,
  vMapLong = 16,
  vMapFloat = 17,
  vMapDouble = 18,
  vMapString = 19,
  vSetBool = 20,
  vSetInt = 22,
  vSetLong = 22,
  vSetFloat = 23,
  vSetDouble = 24,
  vSetString = 25,
};

enum TokenType {
  tOperator = 1,
  tConst = 2,
  tVarible = 3,
  tLeftParenthesis = 4,
  tInnerFunction = 100000,      // 占用10万个空位
  tFunctionBeginIndex = 200000, //占用10万个空位
};

class Value;
typedef std::shared_ptr<Value> ValuePtr;

class Value {
  ValueType _value_type;

public:
  Value() : _value_type(vNull){};
  Value(ValueType t) : _value_type(t){};
  ValueType type() const { return _value_type; };
  virtual ~Value(){};
  virtual bool get_bool() const { return false; };
  // a == b => a.equal(b)
  virtual bool equal(ValuePtr &v) const { return false; };
  // a in b => b.in(a), 右结合
  virtual bool in(ValuePtr &v) const { return false; };
  // a > b => a.gt(b)
  virtual bool gt(ValuePtr &v) const { return false; };
  // a >= b => a.gt_or_equal(b)
  virtual bool gt_or_equal(ValuePtr &v) const { return false; };
  // a < b => a.lt(b)
  virtual bool lt(ValuePtr &v) const { return false; };
  // a <= b => a.lt_or_equal(b)
  virtual bool lt_or_equal(ValuePtr &v) const { return false; };
  // a + b => a.add(b)
  virtual ValuePtr add(ValuePtr &v) const { return std::make_shared<Value>(); };
  // a - b => a.sub(b)
  virtual ValuePtr sub(ValuePtr &v) const { return std::make_shared<Value>(); };
  // a * b => a.mul(b)
  virtual ValuePtr mul(ValuePtr &v) const { return std::make_shared<Value>(); };
  // a / b => a.div(b)
  virtual ValuePtr div(ValuePtr &v) const { return std::make_shared<Value>(); };
  // a % b => a.mod(b)
  virtual ValuePtr mod(ValuePtr &v) const { return std::make_shared<Value>(); };
};

class BoolValue : public Value {

public:
  bool val;
  BoolValue(bool v) : Value(vBool), val(v){};
  ~BoolValue(){};
  bool get_bool() const { return val; };
  // a == b => a.equal(b)
  bool equal(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<BoolValue>(v);
      return val == _v->val;
    }
    return false;
  };
};

template <typename T, ValueType value_type> class NumberValue : public Value {
  typedef NumberValue<T, value_type> NumberValue_TYPE;

public:
  T val;
  NumberValue(T v) : Value(value_type), val(v){};
  ~NumberValue(){};
  bool get_bool() const { return val != 0; };
  T get_value() { return val; };
  bool equal(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
      return val == _v->val;
    }
    return false;
  };
  bool gt(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
      return val > _v->val;
    }
    return false;
  };
  bool lt(ValuePtr &v) const {

    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
      return val < _v->val;
    }
    return false;
  };
  ValuePtr add(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
      return std::make_shared<NumberValue_TYPE>(val + _v->val);
    }
    return std::make_shared<NumberValue_TYPE>(val);
  };
  ValuePtr sub(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
      return std::make_shared<NumberValue_TYPE>(val - _v->val);
    }
    return std::make_shared<NumberValue_TYPE>(val);
  };
  ValuePtr mul(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
      return std::make_shared<NumberValue_TYPE>(val * _v->val);
    }
    return std::make_shared<NumberValue_TYPE>(val);
  };
  ValuePtr div(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
      return std::make_shared<NumberValue_TYPE>(val / _v->val);
    }
    return std::make_shared<NumberValue_TYPE>(val);
  };
  ValuePtr mod(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
      long _m = (long)val % (long)_v->val;
      return std::make_shared<NumberValue_TYPE>((T)_m);
    }
    return std::make_shared<NumberValue_TYPE>(val);
  };
};

class StringValue : public Value {

public:
  std::string val;
  StringValue(const std::string &v) : Value(vString), val(v){};
  bool get_bool() const { return !val.empty(); }
  bool equal(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<StringValue>(v);
      return val == _v->val;
    }
    return false;
  }
  bool in(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<StringValue>(v);
      if (val.empty())
        return true;
      if (_v->val.empty())
        return false;
      return _v->val.find(val) == std::string::npos;
    }
    return false;
  }
  bool gt(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<StringValue>(v);
      return val.compare(_v->val) > 0;
    }
    return false;
  }
  bool gt_or_equal(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<StringValue>(v);
      return val.compare(_v->val) >= 0;
    }
    return false;
  }
  bool lt(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<StringValue>(v);
      return val.compare(_v->val) < 0;
    }
    return false;
  }
  bool lt_or_equal(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<StringValue>(v);
      return val.compare(_v->val) <= 0;
    }
    return false;
  }
};

template <typename T, ValueType value_type, typename H, ValueType H_type>
class VectorValue : public Value {
  typedef VectorValue<T, value_type, H, H_type> VectorValue_TYPE;

public:
  std::vector<T> val;
  VectorValue(std::vector<T> &v) : Value(value_type), val(v){};
  VectorValue(std::vector<ValuePtr> &vals) : Value(value_type) {
    for (auto &v : vals) {
      if (v->type() == H_type) {
        auto _v = std::dynamic_pointer_cast<H>(v);
        val.push_back(_v->val);
      }
    }
  };
  bool get_bool() const { return val.size() > 0; }
  bool equal(ValuePtr &v) const {
    if (v->type() == H_type) {
      auto _v = std::dynamic_pointer_cast<VectorValue_TYPE>(v);
      if (_v->val.size() != val.size())
        return false;
      for (size_t i = 0; i < val.size(); ++i) {
        if (val[i] != _v->val[i])
          return false;
      }
      return true;
    }
    return false;
  }
  bool in(ValuePtr &v) const {
    if (v->type() == H_type) {
      auto _v = std::dynamic_pointer_cast<H>(v);
      return std::find(val.begin(), val.end(), _v->val) != val.end();
    }
    return false;
  }
};

template <typename T, ValueType value_type> class MapValue : public Value {
  typedef MapValue<T, value_type> MapValue_TYPE;
  typedef std::unordered_map<std::string, T> map_type;
  map_type val;

public:
  MapValue(map_type &v) : Value(value_type), val(v){};
  bool get_bool() const { return val.size() > 0; }
  bool equal(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<MapValue_TYPE>(v);
      if (_v->val.size() != val.size())
        return false;
      for (auto it : val) {
        auto fit = _v->val.find(it->first);
        if (fit == _v->val.end())
          return false;
        if (fit->second != it->second)
          return false;
      }
      return true;
    }
    return false;
  }
  //只允许String 为key的类型
  bool in(ValuePtr &v) const {
    if (v->type() == type()) {
      auto _v = std::dynamic_pointer_cast<StringValue>(v);
      return val.find(_v->val) != val.end();
    }
    return false;
  }
};

template <typename T, ValueType value_type, typename H, ValueType H_type>
class SetValue : public Value {
  typedef SetValue<T, value_type, H, H_type> SetValue_TYPE;
  typedef std::unordered_set<T> set_type;

public:
  set_type val;
  SetValue(set_type &v) : Value(value_type), val(v){};
  SetValue(std::vector<ValuePtr> &vals) : Value(value_type) {
    for (auto &v : vals) {
      if (v->type() == H_type) {
        auto _v = std::dynamic_pointer_cast<H>(v);
        val.insert(_v->val);
      }
    }
  };
  bool get_bool() const { return val.size() > 0; }
  bool equal(ValuePtr &v) const {
    if (v->type() == H_type) {
      auto _v = std::dynamic_pointer_cast<SetValue_TYPE>(v);
      if (_v->val.size() != val.size())
        return false;
      for (auto &it : val) {
        auto fit = _v->val.find(it);
        if (fit == _v->val.end())
          return false;
        if (*fit != it)
          return false;
      }
      return true;
    }
    return false;
  }
  bool in(ValuePtr &v) const {
    if (v->type() == H_type) {
      auto _v = std::dynamic_pointer_cast<H>(v);
      return val.find(_v->val) != val.end();
    }
    return false;
  }
};

template <typename T> class UserValue : public Value {
  typedef UserValue<T> UserValue_TYPE;

public:
  UserValue_TYPE value;
  UserValue(UserValue_TYPE &v) : Value(vUserType), value(v){};
};

ValuePtr to_vec_value(std::vector<ValuePtr> &values);
ValuePtr to_set_value(std::vector<ValuePtr> &values);

typedef NumberValue<int, vInt> IntValue;
typedef NumberValue<long, vLong> LongValue;
typedef NumberValue<float, vFloat> FloatValue;
typedef NumberValue<double, vDouble> DoubleValue;
typedef VectorValue<bool, vVecBool, BoolValue, vBool> BoolVecValue;
typedef VectorValue<int, vVecInt, IntValue, vInt> IntVecValue;
typedef VectorValue<long, vVecLong, LongValue, vLong> LongVecValue;
typedef VectorValue<float, vVecFloat, FloatValue, vFloat> FloatVecValue;
typedef VectorValue<double, vVecDouble, DoubleValue, vDouble> DoubleVecValue;
typedef VectorValue<std::string, vVecString, StringValue, vString> StringVecValue;
typedef MapValue<bool, vMapBool> BoolMapValue;
typedef MapValue<int, vMapInt> IntMapValue;
typedef MapValue<long, vMapLong> LongMapValue;
typedef MapValue<float, vMapFloat> FloatMapValue;
typedef MapValue<double, vMapDouble> DoubleMapValue;
typedef MapValue<std::string, vMapString> StringMapValue;
typedef SetValue<bool, vSetBool, BoolValue, vBool> BoolSetValue;
typedef SetValue<int, vSetInt, IntValue, vInt> IntSetValue;
typedef SetValue<long, vSetLong, LongValue, vLong> LongSetValue;
typedef SetValue<float, vSetFloat, FloatValue, vFloat> FloatSetValue;
typedef SetValue<double, vSetDouble, DoubleValue, vDouble> DoubleSetValue;
typedef SetValue<std::string, vSetString, StringValue, vString> StringSetValue;

} // namespace parse
