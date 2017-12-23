#pragma once
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <string.h>
#include <string>
#include <unordered_map>
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
};

class Value;
typedef std::shared_ptr<Value> ValuePtr;

class Value {
public:
  Value(){};
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
  bool val;

public:
  BoolValue(bool v) : Value(), val(v){};
  ~BoolValue(){};
  bool get_bool() const { return val; };
  // a == b => a.equal(b)
  bool equal(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<BoolValue>(v);
    return val == _v->val;
  };
};

template <typename T> class NumberValue : public Value {
  typedef NumberValue<T> NumberValue_TYPE;
  T val;

public:
  NumberValue(T v) : Value(), val(v){};
  ~NumberValue(){};
  bool get_bool() { return val != 0; };
  T get_value() { return val; };
  bool equal(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
    return val == _v->val;
  };
  bool gt(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
    return val > _v->val;
  };
  bool lt(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
    return val < _v->val;
  };
  ValuePtr add(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
    return std::make_shared<NumberValue_TYPE>(val + _v->val);
  };
  ValuePtr sub(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
    return std::make_shared<NumberValue_TYPE>(val - _v->val);
  };
  ValuePtr mul(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
    return std::make_shared<NumberValue_TYPE>(val * _v->val);
  };
  ValuePtr div(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
    return std::make_shared<NumberValue_TYPE>(val / _v->val);
  };
  ValuePtr mod(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<NumberValue_TYPE>(v);
    long _m = (long)val % (long)_v->val;
    return std::make_shared<NumberValue_TYPE>((T)_m);
  };
};

class StringValue : public Value {
  std::string val;

public:
  StringValue(const std::string &v) : Value(), val(v){};
  bool get_bool() const { return !val.empty(); }
  bool equal(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<StringValue>(v);
    return val == _v->val;
  }
  bool in(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<StringValue>(v);
    if (val.empty())
      return true;
    if (_v->val.empty())
      return false;
    return _v->val.find(val) == std::string::npos;
  }
  bool gt(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<StringValue>(v);
    return val.compare(_v->val) > 0;
  }
  bool gt_or_equal(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<StringValue>(v);
    return val.compare(_v->val) >= 0;
  }
  bool lt(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<StringValue>(v);
    return val.compare(_v->val) < 0;
  }
  bool lt_or_equal(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<StringValue>(v);
    return val.compare(_v->val) <= 0;
  }
};

template <typename T> class VectorValue : public Value {
  typedef VectorValue<T> VectorValue_TYPE;
  std::vector<T> val;

public:
  VectorValue(std::vector<T> &v) : Value(), val(v){};
  bool get_bool() const { return val.size() > 0; }
  bool equal(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<VectorValue_TYPE>(v);
    if (_v.val.size() != val.size())
      return false;
    for (size_t i = 0; i < val.size(); ++i) {
      if (val[i] != _v->val[i])
        return false;
    }
    return true;
  }
  bool in(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<VectorValue_TYPE>(v);
    return std::find(_v->val.begin(), _v->val.end(), val) != _v->val.end();
  }
};

template <typename T> class MapValue : public Value {
  typedef MapValue<T> MapValue_TYPE;
  typedef std::unordered_map<std::string, T> map_type;
  map_type val;

public:
  MapValue(map_type &v) : Value(), val(v){};
  bool get_bool() const { return val.size() > 0; }
  bool equal(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<MapValue_TYPE>(v);
    if (_v.val.size() != val.size())
      return false;
    for (auto it : val) {
      auto fit = _v.find(it->first);
      if (fit == _v.end())
        return false;
      if (fit->second != it->second)
        return false;
    }
    return true;
  }
  bool in(ValuePtr &v) const {
    auto _v = std::dynamic_pointer_cast<MapValue_TYPE>(v);
    return val.find(_v) != val.end();
  }
};

template <typename T> class UserValue : public Value {
  typedef UserValue<T> UserValue_TYPE;

public:
  UserValue_TYPE value;
  UserValue(UserValue_TYPE &v) : Value(), value(v){};
};

typedef NumberValue<int> IntValue;
typedef NumberValue<long> LongValue;
typedef NumberValue<float> FloatValue;
typedef NumberValue<double> DoubleValue;
typedef VectorValue<bool> BoolVecValue;
typedef VectorValue<int> IntVecValue;
typedef VectorValue<long> LongVecValue;
typedef VectorValue<float> FloatVecValue;
typedef VectorValue<double> DoubleVecValue;
typedef VectorValue<std::string> StringVecValue;
typedef MapValue<bool> BollMapValue;
typedef MapValue<int> IntMapValue;
typedef MapValue<long> LongMapValue;
typedef MapValue<float> FloatMapValue;
typedef MapValue<double> DoubleMapValue;
typedef MapValue<std::string> StringMapValue;

} // namespace parse
