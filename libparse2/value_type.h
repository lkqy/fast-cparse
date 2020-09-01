#pragma once
#include <algorithm>
#include <functional>
#include <cmath>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string.h>
#include "enum.h"

namespace libparse2 {

#define __AND(a, b) (a && b)
#define __OR(a, b) (a || b)
#define __NOT(a) (!a)
#define __EQUAL(a, b) (a == b)
#define __GT(a, b) (a > b)
#define __GET(a, b) (a >= b)
#define __LT(a, b) (a < b)
#define __LET(a, b) (a <= b)
#define _SAFE_DIV_H(a, b) (b != 0 ? a / b : a)
#define __SAFE_DIV_H(a, b) (a != 0 ? b / a : b)
#define __ADD(a, b) (a + b)
#define __SUB(a, b) (a - b)
#define __MUL(a, b) (a * b)
#define __POW(a, b) (std::pow(a, b))
#define __S__POW(a, b) (std::pow(b, a))

#define __L_LOOP_SEQ(a, b, c, op, seq)  \
    const size_t s = a.size();       \
    for (size_t i = 0; i < s; ++i) { \
        seq(a, b, c, i, op);         \
    }

#define __OP_NN_NUM(c, a, b, op)      \
    auto _c = c.data();              \
    auto _a = a.data();              \
    auto _b = b.data();              \
    const size_t s = a.size();       \
    for (size_t i = 0; i < s; ++i) { \
        _c[i] = op(_a[i], _b[i]);    \
    }

#define _SEQ_N1(a, b, c, i, op) c[i] = op(a[i], b)
#define _SEQ_NN(a, b, c, i, op) c[i] = op(a[i], b[i])

#define __L_OP_N1(c, a, b, op) __L_LOOP_SEQ(a, b, c, op, _SEQ_N1)
#define __L_OP_NN(c, a, b, op) __L_LOOP_SEQ(a, b, c, op, _SEQ_NN)

class Value;
typedef std::shared_ptr<Value> ValuePtr;

class Value {
    ValueType _value_type;

    size_t _type_code;

public:
    Value(ValueType t, size_t type_code) : _value_type(t), _type_code(type_code){};
    ValueType type() const {
        return _value_type;
    };
    size_t type_code() const {
        return _type_code;
    };
    virtual ~Value(){};
    virtual Value* clone() const {
        // ToDo:
        return nullptr;
    };
    virtual Value* get_bool() const {
        // ToDo:
        return nullptr;
    };
    // a == b => a.equal(b)
    virtual void _and(const Value* v, Value* result) const {};
    virtual void _or(const Value* v, Value* result) const {};
    virtual void _not(const Value* result) const {};

    virtual void equal(const Value* v, Value* result) const {};
    // a in b => b.in(a), 右结合
    virtual void in(const Value* v, Value* result) const {};
    virtual void vec_in(const Value* v, Value* result) const {};
    // a > b => a.gt(b)
    virtual void gt(const Value* v, Value* result) const {};
    // a >= b => a.gt_or_equal(b)
    virtual void gt_or_equal(const Value* v, Value* result) const {};
    // a < b => a.lt(b)
    virtual void lt(const Value* v, Value* result) const {};
    // a <= b => a.lt_or_equal(b)
    virtual void lt_or_equal(const Value* v, Value* result) const {};
    // a + b => a.add(b)
    virtual void add(const Value* v, Value* result) const {};
    // a - b => a.sub(b)
    virtual void sub(const Value* v, Value* result) const {};
    // a * b => a.mul(b)
    virtual void mul(const Value* v, Value* result) const {};
    // a / b => a.div(b)
    virtual void div(const Value* v, Value* result) const {};
    // a % b => a.mod(b)
    virtual void mod(const Value* v, Value* result) const {};
    // negative: 1 => -1, or -1 => 1
    virtual void negative(){};
    // a / b => a.div(b)
    virtual void s_div(const Value* v, Value* result) const {};
    // string
    virtual std::string to_string() const {
        return "";
    };
    // a ^ b => pow(a, b)
    virtual void pow(const Value* v, Value* result) const {};
    // a ^ b => pow(b, a)
    virtual void s_pow(const Value* v, Value* result) const {};

