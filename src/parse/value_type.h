#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
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
  kNOTEQUAL = 11,
  kNOT = 12,
  kAND = 13,
  kOR = 14,
  kLEFTPARENTHESIS = 15,  //(
  kRIGHTPARENTHESIS = 16, //)
  kMOD = 17,
  kFUNC1 = 18, // 1参数，一个返回值
  kFUNC2 = 19, // 2参数，一个返回值
  kFUNC3 = 20, // 3参数，一个返回值
  kVEC = 21,   // 一个数组，暂时只支持常量
  kSET = 22,   // 一个集合，暂时只支持常量
  kPOW = 23,   // 2 ^ 3 = 8
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
  virtual Value *clone() { return new Value(); };
  virtual bool get_bool() { return false; };
  // a == b => a.equal(b)
  virtual bool _and(Value *v) { return false; };
  virtual bool _or(Value *v) { return false; };
  virtual bool _not() { return false; };

  virtual bool equal(Value *v) { return false; };
  // a in b => b.in(a), 右结合
  virtual bool in(Value *v) { return false; };
  // a > b => a.gt(b)
  virtual bool gt(Value *v) { return false; };
  // a >= b => a.gt_or_equal(b)
  virtual bool gt_or_equal(Value *v) { return false; };
  // a < b => a.lt(b)
  virtual bool lt(Value *v) { return false; };
  // a <= b => a.lt_or_equal(b)
  virtual bool lt_or_equal(Value *v) { return false; };
  // a + b => a.add(b)
  virtual void add(Value *v, Value *result){};
  // a - b => a.sub(b)
  virtual void sub(Value *v, Value *result){};
  // a * b => a.mul(b)
  virtual void mul(Value *v, Value *result){};
  // a / b => a.div(b)
  virtual void div(Value *v, Value *result){};
  // a % b => a.mod(b)
  virtual void mod(Value *v, Value *result){};
  // negative: 1 => -1, or -1 => 1
  virtual void negative(){};
  // a / b => a.div(b)
  virtual void s_div(Value *v, Value *result){};
  // string
  virtual std::string to_string() { return ""; };
  // a ^ b => pow(a, b)
  virtual void pow(Value *v, Value *result){};
};

class BoolValue : public Value {

public:
  bool val;
  BoolValue(bool v) : Value(vBool), val(v){};
  ~BoolValue(){};
  virtual Value *clone() { return new BoolValue(val); };
  bool get_bool() { return val; };
  bool _and(Value *v) {
    if (v->type() == type()) {
      auto _v = (BoolValue *)v;
      return val && _v->val;
    }
    return false;
  };
  bool _or(Value *v) {
    if (v->type() == type()) {
      auto _v = (BoolValue *)v;
      return val || _v->val;
    }
    return false;
  };
  bool _not() { return !val; };
  // a == b => a.equal(b)
  bool equal(Value *v) {
    if (v->type() == type()) {
      auto _v = (BoolValue *)v;
      return val == _v->val;
    }
    return false;
  };
  // string
  std::string to_string() {
    std::stringstream ss;
    ss << val;
    return ss.str();
  };
};

