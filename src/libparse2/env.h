#pragma once
#include "libparse2/node.h"
#include "libparse2/data_collector.h"

namespace libparse2 {

class Compile;

typedef std::function<const Value*(const Value*, const std::string&)> DataFunc;
class Env {
    friend class Compile;

public:
#define _NE_ADD(name, value_type, value_class, vec_size, value) \
    {                                                           \
        Value* nv = alloc(value_type, vec_size);                \
        if (nv) {                                               \
            ((value_class*)nv)->val = value;                    \
        } else {                                                \
            nv = new value_class(value);                        \
            new_count += 1;                                     \
        }                                                       \
        if (vec_size >= _vec_size) {                            \
            _vec_size = vec_size;                               \
        }                                                       \
        return add_var(name, nv);                               \
    }

    inline bool add(const DataCollector& dt) {
        for (const auto it : dt._datas) {
            if (it.second->vec_size() > _vec_size) {
                _vec_size = it.second->vec_size();
            }
            add_var(it.first, it.second->clone());
        }
        return true;
    }

    inline bool add(const std::string& name, bool v) {
        _NE_ADD(name, vBool, BoolValue, 0, v);
    };
    inline bool add(const std::string& name, int32_t v) {
        _NE_ADD(name, vInt, IntValue, 0, v);
    };
    inline bool add(const std::string& name, int64_t v) {
        _NE_ADD(name, vLong, LongValue, 0, v);
    };
    inline bool add(const std::string& name, float v) {
        _NE_ADD(name, vFloat, FloatValue, 0, v);
    };
    inline bool add(const std::string& name, double v) {
        _NE_ADD(name, vDouble, DoubleValue, 0, v);
    };
    inline bool add(const std::string& name, const char* str) {
        _NE_ADD(name, vString, StringValue, 0, std::string(str));
    }
    inline bool add(const std::string& name, std::string& str) {
        _NE_ADD(name, vString, StringValue, 0, str);
    };
    inline bool add(const std::string& name, std::vector<bool>& v) {
        _NE_ADD(name, vBools, BoolArrayValue, 0, v);
    };
    inline bool add(const std::string& name, std::vector<int32_t>& v) {
        _NE_ADD(name, vInts, IntArrayValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::vector<int64_t>& v) {
        _NE_ADD(name, vLongs, LongArrayValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::vector<float>& v) {
        _NE_ADD(name, vFloats, FloatArrayValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::vector<double>& v) {
        _NE_ADD(name, vDoubles, DoubleArrayValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::vector<std::string>& str) {
        _NE_ADD(name, vStrings, StringArrayValue, str.size(), str);
    };
    inline bool add(const std::string& name, std::unordered_set<bool>& v) {
        _NE_ADD(name, vSetBool, BoolSetValue, 0, v);
    };
    inline bool add(const std::string& name, std::unordered_set<int32_t>& v) {
        _NE_ADD(name, vSetInt, IntSetValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::unordered_set<int64_t>& v) {
        _NE_ADD(name, vSetLong, LongSetValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::unordered_set<float>& v) {
        _NE_ADD(name, vSetFloat, FloatSetValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::unordered_set<double>& v) {
        _NE_ADD(name, vSetDouble, DoubleSetValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::unordered_set<std::string>& str) {
        _NE_ADD(name, vSetString, StringSetValue, str.size(), str);
    };
    template <typename T>
    inline bool add(const std::string& name, T& t) {
        return add_var(name, new UserValue<T>(t, typeid(T).hash_code()));
    };

    template <typename T>
    inline bool add_data_source(T& t) {
        data_source = new UserValue<T>(t);
        return true;
    };

    const std::unordered_set<std::string>& varibles() const {
        return _head->varibles;
    };
    template <typename T>
    bool set_data_func(std::function<Value*(const T&, const std::string& name)> fn);

    bool eval();
    bool eval(std::string& t);
    bool eval(std::vector<std::string>& t);
    template <typename T>
    bool eval(T& t, typename std::enable_if_t<is_pod_type<T>::value>* dump = 0);
    template <typename T>
    bool eval(std::vector<T>& t, typename std::enable_if_t<is_pod_type<T>::value>* dump = 0);
    template <typename T>
    bool eval(T& t, typename std::enable_if_t<!is_pod_type<T>::value>* dump = 0);

    bool get(const std::string& key, std::string& t);
    bool get(const std::string& key, std::vector<std::string>& t);
    template <typename T>
    bool get(const std::string& key, T& t, typename std::enable_if_t<is_pod_type<T>::value>* dump = 0);
    template <typename T>
    bool get(const std::string& key, std::vector<T>& t, typename std::enable_if_t<is_pod_type<T>::value>* dump = 0);
    template <typename T>
    bool get(const std::string& key, T& t, typename std::enable_if_t<!is_pod_type<T>::value>* dump = 0);

    bool has_var(const std::string& key);

    // 错误信息
    const std::string error_log() const;
    bool is_valid() const;

    ~Env();
    // ToDO: add all ok check
    void clear();
    void reset();
    void disable_short_circuit() {
        _enable_short_circuit = false;
    }

protected:
    inline void add_log(const std::string& str) {
        _error_log << "\n" << str;
    };
    bool op_varible(const Node* node);
    bool eval_expression(const Node* node);
    bool _eval();

    // 为减少递归函数调用，对最底层做优化
    bool eval_leaf_expression(const Node* node);

    void push(const Value* v, MemType mm_type);
    // 删掉第二个元素，最后一个元素归位
    void pop2();
    void pop();
    const Value* get_q(size_t t, size_t i);
    const Value* get_q(size_t t);
    const Value* top();
    const Value* get_var(size_t i);
    const Value* get_var(const std::string& name);
    const Value* set_var(size_t var_index);

    Env(const NodeHeadPtr& head, const Compile* sy);

    const size_t _vars_size;
    size_t _index;
    std::vector<const Value*> _vars;
    std::vector<std::pair<const Value*, MemType>> values;
    const Compile* _sy;
    const NodeHeadPtr _head;
    bool _is_evaled;
    const Value* data_source;
    std::ostringstream _error_log;

    std::vector<Value*> _unused_vars;
    int _unused_vars_start = 0;
    size_t new_count;
    size_t _vec_size;
    bool _enable_short_circuit;
    DataFunc data_func;

    bool add_var(const std::string& name, const Value* value);
    Value* alloc_bool(const Value* v);
    Value* alloc(const Value* v);
    Value* alloc(ValueType type, const size_t vec_size);
    void destory(const Value* v);
};

}  // namespace libparse2