    virtual bool is_vec() const {
        return false;
    };
    virtual size_t vec_size() const {
        return 0;
    };
    // 支持提前返回
    virtual bool can_break(OperateType o_type) const {
        return false;
    };
    virtual Value* break_value(OperateType o_type) const {
        return nullptr;
    };
};

class BoolValue : public Value {
public:
    bool val;
    BoolValue(bool v) : Value(vBool, typeid(bool).hash_code()), val(v){};
    ~BoolValue(){};
    virtual Value* clone() const override {
        return new BoolValue(val);
    };
    Value* get_bool() const override {
        return new BoolValue(val);
    };

#define __BOOL_OP(v, result, op)          \
    BoolValue* _r = (BoolValue*)result;  \
    Value* _v = v->get_bool();           \
    if (_v->type() == type()) {          \
        BoolValue* __v = (BoolValue*)_v; \
        _r->val = op(val, __v->val);     \
    } else {                             \
        _r->val = false;                 \
    }                                    \
    delete _v;

    void _and(const Value* v, Value* result) const override {
        __BOOL_OP(v, result, __AND);
    };
    void _or(const Value* v, Value* result) const override {
        __BOOL_OP(v, result, __OR);
    };
    void _not(const Value* result) const override {
        BoolValue* _r = (BoolValue*)result;
        _r->val = !val;
    };
    // a == b => a.equal(b)
    void equal(const Value* v, Value* result) const override {
        __BOOL_OP(v, result, __EQUAL);
    };

    void negative() {
        val = not val;
    }
    // 支持提前返回
    virtual bool can_break(OperateType o_type) const override {
        if ((o_type == kAND && val == false) || (o_type == kOR && val == true))
            return true;
        return false;
    };
    virtual Value* break_value(OperateType o_type) const override {
        if (o_type == kAND && val == false)
            return new BoolValue(false);
        if (o_type == kOR && val == true)
            return new BoolValue(false);
        return nullptr;
    };
    // string
    std::string to_string() const override {
        std::stringstream ss;
        ss << val;
        return ss.str();
    };
};

class BoolArrayValue : public Value {
public:
    std::vector<bool> val;
    BoolArrayValue(const std::vector<bool>& v) : Value(vBools, typeid(std::vector<bool>).hash_code()), val(v){};
    ~BoolArrayValue(){};
    virtual Value* clone() const override {
        return new BoolArrayValue(val);
    };
    Value* get_bool() const override {
        return new BoolArrayValue(val);
    };

#define __L_BOOLS_COMP(v, result, op)                 \
    BoolArrayValue* _r = (BoolArrayValue*)result;  \
    Value* _v = v->get_bool();                     \
    if (_v->type() == type()) {                    \
        BoolArrayValue* __v = (BoolArrayValue*)_v; \
        __L_OP_NN(_r->val, val, __v->val, op);        \
    } else if (_v->type() == vBool) {              \
        BoolValue* __v = (BoolValue*)_v;           \
        __L_OP_N1(_r->val, val, __v->val, op);        \
    }                                              \
    delete _v;

    void _and(const Value* v, Value* result) const override {
        __L_BOOLS_COMP(v, result, __AND);
    };
    void _or(const Value* v, Value* result) const override {
        __L_BOOLS_COMP(v, result, __OR);
    };
    void _not(const Value* result) const override {
        BoolArrayValue* _r = (BoolArrayValue*)result;
        const size_t s = val.size();
        for (size_t i = 0; i < s; ++i) {
            _r->val[i] = !val[i];
        }
    };

    void negative() {
        const size_t size = val.size();
        for (size_t i = 0; i < size; ++i) {
            val[i] = not val[i];
        }
    };

