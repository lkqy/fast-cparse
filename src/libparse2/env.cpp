#include "env.h"
#include "compile.h"

namespace libparse2 {
bool Env::eval() {
    return _eval();
}
bool Env::eval(std::string& t) {
    // ToDo: 确认变量都已添加
    if (not _eval())
        return false;
    auto v = top();
    if (v != nullptr) {
        if (v->type() == vUserType) {
            if (v->type_code() == typeid(std::string).hash_code()) {
                auto _v = (UserValue<std::string>*)v;
                t = _v->val;
            } else {
                return false;
            }
        } else {
            // 支持类型变化
            if (v->type() == vString) {
                t = ((StringValue*)v)->val;
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
bool Env::get(const std::string& key, std::string& t) {
    // ToDo: 确认变量都已添加
    if (not _eval())
        return false;
    auto v = get_var(key);
    if (v != nullptr) {
        if (v->type() == vUserType) {
            if (v->type_code() == typeid(std::string).hash_code()) {
                auto _v = (UserValue<std::string>*)v;
                t = _v->val;
            } else {
                return false;
            }
        } else {
            // 支持类型变化
            if (v->type() == vString) {
                t = ((StringValue*)v)->val;
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
bool Env::eval(std::vector<std::string>& t) {
    if (not _eval())
        return false;
    auto v = top();
    if (v != nullptr) {
        if (v->type() == vUserType) {
            if (v->type_code() == typeid(std::vector<std::string>).hash_code()) {
                auto _v = (UserValue<std::vector<std::string>>*)v;
                t = _v->val;
            } else {
                return false;
            }
        } else {
            // 支持类型变化
            if (v->type() == vStrings) {
                for (auto _v : ((StringArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vString) {
                t.push_back(((StringValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((StringValue*)v)->val);
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
bool Env::get(const std::string& key, std::vector<std::string>& t) {
    if (not _eval())
        return false;
    auto v = get_var(key);
    if (v != nullptr) {
        if (v->type() == vUserType) {
            if (v->type_code() == typeid(std::vector<std::string>).hash_code()) {
                auto _v = (UserValue<std::vector<std::string>>*)v;
                t = _v->val;
            } else {
                return false;
            }
        } else {
            // 支持类型变化
            if (v->type() == vStrings) {
                for (auto _v : ((StringArrayValue*)v)->val) {
                    t.push_back(_v);
                }
            } else if (v->type() == vString) {
                t.push_back(((StringValue*)v)->val);
                for (size_t i = 1; i < _vec_size; ++i) {
                    t.push_back(((StringValue*)v)->val);
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
const std::string Env::error_log() const {
    if (not is_valid()) {
        return _head->status_log;
    }
    return _error_log.str();
};
bool Env::is_valid() const {
    return _head->node != nullptr;
};
Env::~Env() {
    clear();
    for (const auto& v : _vars) {
        if (v) {
            delete v;
        }
    }
    for (const auto& v : _unused_vars) {
        if (v) {
            delete v;
        }
    }
    if (data_source != nullptr) {
        delete data_source;
    }
}
void Env::clear() {
    for (size_t i = 0; i < _index; ++i) {
        const auto& p = values[i];
        if (p.second == mm_Alloc && p.first) {
            destory(p.first);
        }
    }
    _index = 0;
    _is_evaled = false;
}
void Env::reset() {
    const size_t size = _vars.size();
    for (size_t i = 0; i < size; ++i) {
        if (_vars[i]) {
            destory(_vars[i]);
        }
        _vars[i] = nullptr;
    }
    for (size_t i = 0; i < _index; ++i) {
        const auto& p = values[i];
        if (p.second == mm_Alloc && p.first) {
            destory(p.first);
        }
    }
    _index = 0;
    _is_evaled = false;
    _enable_short_circuit = true;
}
bool Env::_eval() {
    // 只有正确的情况下才能反复求值
    if (_is_evaled)
        return true;
    if (eval_expression(_head->node)) {
        _is_evaled = true;
        return top() != nullptr;
    } else {
        add_log("eval expression error");
    }
    return false;
}
void Env::push(const Value* v, MemType mm_type) {
    values[_index++] = std::make_pair(v, mm_type);
};
// 删掉第二个元素，最后一个元素归位
void Env::pop2() {
    --_index;
    auto& p = values[_index - 1];
    if (p.second == mm_Alloc) {
        destory(p.first);
    }
    values[_index - 1] = values[_index];
};
void Env::pop() {
    auto& p = values[--_index];
    if (p.second == mm_Alloc) {
        destory(p.first);
    }
};
const Value* Env::get_q(size_t t, size_t i) {
    return values[_index - t + i].first;
};
const Value* Env::get_q(size_t t) {
    return values[t].first;
};
const Value* Env::top() {
    return values[_index - 1].first;
};
const Value* Env::set_var(size_t var_index) {
    if (data_source != nullptr && data_func) {
        const std::string& name = _head->fast_find_vars[var_index];
        const Value* v = data_func(data_source, name);
        add_var(name, v);
        return get_var(var_index);
    }
    return nullptr;
}

const Value* Env::get_var(size_t i) {
    return i >= _vars.size() ? nullptr : _vars[i];
};

const Value* Env::get_var(const std::string& name) {
    if (name.empty()) {
        return top();
    }
    if (_head->varibles.find(name) == _head->varibles.end()) {
        return nullptr;
    }
    size_t i = _sy->vars_index(name, _head->bucket_size);
    return i >= _vars.size() ? nullptr : _vars[i];
};
bool Env::has_var(const std::string& key) {
    return _head->varibles.find(key) != _head->varibles.end();
}
Env::Env(const NodeHeadPtr& head, const Compile* sy)
        : _vars_size(head->bucket_size),
          _index(0),
          _vars(head->bucket_size, nullptr),
          values(head->elements),
          _sy(sy),
          _head(head),
          _is_evaled(false),
          data_source(nullptr),
          _vec_size(0),
          _enable_short_circuit(true) {
    if (head->node) {
    }
    new_count = 0;
};
bool Env::add_var(const std::string& name, const Value* value) {
    const size_t index = (uint32_t)(hash_str(name)) % _head->bucket_size;
    if (_head->fast_find_vars[index] == name) {
        if (_vars[index]) {
            destory(_vars[index]);
        }
        _vars[index] = value;
        return true;
    }
    destory(value);
    return false;
}

Value* Env::alloc_bool(const Value* v) {
    auto r = alloc(v->is_vec() ? vBools : vBool, v->vec_size());
    return r == nullptr ? v->get_bool() : r;
}
Value* Env::alloc(const Value* v) {
    Value* r = alloc(v->type(), v->vec_size());

    return r != nullptr ? r : v->clone();
}

Value* Env::alloc(ValueType type, const size_t vec_size) {
    /// 如果使用变量复用，在长度为10情况下，比新申请快10倍左右
    const int _unused_vars_end = _unused_vars.size();
    if (vec_size > 0) {
        for (int i = _unused_vars_start; i < _unused_vars_end; ++i) {
            if (_unused_vars[i]->vec_size() == vec_size and _unused_vars[i]->type() == type) {
                auto v = _unused_vars[i];
                _unused_vars[i] = _unused_vars[_unused_vars_start];
                _unused_vars[_unused_vars_start++] = nullptr;
                return v;
            }
        }
    } else {
        for (int i = _unused_vars_start; i < _unused_vars_end; ++i) {
            if (_unused_vars[i]->type() == type) {
                auto v = _unused_vars[i];
                _unused_vars[i] = _unused_vars[_unused_vars_start];
                _unused_vars[_unused_vars_start++] = nullptr;
                return v;
            }
        }
    }
    return nullptr;
}

void Env::destory(const Value* v) {
    if (_unused_vars_start > 0) {
        _unused_vars[--_unused_vars_start] = (Value*)v;
        return;
    }
    _unused_vars.push_back((Value*)v);
}
}