template <typename T, ValueType value_type> class NumberValue : public Value {
  typedef NumberValue<T, value_type> NumberValue_TYPE;
  typedef NumberValue<int, vInt> IntValue_TYPE;
  typedef NumberValue<long, vLong> LongValue_TYPE;
  typedef NumberValue<float, vFloat> FloatValue_TYPE;
  typedef NumberValue<double, vDouble> DoubleValue_TYPE;

public:
  T val;
  NumberValue(T v) : Value(value_type), val(v){};
  ~NumberValue(){};
  virtual Value *clone() { return new NumberValue_TYPE(val); };
  bool get_bool() { return val != 0; };
  T get_value() { return val; };
  bool equal(Value *v) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      return val == _v->val;
    }
    return false;
  };
  bool gt(Value *v) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      return val > _v->val;
    } else {
      if (v->type() == vInt) {
        return val > ((IntValue_TYPE *)v)->val;
      } else if (v->type() == vDouble) {
        return val > ((DoubleValue_TYPE *)v)->val;
      } else if (v->type() == vLong) {
        return val > ((LongValue_TYPE *)v)->val;
      } else if (v->type() == vFloat) {
        return val > ((FloatValue_TYPE *)v)->val;
      }
    }
    return false;
  };
  bool gt_or_equal(Value *v) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      return val >= _v->val;
    } else {
      if (v->type() == vInt) {
        return val >= ((IntValue_TYPE *)v)->val;
      } else if (v->type() == vDouble) {
        return val >= ((DoubleValue_TYPE *)v)->val;
      } else if (v->type() == vLong) {
        return val >= ((LongValue_TYPE *)v)->val;
      } else if (v->type() == vFloat) {
        return val >= ((FloatValue_TYPE *)v)->val;
      }
    }
    return false;
  };
  bool lt(Value *v) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      return val < _v->val;
    } else {
      if (v->type() == vInt) {
        return val < ((IntValue_TYPE *)v)->val;
      } else if (v->type() == vDouble) {
        return val < ((DoubleValue_TYPE *)v)->val;
      } else if (v->type() == vLong) {
        return val < ((LongValue_TYPE *)v)->val;
      } else if (v->type() == vFloat) {
        return val < ((FloatValue_TYPE *)v)->val;
      }
    }
    return false;
  };
  bool lt_or_equal(Value *v) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      return val <= _v->val;
    } else {
      if (v->type() == vInt) {
        return val <= ((IntValue_TYPE *)v)->val;
      } else if (v->type() == vDouble) {
        return val <= ((DoubleValue_TYPE *)v)->val;
      } else if (v->type() == vLong) {
        return val <= ((LongValue_TYPE *)v)->val;
      } else if (v->type() == vFloat) {
        return val <= ((FloatValue_TYPE *)v)->val;
      }
    }
    return false;
  };
  void add(Value *v, Value *result) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      auto _r = (NumberValue_TYPE *)result;
      _r->val = val + _v->val;
    } else {
      auto _r = (NumberValue_TYPE *)result;
      if (v->type() == vInt) {
        auto _v = (IntValue_TYPE *)v;
        _r->val = val + _v->val;
      } else if (v->type() == vDouble) {
        auto _v = (DoubleValue_TYPE *)v;
        _r->val = val + _v->val;
      } else if (v->type() == vLong) {
        auto _v = (LongValue_TYPE *)v;
        _r->val = val + _v->val;
      } else if (v->type() == vFloat) {
        auto _v = (FloatValue_TYPE *)v;
        _r->val = val + _v->val;
      }
    }
  };
  void sub(Value *v, Value *result) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      auto _r = (NumberValue_TYPE *)result;
      _r->val = val - _v->val;
    } else {
      auto _r = (NumberValue_TYPE *)result;
      if (v->type() == vInt) {
        auto _v = (IntValue_TYPE *)v;
        _r->val = val - _v->val;
      } else if (v->type() == vDouble) {
        auto _v = (DoubleValue_TYPE *)v;
        _r->val = val - _v->val;
      } else if (v->type() == vLong) {
        auto _v = (LongValue_TYPE *)v;
        _r->val = val - _v->val;
      } else if (v->type() == vFloat) {
        auto _v = (FloatValue_TYPE *)v;
        _r->val = val - _v->val;
      }
    }
  };
  void mul(Value *v, Value *result) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      auto _r = (NumberValue_TYPE *)result;
      _r->val = val * _v->val;
    } else {
      auto _r = (NumberValue_TYPE *)result;
      if (v->type() == vInt) {
        auto _v = (IntValue_TYPE *)v;
        _r->val = val * _v->val;
      } else if (v->type() == vDouble) {
        auto _v = (DoubleValue_TYPE *)v;
        _r->val = val * _v->val;
      } else if (v->type() == vLong) {
        auto _v = (LongValue_TYPE *)v;
        _r->val = val * _v->val;
      } else if (v->type() == vFloat) {
        auto _v = (FloatValue_TYPE *)v;
        _r->val = val * _v->val;
      }
    }
  };
  void div(Value *v, Value *result) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      auto _r = (NumberValue_TYPE *)result;
      _r->val = val / _v->val;
    } else {
      auto _r = (NumberValue_TYPE *)result;
      if (v->type() == vInt) {
        auto _v = (IntValue_TYPE *)v;
        _r->val = val / _v->val;
      } else if (v->type() == vDouble) {
        auto _v = (DoubleValue_TYPE *)v;
        _r->val = val / _v->val;
      } else if (v->type() == vLong) {
        auto _v = (LongValue_TYPE *)v;
        _r->val = val / _v->val;
      } else if (v->type() == vFloat) {
        auto _v = (FloatValue_TYPE *)v;
        _r->val = val / _v->val;
      }
    }
  };
  void s_div(Value *v, Value *result) {
    auto _r = (DoubleValue_TYPE *)result;
    if (v->type() == vInt) {
      auto _v = (IntValue_TYPE *)v;
      _r->val = val / _v->val;
    } else if (v->type() == vDouble) {
      auto _v = (DoubleValue_TYPE *)v;
      _r->val = val / _v->val;
    } else if (v->type() == vLong) {
      auto _v = (LongValue_TYPE *)v;
      _r->val = val / _v->val;
    } else if (v->type() == vFloat) {
      auto _v = (FloatValue_TYPE *)v;
      _r->val = val / _v->val;
    }
  };
  void mod(Value *v, Value *result) {
    if (v->type() == type()) {
      auto _v = (NumberValue_TYPE *)v;
      auto _r = (NumberValue_TYPE *)result;
      long _m = (long)val % (long)_v->val;
      _r->val = (T)_m;
    }
  };
  // negative: 1 => -1, or -1 => 1
  virtual void negative() { val = -val; };
  // string
  std::string to_string() {
    std::stringstream ss;
    ss << val;
    return ss.str();
  };
  // a ^ b => pow(a, b)
  virtual void pow(Value *v, Value *result) {
    auto _r = (DoubleValue_TYPE *)result;
    if (v->type() == vInt) {
      auto _v = (IntValue_TYPE *)v;
      _r->val = std::pow(val, _v->val);
    } else if (v->type() == vDouble) {
      auto _v = (DoubleValue_TYPE *)v;
      _r->val = std::pow(val, _v->val);
    } else if (v->type() == vLong) {
      auto _v = (LongValue_TYPE *)v;
      _r->val = std::pow(val, _v->val);
    } else if (v->type() == vFloat) {
      auto _v = (FloatValue_TYPE *)v;
      _r->val = std::pow(val, _v->val);
    }
  };
};