    // a == b => a.equal(b)
    void equal(const Value* v, Value* result) const override {
        __L_BOOLS_COMP(v, result, __EQUAL);
    };
    virtual bool is_vec() const override {
        return true;
    };
    virtual size_t vec_size() const override {
        return val.size();
    };
};

template <typename T, ValueType value_type>
class NumberValue : public Value {
    typedef NumberValue<T, value_type> NumberValue_TYPE;
    typedef NumberValue<int, vInt> IntValue_TYPE;
    typedef NumberValue<long, vLong> LongValue_TYPE;
    typedef NumberValue<float, vFloat> FloatValue_TYPE;
    typedef NumberValue<double, vDouble> DoubleValue_TYPE;

public:
    T val;
    NumberValue(T v) : Value(value_type, typeid(T).hash_code()), val(v){};
    ~NumberValue(){};
    virtual Value* clone() const override {
        return new NumberValue_TYPE(val);
    };
    Value* get_bool() const override {
        return new BoolValue(val != 0);
    };

#define __NUMBER_ARITH(l, r, result, op)                  \
    NumberValue_TYPE* _r = (NumberValue_TYPE*)result;    \
    if (r->type() == type()) {                           \
        NumberValue_TYPE* _v = (NumberValue_TYPE*)r;     \
        _r->val = op(l->val, _v->val);                   \
    } else {                                             \
        if (r->type() == vInt) {                         \
            IntValue_TYPE* _v = (IntValue_TYPE*)r;       \
            _r->val = op(l->val, _v->val);               \
        } else if (r->type() == vDouble) {               \
            DoubleValue_TYPE* _v = (DoubleValue_TYPE*)r; \
            _r->val = op(l->val, _v->val);               \
        } else if (r->type() == vLong) {                 \
            LongValue_TYPE* _v = (LongValue_TYPE*)r;     \
            _r->val = op(l->val, _v->val);               \
        } else if (r->type() == vFloat) {                \
            FloatValue_TYPE* _v = (FloatValue_TYPE*)r;   \
            _r->val = op(l->val, _v->val);               \
        }                                                \
    }

#define __NUMBER_COMP(v, result, op)                         \
    BoolValue* _r = (BoolValue*)result;                     \
    if (v->type() == type()) {                              \
        NumberValue_TYPE* _v = (NumberValue_TYPE*)v;        \
        _r->val = op(val, _v->val);                         \
    } else {                                                \
        if (v->type() == vInt) {                            \
            _r->val = op(val, ((IntValue_TYPE*)v)->val);    \
        } else if (v->type() == vDouble) {                  \
            _r->val = op(val, ((DoubleValue_TYPE*)v)->val); \
        } else if (v->type() == vLong) {                    \
            _r->val = op(val, ((LongValue_TYPE*)v)->val);   \
        } else if (v->type() == vFloat) {                   \
            _r->val = op(val, ((FloatValue_TYPE*)v)->val);  \
        }                                                   \
    }

    void equal(const Value* v, Value* result) const override {
        __NUMBER_COMP(v, result, __EQUAL);
    };
    void gt(const Value* v, Value* result) const override {
        __NUMBER_COMP(v, result, __GT);
    };
    void gt_or_equal(const Value* v, Value* result) const override {
        __NUMBER_COMP(v, result, __GET);
    };
    void lt(const Value* v, Value* result) const override {
        __NUMBER_COMP(v, result, __LT);
    };
    void lt_or_equal(const Value* v, Value* result) const override {
        __NUMBER_COMP(v, result, __LET);
    };
    void add(const Value* v, Value* result) const override {
        __NUMBER_ARITH(this, v, result, __ADD);
    };
    void sub(const Value* v, Value* result) const override {
        __NUMBER_ARITH(this, v, result, __SUB);
    };
    void mul(const Value* v, Value* result) const override {
        __NUMBER_ARITH(this, v, result, __MUL);
    };
    void div(const Value* v, Value* result) const override {
        __NUMBER_ARITH(this, v, result, _SAFE_DIV_H);
    };
    void s_div(const Value* v, Value* result) const override {
        __NUMBER_ARITH(this, v, result, __SAFE_DIV_H);
    };
    void mod(const Value* v, Value* result) const override {
        if (v->type() == type()) {
            NumberValue_TYPE* _v = (NumberValue_TYPE*)v;
            NumberValue_TYPE* _r = (NumberValue_TYPE*)result;
            long _m = (long)val % (long)_v->val;
            _r->val = (T)_m;
        }
    };

#define __L_VALUE_LOGIC(v, result, op)     \
    Value* _v = v->get_bool();          \
    BoolValue* _r = (BoolValue*)result; \
    BoolValue* _vs = (BoolValue*)_v;    \
    _r->val = op(val, _vs->val);        \
    delete _v;

    void _and(const Value* v, Value* result) const override {
        __L_VALUE_LOGIC(v, result, __AND);
    };
    void _or(const Value* v, Value* result) const override {
        __L_VALUE_LOGIC(v, result, __OR);
    };
    void _not(const Value* result) const override {
        BoolValue* _r = (BoolValue*)result;
        _r->val = !_r->val;
    };
    // negative: 1 => -1, or -1 => 1
    virtual void negative() {
        val = -val;
    };
    // string
    std::string to_string() const override {
        std::stringstream ss;
        ss << val;
        return ss.str();
    };
    // a ^ b => pow(a, b)
    virtual void pow(const Value* v, Value* result) const override {
        __NUMBER_ARITH(this, v, result, __POW);
    };

