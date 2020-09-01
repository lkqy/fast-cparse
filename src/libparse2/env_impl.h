#pragma once
#include "env.h"

namespace libparse2 {

template <typename T>
bool Env::set_data_func(std::function<Value*(const T&, const std::string& name)> fn) {
    if (data_func)
        return false;
    data_func = [=](const Value* data, const std::string& name) -> const Value* {
        auto* p = (UserValue<T>*)(data);
        if (p == nullptr) {
            return nullptr;
        }
        Value* v = fn(p->val, name);
        return v;
    };
    return true;
}

template <typename T>
bool Env::eval(T& t, typename std::enable_if_t<is_pod_type<T>::value>* dump) {
    if (not _eval()) {
        return false;
    }
    auto v = top();
    if (v != nullptr) {
        if (v->type() == vUserType) {
            if (v->type_code() == typeid(T).hash_code()) {
                auto _v = (UserValue<T>*)v;
                t = _v->val;
            } else {
                return false;
            }
        } else {
            // 支持类型变化
            if (v->type() == vBool) {
                t = ((BoolValue*)v)->val;
            } else if (v->type() == vInt) {
                t = ((IntValue*)v)->val;
            } else if (v->type() == vDouble) {
                t = ((DoubleValue*)v)->val;
            } else if (v->type() == vLong) {
                t = ((LongValue*)v)->val;
            } else if (v->type() == vFloat) {
                t = ((FloatValue*)v)->val;
            } else {
                add_log("type_error " + std::to_string(v->type()) + " in get_pod return");
                return false;
            }
        }
    } else {
        add_log("null pointer in top when get_pod return");
        return false;
    }
    return true;
};
template <typename T>
bool Env::get(const std::string& key, T& t, typename std::enable_if_t<is_pod_type<T>::value>* dump) {
    if (not _eval())
        return false;
    auto v = get_var(key);
    if (v != nullptr) {
        if (v->type() == vUserType) {
            if (v->type_code() == typeid(T).hash_code()) {
                auto _v = (UserValue<T>*)v;
                t = _v->val;
            } else {
                return false;
            }
        } else {
            // 支持类型变化
            if (v->type() == vBool) {
                t = ((BoolValue*)v)->val;
            } else if (v->type() == vInt) {
                t = ((IntValue*)v)->val;
            } else if (v->type() == vDouble) {
                t = ((DoubleValue*)v)->val;
            } else if (v->type() == vLong) {
                t = ((LongValue*)v)->val;
            } else if (v->type() == vFloat) {
                t = ((FloatValue*)v)->val;
            } else {
                add_log("type_error " + std::to_string(v->type()) + " in get_pod return");
                return false;
            }
        }
    } else {
        add_log("null pointer in top when get_pod return");
        return false;
    }
    return true;
};
template <typename T>
bool Env::eval(std::vector<T>& t, typename std::enable_if_t<is_pod_type<T>::value>* dump) {
    if (not _eval())
        return false;
    auto v = top();
    if (v != nullptr) {
        if (v->type() == vUserType) {
            if (v->type_code() == typeid(T).hash_code()) {
                auto _v = (UserValue<std::vector<T>>*)v;
                t = _v->val;
                return true;
            } else {
                return false;
            }
        } else {
            // 支持类型变化
            if (v->type() == vBool) {
                t.push_back(((BoolValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((BoolValue*)v)->val);
                }
            } else if (v->type() == vInt) {
                t.push_back(((IntValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((IntValue*)v)->val);
                }
            } else if (v->type() == vDouble) {
                t.push_back(((DoubleValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((DoubleValue*)v)->val);
                }
            } else if (v->type() == vLong) {
                t.push_back(((LongValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((LongValue*)v)->val);
                }
            } else if (v->type() == vFloat) {
                t.push_back(((FloatValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((FloatValue*)v)->val);
                }
            } else if (v->type() == vBools) {
                for (auto _v : ((BoolArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vInts) {
                for (auto _v : ((IntArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vDoubles) {
                for (auto _v : ((DoubleArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vLongs) {
                for (auto _v : ((LongArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vFloats) {
                for (auto _v : ((FloatArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else {
                add_log("type_error " + std::to_string(v->type()) + " in get_pod return");
                return false;
            }
        }
    } else {
        add_log("null pointer in top when get_pod return");
        return false;
    }
    return true;
};
template <typename T>
bool Env::get(const std::string& key, std::vector<T>& t, typename std::enable_if_t<is_pod_type<T>::value>* dump) {
    if (not _eval())
        return false;
    auto v = get_var(key);
    if (v != nullptr) {
        if (v->type() == vUserType) {
            if (v->type_code() == typeid(T).hash_code()) {
                auto _v = (UserValue<std::vector<T>>*)v;
                t = _v->val;
                return true;
            } else {
                return false;
            }
        } else {
            // 支持类型变化
            if (v->type() == vBool) {
                t.push_back(((BoolValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((BoolValue*)v)->val);
                }
            } else if (v->type() == vInt) {
                t.push_back(((IntValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((IntValue*)v)->val);
                }
            } else if (v->type() == vDouble) {
                t.push_back(((DoubleValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((DoubleValue*)v)->val);
                }
            } else if (v->type() == vLong) {
                t.push_back(((LongValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((LongValue*)v)->val);
                }
            } else if (v->type() == vFloat) {
                t.push_back(((FloatValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((FloatValue*)v)->val);
                }
            } else if (v->type() == vBools) {
                for (auto _v : ((BoolArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vInts) {
                for (auto _v : ((IntArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vDoubles) {
                for (auto _v : ((DoubleArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vLongs) {
                for (auto _v : ((LongArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vFloats) {
                for (auto _v : ((FloatArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else {
                add_log("type_error " + std::to_string(v->type()) + " in get_pod return");
                return false;
            }
        }
    } else {
        add_log("null pointer in top when get_pod return");
        return false;
    }
    return true;
};
template <typename T>
bool Env::eval(T& t, typename std::enable_if_t<!is_pod_type<T>::value>* dump) {
    if (not _eval())
        return false;
    auto v = top();
    if (v != nullptr) {
        if (v->type() == vUserType and v->type_code() == typeid(T).hash_code()) {
            auto _v = (UserValue<T>*)v;
            t = _v->val;
            return true;
        } else {
            add_log("type_error, not find type:" + std::to_string(v->type()) +
                    ", type_code:" + std::to_string(v->type_code()) + " in get_non_pod return");
        }
    } else {
        add_log("null pointer in top when get_non_pod return");
    }
    return false;
};

template <typename T>
bool Env::get(const std::string& key, T& t, typename std::enable_if_t<!is_pod_type<T>::value>* dump) {
    if (not _eval())
        return false;
    auto v = get_var(key);
    if (v != nullptr) {
        if (v->type() == vUserType and v->type_code() == typeid(T).hash_code()) {
            auto _v = (UserValue<T>*)v;
            t = _v->val;
            return true;
        } else {
            add_log("type_error, not find type:" + std::to_string(v->type()) +
                    ", type_code:" + std::to_string(v->type_code()) + " in get_non_pod return");
        }
    } else {
        add_log("null pointer in top when get_non_pod return");
    }
    return false;
};

}  // namespace libparse2