class StringValue : public Value {

public:
  const char *val;
  inline size_t size() { return strlen(val); };
  inline bool empty() { return size() == 0; };
  StringValue(const char *v) : Value(vString), val(v){};
  virtual Value *clone() { return new StringValue(val); };
  bool get_bool() { return !empty(); };
  bool equal(Value *v) {
    if (v->type() == type()) {
      auto _v = (StringValue *)v;
      return strcmp(val, _v->val) == 0;
    }
    return false;
  };
  bool in(Value *v) {
    if (v->type() == type()) {
      auto _v = (StringValue *)v;
      if (empty()) {
        return true;
      } else if (_v->empty()) {
        return false;
      } else {
        return strstr(_v->val, val) != NULL;
      }
    }
    return false;
  };
  bool gt(Value *v) {
    if (v->type() == type()) {
      auto _v = (StringValue *)v;
      return strcmp(val, _v->val) > 0;
    }
    return false;
  };
  bool gt_or_equal(Value *v) {
    if (v->type() == type()) {
      auto _v = (StringValue *)v;
      return strcmp(val, _v->val) >= 0;
    }
    return false;
  };
  bool lt(Value *v) {
    if (v->type() == type()) {
      auto _v = (StringValue *)v;
      return strcmp(val, _v->val) < 0;
    }
    return false;
  };
  bool lt_or_equal(Value *v) {
    if (v->type() == type()) {
      auto _v = (StringValue *)v;
      return strcmp(val, _v->val) <= 0;
    }
    return false;
  };
  // string
  std::string to_string() {
    std::stringstream ss;
    ss << std::string(val);
    return ss.str();
  };
};