    // a ^ b => pow(b, a)
    virtual void s_pow(const Value* v, Value* result) const override {
        __NUMBER_ARITH(this, v, result, __S__POW);
    };
    // 支持提前返回
    virtual bool can_break(OperateType o_type) const override {
        if (o_type == kMUL && val == 0)
            return true;
        return false;
    };
    virtual Value* break_value(OperateType o_type) const override {
        if (o_type == kMUL && val == 0)
            return new NumberValue_TYPE(0);
        return nullptr;
    };
};

template <typename T, ValueType value_type>
class NumberArrayValue : public Value {
    typedef NumberArrayValue<T, value_type> NumberArrayValue_TYPE;
    typedef NumberArrayValue<int, vInt> IntArrayValue_TYPE;
    typedef NumberArrayValue<long, vLong> LongArrayValue_TYPE;
    typedef NumberArrayValue<float, vFloat> FloatArrayValue_TYPE;
    typedef NumberArrayValue<double, vDouble> DoubleArrayValue_TYPE;
    typedef NumberValue<int, vInt> IntValue_TYPE;
    typedef NumberValue<long, vLong> LongValue_TYPE;
    typedef NumberValue<float, vFloat> FloatValue_TYPE;
    typedef NumberValue<double, vDouble> DoubleValue_TYPE;

public:
    std::vector<T> val;
    NumberArrayValue(const std::vector<T>& v) : Value(value_type, typeid(std::vector<T>).hash_code()), val(v){};
    NumberArrayValue(std::vector<T>&& v) : Value(value_type, typeid(std::vector<T>).hash_code()), val(std::move(v)){};
    ~NumberArrayValue(){};
    Value* clone() const override {
        auto r = new NumberArrayValue_TYPE(val);
        return r;
    };
    Value* get_bool() const override {
        std::vector<bool> _vals(val.size());
        const size_t s = val.size();
        for (size_t i = 0; i < s; ++i) {
            _vals[i] = val[i];
        }
        return new BoolArrayValue(_vals);
    };

#define __L_ARRAY_COMP(v, result, op)                                 \
    BoolArrayValue* _r = (BoolArrayValue*)result;                  \
    if (v->type() == type()) {                                     \
        NumberArrayValue_TYPE* _v = (NumberArrayValue_TYPE*)v;     \
        __L_OP_NN(_r->val, val, _v->val, op);                         \
    } else {                                                       \
        if (v->type() == vInt) {                                   \
            IntValue_TYPE* _v = (IntValue_TYPE*)v;                 \
            __L_OP_N1(_r->val, val, _v->val, op);                     \
        } else if (v->type() == vDouble) {                         \
            DoubleValue_TYPE* _v = (DoubleValue_TYPE*)v;           \
            __L_OP_N1(_r->val, val, _v->val, op);                     \
        } else if (v->type() == vLong) {                           \
            LongValue_TYPE* _v = (LongValue_TYPE*)v;               \
            __L_OP_N1(_r->val, val, _v->val, op);                     \
        } else if (v->type() == vFloat) {                          \
            FloatValue_TYPE* _v = (FloatValue_TYPE*)v;             \
            __L_OP_N1(_r->val, val, _v->val, op);                     \
        } else if (v->type() == vInts) {                           \
            IntArrayValue_TYPE* _v = (IntArrayValue_TYPE*)v;       \
            __L_OP_NN(_r->val, val, _v->val, op);                     \
        } else if (v->type() == vDoubles) {                        \
            DoubleArrayValue_TYPE* _v = (DoubleArrayValue_TYPE*)v; \
            __L_OP_NN(_r->val, val, _v->val, op);                     \
        } else if (v->type() == vLongs) {                          \
            LongArrayValue_TYPE* _v = (LongArrayValue_TYPE*)v;     \
            __L_OP_NN(_r->val, val, _v->val, op);                     \
        } else if (v->type() == vFloats) {                         \
            FloatArrayValue_TYPE* _v = (FloatArrayValue_TYPE*)v;   \
            __L_OP_NN(_r->val, val, _v->val, op);                     \
        }                                                          \
    }

#define __L_ARRAY_ARITH(l, r, result, op)                                  \
    if (r->type() == type()) {                                          \
        NumberArrayValue_TYPE* _v = (NumberArrayValue_TYPE*)r;          \
        NumberArrayValue_TYPE* _r = (NumberArrayValue_TYPE*)result;     \
        __OP_NN_NUM(_r->val, l->val, _v->val, op);                       \
    } else {                                                            \
        if (r->type() == vInt) {                                        \
            IntValue_TYPE* _v = (IntValue_TYPE*)r;                      \
            NumberArrayValue_TYPE* _r = (NumberArrayValue_TYPE*)result; \
            __L_OP_N1(_r->val, l->val, _v->val, op);                       \
        } else if (r->type() == vDouble) {                              \
            DoubleValue_TYPE* _v = (DoubleValue_TYPE*)r;                \
            NumberArrayValue_TYPE* _r = (NumberArrayValue_TYPE*)result; \
            __L_OP_N1(_r->val, l->val, _v->val, op);                       \
        } else if (r->type() == vLong) {                                \
            LongValue_TYPE* _v = (LongValue_TYPE*)r;                    \
            NumberArrayValue_TYPE* _r = (NumberArrayValue_TYPE*)result; \
            __L_OP_N1(_r->val, l->val, _v->val, op);                       \
        } else if (r->type() == vFloat) {                               \
            FloatValue_TYPE* _v = (FloatValue_TYPE*)r;                  \
            NumberArrayValue_TYPE* _r = (NumberArrayValue_TYPE*)result; \
            __L_OP_N1(_r->val, l->val, _v->val, op);                       \
        } else if (r->type() == vInts) {                                \
            IntArrayValue_TYPE* _v = (IntArrayValue_TYPE*)r;            \
            NumberArrayValue_TYPE* _r = (NumberArrayValue_TYPE*)result; \
            __L_OP_NN(_r->val, l->val, _v->val, op);                       \
        } else if (r->type() == vDoubles) {                             \
            DoubleArrayValue_TYPE* _v = (DoubleArrayValue_TYPE*)r;      \
            NumberArrayValue_TYPE* _r = (NumberArrayValue_TYPE*)result; \
            __L_OP_NN(_r->val, l->val, _v->val, op);                       \
        } else if (r->type() == vLongs) {                               \
            LongArrayValue_TYPE* _v = (LongArrayValue_TYPE*)r;          \
            NumberArrayValue_TYPE* _r = (NumberArrayValue_TYPE*)result; \
            __L_OP_NN(_r->val, l->val, _v->val, op);                       \
        } else if (r->type() == vFloats) {                              \
            FloatArrayValue_TYPE* _v = (FloatArrayValue_TYPE*)r;        \
            NumberArrayValue_TYPE* _r = (NumberArrayValue_TYPE*)result; \
            __L_OP_NN(_r->val, l->val, _v->val, op);                       \
        }                                                               \
    }

