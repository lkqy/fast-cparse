#pragma once
#include <functional>
#include <iostream>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <string.h>
#include <time.h>
#include "libparse2/env.h"
#include "libparse2/env_impl.h"

namespace libparse2 {

typedef std::function<const Value*(const Value*)> BindFunc1;
typedef std::function<const Value*(const Value*, const Value*)> BindFunc2;
typedef std::function<const Value*(const Value*, const Value*, const Value*)> BindFunc3;
typedef std::function<const Value*(const Value*, const Value*, const Value*, const Value*)> BindFunc4;
typedef std::function<const Value*(const Value*, const Value*, const Value*, const Value*, const Value*)> BindFunc5;

class Env;
typedef std::shared_ptr<Env> NewEnvPtr;
typedef Env NewEnv;

class Compile {
    friend class Env;

public:
    Compile(){};
    NewEnvPtr get(const std::string expression) {
        std::lock_guard<std::mutex> guard(mutex);
        return get_instance(expression);
    };
    // ToDo: 检查函数名是否有冲突
    // 如何兼容const修饰符
    // ToDo: 废弃指针，使用引用

    // 强类型匹配
    template <typename ParamT, typename RetT>
    void set_func(std::string key, std::function<RetT(const ParamT&)> fn) {
        // 必须要用值捕获，否则fn会析构
        func1_map[key] = [=](const Value* f) -> const Value* {
            ParamT* p = get_value_pointer<ParamT>(f);
            if (p != nullptr) {
                RetT _r = fn(*p);
                return to_value(_r);
            }
            return nullptr;
        };
    };
    template <typename ParamT1, typename ParamT2, typename RetT>
    void set_func(std::string key, std::function<RetT(const ParamT1&, const ParamT2&)> fn) {
        func2_map[key] = [=](const Value* f1, const Value* f2) -> const Value* {
            ParamT1* p1 = get_value_pointer<ParamT1>(f1);
            ParamT2* p2 = get_value_pointer<ParamT2>(f2);
            if (p1 != nullptr and p2 != nullptr) {
                RetT _r = fn(*p1, *p2);
                return to_value(_r);
            }
            return nullptr;
        };
    };
    template <typename ParamT1, typename ParamT2, typename ParamT3, typename RetT>
    void set_func(std::string key, std::function<RetT(const ParamT1&, const ParamT2&, const ParamT3&)> fn) {
        func3_map[key] = [=](const Value* f1, const Value* f2, const Value* f3) -> const Value* {
            ParamT1* p1 = get_value_pointer<ParamT1>(f1);
            ParamT2* p2 = get_value_pointer<ParamT2>(f2);
            ParamT3* p3 = get_value_pointer<ParamT3>(f3);
            if (p1 != nullptr and p2 != nullptr and p3 != nullptr) {
                RetT _r = fn(*p1, *p2, *p3);
                return to_value(_r);
            }
            return nullptr;
        };
    };

    template <typename ParamT1, typename ParamT2, typename ParamT3, typename ParamT4, typename RetT>
    void set_func(std::string key,
                  std::function<RetT(const ParamT1&, const ParamT2&, const ParamT3&, const ParamT4&)> fn) {
        func4_map[key] = [=](const Value* f1, const Value* f2, const Value* f3, const Value* f4) -> const Value* {
            ParamT1* p1 = get_value_pointer<ParamT1>(f1);
            ParamT2* p2 = get_value_pointer<ParamT2>(f2);
            ParamT3* p3 = get_value_pointer<ParamT3>(f3);
            ParamT4* p4 = get_value_pointer<ParamT4>(f4);
            if (p1 != nullptr and p2 != nullptr and p3 != nullptr and p4 != nullptr) {
                RetT _r = fn(*p1, *p2, *p3, *p4);
                return to_value(_r);
            }
            return nullptr;
        };
    };

    template <typename ParamT1, typename ParamT2, typename ParamT3, typename ParamT4, typename ParamT5, typename RetT>
    void set_func(
            std::string key,
            std::function<RetT(const ParamT1&, const ParamT2&, const ParamT3&, const ParamT4&, const ParamT5&)> fn) {
        func5_map[key] = [=](const Value* f1, const Value* f2, const Value* f3, const Value* f4,
                             const Value* f5) -> const Value* {
            ParamT1* p1 = get_value_pointer<ParamT1>(f1);
            ParamT2* p2 = get_value_pointer<ParamT2>(f2);
            ParamT3* p3 = get_value_pointer<ParamT3>(f3);
            ParamT4* p4 = get_value_pointer<ParamT4>(f4);
            ParamT5* p5 = get_value_pointer<ParamT5>(f5);
            if (p1 != nullptr and p2 != nullptr and p3 != nullptr and p4 != nullptr and p5 != nullptr) {
                RetT _r = fn(*p1, *p2, *p3, *p4, *p5);
                return to_value(_r);
            }
            return nullptr;
        };
    };

protected:
    bool build_visit_queue(const std::string&, std::queue<std::pair<std::string, int>>&, std::string&);

    template <typename T>
    typename std::enable_if_t<!is_pod_type<T>::value, T*> get_value_pointer(const Value* f) const {
        T* p = nullptr;
        if (f->type() == vUserType) {
            if (f->type_code() == typeid(T).hash_code()) {
                auto _f = (UserValue<T>*)f;
                p = &(_f->val);
            }
        }
        return p;
    }

    template <typename T>
    typename std::enable_if_t<is_pod_type<T>::value, T*> get_value_pointer(const Value* f) const {
        T* p = nullptr;
        if (f->type() == vUserType) {
            if (f->type_code() == typeid(T).hash_code()) {
                auto _f = (UserValue<T>*)f;
                p = &(_f->val);
            }
        } else {
            if (f->type_code() == typeid(T).hash_code()) {
                p = get_value<T>(f);
            }
        }
        return p;
    }

    bool compile(NodeHeadPtr head);

    NewEnvPtr get_instance(const std::string expression);

    Node* build_expression_tree(std::queue<std::pair<std::string, int>>&, std::unordered_set<std::string>&,
                                std::string&);

    // 寻找完美hash桶
    size_t find_perfect_bucket_size(std::unordered_set<std::string>&, NodeHeadPtr);

    bool pre_build_varible_index(Node*, size_t);

    inline uint32_t hash_f(const std::string& s) const {
        return hash_str(s);
    }
    inline size_t vars_index(const std::string& s, size_t bucket_size) const {
        return hash_f(s) % bucket_size;
    }

    // 函数类型
    static std::unordered_map<int, OperateType> function_map;

    std::unordered_map<std::string, BindFunc1> func1_map;
    std::unordered_map<std::string, BindFunc2> func2_map;
    std::unordered_map<std::string, BindFunc3> func3_map;
    std::unordered_map<std::string, BindFunc4> func4_map;
    std::unordered_map<std::string, BindFunc5> func5_map;

    std::unordered_map<std::string, NodeHeadPtr> expression_trees;
    std::mutex mutex;
};

};  // namespace libparse2
