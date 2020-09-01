#pragma once
#include "libparse2/node.h"

namespace libparse2 {
// 提供一个可以跨Env共享变量的机制
class DataCollector {
#define _DNE_ADD(name, value_type, value_class, vec_size, value) \
    {                                                            \
        Value* nv = new value_class(value);                      \
        auto it = _datas.find(name);                             \
        if (it != _datas.end()) {                                \
            _datas[name] = nv;                                   \
            return false;                                        \
        } else {                                                 \
            _datas[name] = nv;                                   \
            return true;                                         \
        }                                                        \
    }

public:
    ~DataCollector() {
        for (auto it : _datas) {
            delete it.second;
        }
    }
    inline bool add(const std::string& name, bool v) {
        _DNE_ADD(name, vBool, BoolValue, 0, v);
    };
    inline bool add(const std::string& name, int32_t v) {
        _DNE_ADD(name, vInt, IntValue, 0, v);
    };
    inline bool add(const std::string& name, int64_t v) {
        _DNE_ADD(name, vLong, LongValue, 0, v);
    };
    inline bool add(const std::string& name, float v) {
        _DNE_ADD(name, vFloat, FloatValue, 0, v);
    };
    inline bool add(const std::string& name, double v) {
        _DNE_ADD(name, vDouble, DoubleValue, 0, v);
    };
    inline bool add(const std::string& name, const char* str) {
        _DNE_ADD(name, vString, StringValue, 0, std::string(str));
    }
    inline bool add(const std::string& name, const std::string& str) {
        _DNE_ADD(name, vString, StringValue, 0, str);
    };
    inline bool add(const std::string& name, std::vector<bool>& v) {
        _DNE_ADD(name, vBools, BoolArrayValue, 0, v);
    };
    inline bool add(const std::string& name, std::vector<int32_t>& v) {
        _DNE_ADD(name, vInts, IntArrayValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::vector<int64_t>& v) {
        _DNE_ADD(name, vLongs, LongArrayValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::vector<float>& v) {
        _DNE_ADD(name, vFloats, FloatArrayValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::vector<double>& v) {
        _DNE_ADD(name, vDoubles, DoubleArrayValue, v.size(), v);
    };
    inline bool add(const std::string& name, const std::vector<std::string>& str) {
        _DNE_ADD(name, vStrings, StringArrayValue, str.size(), str);
    };
    inline bool add(const std::string& name, std::unordered_set<bool>& v) {
        _DNE_ADD(name, vSetBool, BoolSetValue, 0, v);
    };
    inline bool add(const std::string& name, std::unordered_set<int32_t>& v) {
        _DNE_ADD(name, vSetInt, IntSetValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::unordered_set<int64_t>& v) {
        _DNE_ADD(name, vSetLong, LongSetValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::unordered_set<float>& v) {
        _DNE_ADD(name, vSetFloat, FloatSetValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::unordered_set<double>& v) {
        _DNE_ADD(name, vSetDouble, DoubleSetValue, v.size(), v);
    };
    inline bool add(const std::string& name, std::unordered_set<std::string>& str) {
        _DNE_ADD(name, vSetString, StringSetValue, str.size(), str);
    };
    template <typename T>
    inline bool add(const std::string& name, T& t) {
        return add_var(name, new UserValue<T>(t, typeid(T).hash_code()));
    };

private:
    std::unordered_map<std::string, Value*> _datas;
    friend class Env;
};

}  // namespace libparse2