    void equal(const Value* v, Value* result) const override {
        __L_ARRAY_COMP(v, result, __EQUAL);
    };
    void gt(const Value* v, Value* result) const override {
        __L_ARRAY_COMP(v, result, __GT);
    };
    void gt_or_equal(const Value* v, Value* result) const override {
        __L_ARRAY_COMP(v, result, __GET);
    };
    void lt(const Value* v, Value* result) const override {
        __L_ARRAY_COMP(v, result, __LT);
    };
    void lt_or_equal(const Value* v, Value* result) const override {
        __L_ARRAY_COMP(v, result, __LET);
    };
    void add(const Value* v, Value* result) const override {
        __L_ARRAY_ARITH(this, v, result, __ADD);
    };
    void sub(const Value* v, Value* result) const override {
        __L_ARRAY_ARITH(this, v, result, __SUB);
    };
    void mul(const Value* v, Value* result) const override {
        __L_ARRAY_ARITH(this, v, result, __MUL);
    };
    void div(const Value* v, Value* result) const override {
        __L_ARRAY_ARITH(this, v, result, _SAFE_DIV_H);
    };
    void s_div(const Value* v, Value* result) const override {
        __L_ARRAY_ARITH(this, v, result, __SAFE_DIV_H);
    };
    void mod(const Value* v, Value* result) const override{
            // ToDo:
    };
    // negative: 1 => -1, or -1 => 1
    virtual void negative() {
        const size_t s = val.size();
        for (size_t i = 0; i < s; ++i) {
            val[i] = -val[i];
        }
    };

#define __L_ARRAY_LOGIC(v, result, op)                \
    Value* _v = v->get_bool();                     \
    BoolArrayValue* _r = (BoolArrayValue*)result;  \
    if (_v->is_vec()) {                            \
        BoolArrayValue* _vs = (BoolArrayValue*)_v; \
        __L_OP_NN(_r->val, val, _vs->val, op);        \
    } else {                                       \
        BoolValue* _vs = (BoolValue*)_v;           \
        __L_OP_N1(_r->val, val, _vs->val, op);        \
    }                                              \
    delete _v;