template <typename T, ValueType value_type, typename H, ValueType H_type>
class VectorValue : public Value {
  typedef VectorValue<T, value_type, H, H_type> VectorValue_TYPE;

public:
  std::vector<T> val;
  VectorValue(std::vector<T> &v) : Value(value_type), val(v){};
  virtual Value *clone() {
    auto t = std::vector<T>();
    return new VectorValue_TYPE(val);
  };
  VectorValue(std::vector<Value *> &vals) : Value(value_type) {
    for (auto v : vals) {
      if (v->type() == H_type) {
        auto _v = (H *)v;
        val.push_back(_v->val);
      }
    }
  };
  bool get_bool() { return val.size() > 0; };
  bool equal(Value *v) {
    if (v->type() == H_type) {
      auto _v = (VectorValue_TYPE *)v;
      if (_v->val.size() != val.size()) {
        return false;
      } else {
        for (size_t i = 0; i < val.size(); ++i) {
          if (val[i] != _v->val[i])
            return false;
        }
        return true;
      }
    }
    return false;
  };
  bool in(Value *v) {
    if (v->type() == H_type) {
      auto _v = (H *)v;
      return std::find(val.begin(), val.end(), _v->val) != val.end();
    }
    return false;
  };
};

template <typename T, ValueType value_type> class MapValue : public Value {
  typedef MapValue<T, value_type> MapValue_TYPE;
  typedef std::unordered_map<std::string, T> map_type;
  map_type val;

public:
  MapValue(map_type &v) : Value(value_type), val(v){};
  virtual Value *clone() { return new MapValue_TYPE(val); };
  bool get_bool() { return val.size() > 0; };
  bool equal(Value *v) {
    if (v->type() == type()) {
      auto _v = (MapValue_TYPE *)v;
      if (_v->val.size() != val.size()) {
        return false;
      }
      for (auto it : val) {
        auto fit = _v->val.find(it.first);
        if (fit == _v->val.end()) {
          return false;
        }
        if (fit->second != it.second) {
          return false;
        }
      }
      return true;
    }
    return false;
  };
  //只允许String 为key的类型
  bool in(Value *v) {
    if (v->type() == vString) {
      auto _v = (StringValue *)v;
      return val.find(_v->val) != val.end();
    }
    return false;
  };
};

template <typename T, ValueType value_type, typename H, ValueType H_type>
class SetValue : public Value {
  typedef SetValue<T, value_type, H, H_type> SetValue_TYPE;
  typedef std::unordered_set<T> set_type;

public:
  set_type val;
  SetValue(set_type &v) : Value(value_type), val(v){};
  SetValue(std::vector<Value *> &vals) : Value(value_type) {
    for (auto &v : vals) {
      if (v->type() == H_type) {
        auto _v = (H *)v;
        val.insert(_v->val);
      }
    }
  };
  virtual Value *clone() { return new SetValue_TYPE(val); };
  bool get_bool() { return val.size() > 0; };
  bool equal(Value *v) {
    if (v->type() == H_type) {
      auto _v = (SetValue_TYPE *)v;
      if (_v->val.size() != val.size()) {
        return false;
      }
      for (auto &it : val) {
        auto fit = _v->val.find(it);
        if (fit == _v->val.end()) {
          return false;
        }
        if (*fit != it) {
          return false;
        }
      }
      return true;
    }
    return false;
  };
  bool in(Value *v) {
    if (v->type() == H_type) {
      auto _v = (H *)v;
      return val.find(_v->val) != val.end();
    }
    return false;
  };
};

template <typename T> class UserValue : public Value {
  typedef UserValue<T> UserValue_TYPE;

public:
  UserValue_TYPE value;
  UserValue(UserValue_TYPE &v) : Value(vUserType), value(v){};
};

Value *to_vec_value(std::vector<Value *> &values);
Value *to_set_value(std::vector<Value *> &values);

typedef NumberValue<int, vInt> IntValue;
typedef NumberValue<long, vLong> LongValue;
typedef NumberValue<float, vFloat> FloatValue;
typedef NumberValue<double, vDouble> DoubleValue;
typedef VectorValue<bool, vVecBool, BoolValue, vBool> BoolVecValue;
typedef VectorValue<int, vVecInt, IntValue, vInt> IntVecValue;
typedef VectorValue<long, vVecLong, LongValue, vLong> LongVecValue;
typedef VectorValue<float, vVecFloat, FloatValue, vFloat> FloatVecValue;
typedef VectorValue<double, vVecDouble, DoubleValue, vDouble> DoubleVecValue;
typedef VectorValue<std::string, vVecString, StringValue, vString>
    StringVecValue;
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