    void _and(const Value* v, Value* result) const override {
        __L_ARRAY_LOGIC(v, result, __AND);
    };
    void _or(const Value* v, Value* result) const override {
        __L_ARRAY_LOGIC(v, result, __OR);
    };
    void _not(const Value* result) const override {
        BoolArrayValue* _r = (BoolArrayValue*)result;
        const size_t s = val.size();
        for (size_t i = 0; i < s; ++i) {
            _r->val[i] = !val[i];
        }
    };

    // string
    std::string to_string() const override {
        std::stringstream ss;
        // ToDo: to add
        return ss.str();
    };
    // a ^ b => pow(a, b)
    void pow(const Value* v, Value* result) const override {
        __L_ARRAY_ARITH(this, v, result, __POW);
    };
    void s_pow(const Value* v, Value* result) const override {
        __L_ARRAY_ARITH(this, v, result, __S__POW);
    };
    bool is_vec() const override {
        return true;
    };
    size_t vec_size() const override {
        return val.size();
    };
};

class StringValue : public Value {
public:
    std::string val;
    inline size_t size() const {
        return val.size();
    };
    inline bool empty() const {
        return size() == 0;
    };
    StringValue(const std::string& v) : Value(vString, typeid(std::string).hash_code()), val(v){};
    virtual Value* clone() const override {
        return new StringValue(val);
    };
    Value* get_bool() const override {
        return new BoolValue(!empty());
    };
#define _L_STR_COMP(v, result, op)           \
    bool r = false;                        \
    if (v->type() == type()) {             \
        StringValue* _v = (StringValue*)v; \
        r = op(val.compare(_v->val), 0);   \
    }                                      \
    BoolValue* _r = (BoolValue*)result;    \
    _r->val = r;

    void equal(const Value* v, Value* result) const override {
        _L_STR_COMP(v, result, __EQUAL);
    };
    void gt(const Value* v, Value* result) const override {
        _L_STR_COMP(v, result, __GT);
    };
    void gt_or_equal(const Value* v, Value* result) const override {
        _L_STR_COMP(v, result, __GET);
    };
    void lt(const Value* v, Value* result) const override {
        _L_STR_COMP(v, result, __LT);
    };
    void lt_or_equal(const Value* v, Value* result) const override {
        _L_STR_COMP(v, result, __LET);
    };
    void in(const Value* v, Value* result) const override {
        bool r = false;
        if (v->type() == type()) {
            StringValue* _v = (StringValue*)v;
            if (_v->empty()) {
                r = true;
            } else if (empty()) {
                r = false;
            } else {
                r = val.find(_v->val) != std::string::npos;
            }
        }
        BoolValue* _r = (BoolValue*)result;
        _r->val = r;
    };
    void add(const Value* v, Value* result) const override {
        if (v->type() == type()) {
            StringValue* _v = (StringValue*)v;
            StringValue* _r = (StringValue*)result;
            _r->val = val + _v->val;
        }
    };
    // string
    std::string to_string() const override {
        return val;
    };
};

class StringArrayValue : public Value {
public:
    std::vector<std::string> val;
    inline size_t size() const {
        return val.size();
    };
    inline bool empty() const {
        return size() == 0;
    };
    StringArrayValue(const std::vector<std::string>& v)
            : Value(vStrings, typeid(std::vector<std::string>).hash_code()), val(v){};
    virtual Value* clone() const override {
        return new StringArrayValue(val);
    };
    Value* get_bool() const override {
        std::vector<bool> _vals(val.size());
        const size_t s = val.size();
        for (size_t i = 0; i < s; ++i) {
            _vals[i] = val[i].length();
        }
        return new BoolArrayValue(_vals);
    };
#define _L_STRS_COMP(v, result, op)                     \
    std::vector<bool> r(val.size(), false);           \
    if (v->type() == type()) {                        \
        StringArrayValue* _v = (StringArrayValue*)v;  \
        for (size_t i = 0; i < val.size(); ++i) {     \
            r[i] = op(val[i].compare(_v->val[i]), 0); \
        }                                             \
    } else if (v->type() == vString) {                \
        StringValue* _v = (StringValue*)v;            \
        for (size_t i = 0; i < val.size(); ++i) {     \
            r[i] = op(val[i].compare(_v->val), 0);    \
        }                                             \
    }                                                 \
    BoolArrayValue* _r = (BoolArrayValue*)result;     \
    _r->val = r;

    void equal(const Value* v, Value* result) const override {
        _L_STRS_COMP(v, result, __EQUAL);
    };
    void gt(const Value* v, Value* result) const override {
        _L_STRS_COMP(v, result, __GT);
    };
    void gt_or_equal(const Value* v, Value* result) const override {
        _L_STRS_COMP(v, result, __GET);
    };
    void lt(const Value* v, Value* result) const override {
        _L_STRS_COMP(v, result, __LT);
    };
    void lt_or_equal(const Value* v, Value* result) const override {
        _L_STRS_COMP(v, result, __LET);
    };
    void in(const Value* v, Value* result) const override {
        // StringArray 是向量操作
        const size_t size = val.size();
        std::vector<bool> r(size, false);
        if (v->type() == type()) {
            StringArrayValue* _v = (StringArrayValue*)v;
            for (size_t i = 0; i < size; ++i) {
                if (val[i].empty()) {
                    r[i] = true;
                } else if (_v->val[i].empty()) {
                    r[i] = false;
                } else {
                    r[i] = val[i].find(_v->val[i]) != std::string::npos;
                }
            }
        } else if (v->type() == vString) {
            StringValue* _v = (StringValue*)v;
            for (size_t i = 0; i < size; ++i) {
                if (val[i].empty()) {
                    r[i] = true;
                } else if (_v->val.empty()) {
                    r[i] = false;
                } else {
                    r[i] = val[i].find(_v->val) != std::string::npos;
                }
            }
        }
        BoolArrayValue* _r = (BoolArrayValue*)result;
        _r->val = r;
    };
    virtual bool is_vec() const override {
        return true;
    };
    virtual size_t vec_size() const override {
        return val.size();
    };
    // string
    std::string to_string() const override {
        std::stringstream ss;
        return ss.str();
    };
};

template <typename T, ValueType value_type, typename H, ValueType H_type, typename M, ValueType M_type, typename VH,
          ValueType VH_type, typename VM, ValueType VM_type>
class SetValue : public Value {
    typedef SetValue<T, value_type, H, H_type, M, M_type, VH, VH_type, VM, VM_type> SetValue_TYPE;
    typedef std::unordered_set<T> set_type;

public:
    set_type val;
    SetValue(const set_type& v) : Value(value_type, typeid(set_type).hash_code()), val(v){};
    Value* clone() const override {
        return new SetValue_TYPE(val);
    };
    Value* get_bool() const override {
        return new BoolValue(val.size() > 0);
    };
    void equal(const Value* v, Value* result) const override {
        bool r = false;
        if (v->type() == H_type) {
            SetValue_TYPE* _v = (SetValue_TYPE*)v;
            if (_v->val.size() == val.size()) {
                bool _r = true;
                for (auto& it : val) {
                    auto fit = _v->val.find(it);
                    if (fit == _v->val.end() || *fit != it) {
                        _r = false;
                        break;
                    }
                }
                r = _r;
            }
        }
        BoolValue* _r = (BoolValue*)result;
        _r->val = r;
    };
    void in(const Value* v, Value* result) const override {
        bool r = false;
        if (v->type() == H_type) {
            auto _v = (H*)v;
            r = val.find(_v->val) != val.end();
        } else if (v->type() == M_type) {
            auto _v = (M*)v;
            r = val.find(_v->val) != val.end();
        }
        BoolValue* _r = (BoolValue*)result;
        _r->val = r;
    };
    void vec_in(const Value* v, Value* result) const override {
        BoolArrayValue* _r = (BoolArrayValue*)result;
        if (v->type() == VH_type) {
            VH* _v = (VH*)v;
            const auto s = _v->val.size();
            for (size_t i = 0; i < s; ++i) {
                _r->val[i] = val.find(_v->val[i]) != val.end();
            }
        } else if (v->type() == VM_type) {
            VM* _v = (VM*)v;
            const auto s = _v->val.size();
            for (size_t i = 0; i < s; ++i) {
                _r->val[i] = val.find(_v->val[i]) != val.end();
            }
        }
    };
};

template <typename T>
class UserValue : public Value {
public:
    T val;
    UserValue(T& v, size_t type_code = typeid(T).hash_code()) : Value(vUserType, type_code), val(v){};
};

/*
template <typename T>
class UserArrayValue : public Value {
    typedef UserArrayValue<T> UserValue_TYPE;

public:
    std::vector<UserValue_TYPE> val;
    UserArrayValue(std::vector<UserValue_TYPE>& v)
            : Value(vUserTypes, typeid(std::vector<UserValue_TYPE>).hash_code()), val(v){};
};
*/

Value* merge_array(const std::vector<bool>& bools, const Value* left, const Value* right);
Value* to_set_value(std::vector<Value*>& values);
Value* to_array_value(std::vector<Value*>& values);
const Value* to_array(const Value* value, size_t size);

typedef NumberValue<int32_t, vInt> IntValue;
typedef NumberValue<int64_t, vLong> LongValue;
typedef NumberValue<float, vFloat> FloatValue;
typedef NumberValue<double, vDouble> DoubleValue;

typedef NumberArrayValue<int32_t, vInts> IntArrayValue;
typedef NumberArrayValue<int64_t, vLongs> LongArrayValue;
typedef NumberArrayValue<float, vFloats> FloatArrayValue;
typedef NumberArrayValue<double, vDoubles> DoubleArrayValue;

typedef SetValue<bool, vSetBool, BoolValue, vBool, BoolValue, vBool, BoolArrayValue, vBools, BoolArrayValue, vBools>
        BoolSetValue;
typedef SetValue<int32_t, vSetInt, IntValue, vInt, LongValue, vLong, IntArrayValue, vInts, LongArrayValue, vLongs>
        IntSetValue;
typedef SetValue<int64_t, vSetLong, IntValue, vInt, LongValue, vLong, LongArrayValue, vLongs, IntArrayValue, vInts>
        LongSetValue;
typedef SetValue<float, vSetFloat, FloatValue, vFloat, DoubleValue, vDouble, FloatArrayValue, vFloats, DoubleArrayValue,
                 vDoubles>
        FloatSetValue;
typedef SetValue<double, vSetDouble, FloatValue, vFloat, DoubleValue, vDouble, DoubleArrayValue, vDoubles,
                 FloatArrayValue, vFloats>
        DoubleSetValue;
typedef SetValue<std::string, vSetString, StringValue, vString, StringValue, vString, StringArrayValue, vStrings,
                 StringArrayValue, vStrings>
        StringSetValue;

template <class T>
struct is_pod_type
        : std::integral_constant<
                  bool,
                  std::is_same<T, bool>::value || std::is_same<T, int32_t>::value || std::is_same<T, int64_t>::value ||
                          std::is_same<T, float>::value || std::is_same<T, double>::value ||
                          std::is_same<T, std::string>::value || std::is_same<T, std::vector<bool>>::value ||
                          std::is_same<T, std::vector<int32_t>>::value ||
                          std::is_same<T, std::vector<int64_t>>::value || std::is_same<T, std::vector<float>>::value ||
                          std::is_same<T, std::vector<double>>::value ||
                          std::is_same<T, std::vector<std::string>>::value

                  > {};

template <class T>
std::enable_if_t<std::is_same<T, bool>::value, T*> get_value(const Value* f) {
    return &(((BoolValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, int32_t>::value, T*> get_value(const Value* f) {
    return &(((IntValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, int64_t>::value, T*> get_value(const Value* f) {
    return &(((LongValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, float>::value, T*> get_value(const Value* f) {
    return &(((FloatValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, double>::value, T*> get_value(const Value* f) {
    return &(((DoubleValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, std::string>::value, T*> get_value(const Value* f) {
    return &(((StringValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, std::vector<bool>>::value, T*> get_value(const Value* f) {
    return &(((BoolArrayValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, std::vector<int32_t>>::value, T*> get_value(const Value* f) {
    return &(((IntArrayValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, std::vector<int64_t>>::value, T*> get_value(const Value* f) {
    return &(((LongArrayValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, std::vector<float>>::value, T*> get_value(const Value* f) {
    return &(((FloatArrayValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, std::vector<double>>::value, T*> get_value(const Value* f) {
    return &(((DoubleArrayValue*)f)->val);
}

template <class T>
std::enable_if_t<std::is_same<T, std::vector<std::string>>::value, T*> get_value(const Value* f) {
    return &(((StringArrayValue*)f)->val);
}

Value* to_value(bool f);
Value* to_value(int32_t f);
Value* to_value(int64_t f);
Value* to_value(float f);
Value* to_value(double f);
Value* to_value(const char* f);
Value* to_value(std::string& f);
Value* to_value(std::vector<bool>& f);
Value* to_value(std::vector<int32_t>& f);
Value* to_value(std::vector<int64_t>& f);
Value* to_value(std::vector<float>& f);
Value* to_value(std::vector<double>& f);
Value* to_value(std::vector<const char*>& f);
Value* to_value(std::vector<std::string>& f);
Value* to_value(Value* f);

template <class T>
Value* to_value(T& f) {
    return new UserValue<T>(f);
}

inline uint32_t time33(const char* str, size_t key_length) {
    uint32_t hash = 5381;
    while (key_length--) {
        hash += (hash << 5) + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}

inline uint32_t hash_str(const std::string& name) {
    return time33(name.c_str(), name.length());
}
}  // namespace libparse2
