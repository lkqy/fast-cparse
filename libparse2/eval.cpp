#include "libparse2/env.h"
#include "libparse2/compile.h"

namespace libparse2 {

#define check_vec_size(v1, v2)                                              \
    if (v1->is_vec() && v2->is_vec() && v1->vec_size() != v2->vec_size()) { \
        return false;                                                       \
    }

#define __NN_OP1_OP2_SEQ(v1, v2, op1, op2, func)     \
    check_vec_size(v1, v2);                          \
    if (v1->type() >= v2->type()) {                  \
        if (values[_index - 2].second == mm_Alloc) { \
            v1->op1(v2, (Value*)v1);                 \
            pop();                                   \
            return true;                             \
        } else {                                     \
            Value* result = func(v1);                \
            v1->op1(v2, result);                     \
            pop();                                   \
            pop();                                   \
            push(result, mm_Alloc);                  \
            return true;                             \
        }                                            \
    } else {                                         \
        Value* result = func(v2);                    \
        v2->op2(v1, result);                         \
        pop();                                       \
        pop();                                       \
        push(result, mm_Alloc);                      \
        return true;                                 \
    }

#define __N_OP1_OP2_SEQ(v1, v2, op1, op2, func) \
    check_vec_size(v1, v2);                     \
    if (v1->type() >= v2->type()) {             \
        Value* result = func(v1);               \
        v1->op1(v2, result);                    \
        pop();                                  \
        pop();                                  \
        push(result, mm_Alloc);                 \
        return true;                            \
    } else {                                    \
        Value* result = func(v2);               \
        v2->op2(v1, result);                    \
        pop();                                  \
        pop();                                  \
        push(result, mm_Alloc);                 \
        return true;                            \
    }

bool Env::op_varible(const Node* node) {
    switch (node->operator_type) {
        case kAND: {
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), _and, _and, alloc_bool);
        } break;
        case kOR: {
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), _or, _or, alloc_bool);
        } break;
        case kADD: {
            __NN_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), add, add, alloc);
        } break;
        case kSUB: {
            auto left = get_q(2, 0);
            auto right = get_q(2, 1);
            check_vec_size(left, right);
            if (left->type() >= right->type()) {
                Value* result = alloc(left);
                left->sub(right, result);
                pop();
                pop();
                push(result, mm_Alloc);
                return true;
            } else {
                Value* result = alloc(right);
                right->sub(left, result);
                result->negative();
                pop();
                pop();
                push(result, mm_Alloc);
                return true;
            }
        } break;
        case kMUL: {
            __NN_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), mul, mul, alloc);
        } break;
        case kDIV: {
            __NN_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), div, s_div, alloc);
        } break;
        case kPOW: {
            __NN_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), pow, s_pow, alloc);
        } break;
        case kMOD: {
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), mod, mod, alloc);
        } break;
        case kIN: {
            Value* result;
            auto left = get_q(2, 0);
            auto right = get_q(2, 1);
            if (left->is_vec()) {
                result = alloc_bool(left);
                right->vec_in(left, result);
                pop();
                pop();
                push(result, mm_Alloc);
            } else {
                result = alloc_bool(right);
                right->in(left, result);
                pop();
                pop();
                push(result, mm_Alloc);
            }
            return result != nullptr;
        } break;
        case kGREATE: {
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), gt, lt, alloc_bool);
        } break;
        case kGREATEOREQUAL: {
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), gt_or_equal, lt_or_equal, alloc_bool);
        } break;
        case kLOWER: {
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), lt, gt, alloc_bool);
        } break;
        case kLOWEROREQUAL: {
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), lt_or_equal, gt_or_equal, alloc_bool);
        } break;
        case kEQUAL: {
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), equal, equal, alloc_bool);
        } break;
        case kNOTEQUAL: {
            auto left = get_q(2, 0);
            auto right = get_q(2, 1);
            check_vec_size(left, right);
            if (left->type() >= right->type()) {
                Value* result = left->get_bool();
                left->equal(right, result);

                result->negative();
                pop();
                pop();
                push(result, mm_Alloc);
                return true;
            } else {
                Value* result = alloc_bool(right);
                right->equal(left, result);
                result->negative();
                pop();
                pop();
                push(result, mm_Alloc);
                return true;
            }
        } break;
        case kBRANCH: {
            // push(values[1]->clone());
            return true;
        } break;
        case kFUNC2: {
            const auto it = _sy->func2_map.find(node->result_varible);
            if (it == _sy->func2_map.end()) {
                add_log("could not find func2:" + node->result_varible);
                return false;
            }
            auto ret = it->second(get_q(2, 0), get_q(2, 1));
            if (ret == nullptr) {
                add_log("call func2:" + node->result_varible + " error");
                return false;
            }
            pop();
            pop();
            push(ret, mm_Alloc);
            return true;
        } break;
        case kNOT: {
            Value* result = get_q(1, 0)->get_bool();
            get_q(1, 0)->_not(result);
            pop();
            push(result, mm_Alloc);
            return true;
        } break;
        case kFUNC1: {
            const auto it = _sy->func1_map.find(node->result_varible);
            if (it == _sy->func1_map.end()) {
                add_log("could not find func1:" + node->result_varible);
                return false;
            }
            auto ret = it->second(get_q(1, 0));
            if (ret == nullptr) {
                add_log("call func1:" + node->result_varible + " error");
                return false;
            }
            pop();
            push(ret, mm_Alloc);
            return true;
        } break;
        case kFUNC3: {
            const auto it = _sy->func3_map.find(node->result_varible);
            if (it == _sy->func3_map.end()) {
                add_log("could not find func3:" + node->result_varible);
                return false;
            }
            auto ret = it->second(get_q(3, 0), get_q(3, 1), get_q(3, 2));
            if (ret == nullptr) {
                add_log("call func3:" + node->result_varible + " error");
                return false;
            }
            pop();
            pop();
            pop();
            push(ret, mm_Alloc);
            return true;
        } break;
        case kFUNC4: {
            const auto it = _sy->func4_map.find(node->result_varible);
            if (it == _sy->func4_map.end()) {
                add_log("could not find func4:" + node->result_varible);
                return false;
            }
            auto ret = it->second(get_q(4, 0), get_q(4, 1), get_q(4, 2), get_q(4, 3));
            if (ret == nullptr) {
                add_log("call func4:" + node->result_varible + " error");
                return false;
            }
            pop();
            pop();
            pop();
            pop();
            push(ret, mm_Alloc);
            return true;
        } break;
        case kFUNC5: {
            const auto it = _sy->func5_map.find(node->result_varible);
            if (it == _sy->func5_map.end()) {
                add_log("could not find func5:" + node->result_varible);
                return false;
            }
            auto ret = it->second(get_q(5, 0), get_q(5, 1), get_q(5, 2), get_q(5, 3), get_q(5, 4));
            if (ret == nullptr) {
                add_log("call func5:" + node->result_varible + " error");
                return false;
            }
            pop();
            pop();
            pop();
            pop();
            pop();
            push(ret, mm_Alloc);
            return true;
        } break;
        default:
            add_log("not support operator");
            return false;
    };
}

// return true：执行正确；否则表示执行失败
bool Env::eval_expression(const Node* node) {
    if (node->node_level == nAllChildIsLeaf) {
        return eval_leaf_expression(node);
    }
    // node->print("eval_f_leaf");
    // node->print(2);
    const int count = node->op_count;
    if (count > 0) {
        if (node->operator_type == kTERNARY) {
            if (eval_expression(node->nodes[0])) {
                auto entry = top()->get_bool();
                if (entry->type() == vBool) {
                    if (((BoolValue*)entry)->val) {
                        return eval_expression(node->nodes[1]);
                    } else {
                        return eval_expression(node->nodes[2]);
                    }
                } else if (entry->type() == vBools) {
                    auto bools = ((BoolArrayValue*)entry)->val;
                    if (not eval_expression(node->nodes[1])) {
                        add_log("kTERNARY sub op1 error");
                        return false;
                    }
                    if (not eval_expression(node->nodes[2])) {
                        add_log("kTERNARY sub op2 error");
                        return false;
                    }
                    auto left = to_array(get_q(2, 0), bools.size());
                    auto right = to_array(get_q(2, 1), bools.size());
                    auto ret = merge_array(bools, left, right);
                    if (ret == nullptr) {
                        add_log("type is error, left:" + std::to_string(left->type()) +
                                ", right:" + std::to_string(right->type()));
                        return false;
                    }
                    push(ret, mm_Alloc);
                    return true;
                } else {
                    add_log("first type should be bool, actual:" + std::to_string(top()->type()));
                    return false;
                }
            } else {
                add_log("eval ternary fail");
                return false;
            }
        } else if (node->operator_type == kAND and _enable_short_circuit) {
            if (not eval_expression(node->nodes[0])) {
                add_log("and op error in first");
                return false;
            }
            if (top()->type() == vBool and not((BoolValue*)top())->val) {
                return true;
            }
            if (not eval_expression(node->nodes[1])) {
                add_log("and op error in second");
                return false;
            }
            if (top()->type() == vBool and not((BoolValue*)top())->val) {
                pop2();
                return true;
            }
            // 执行两个值 and 操作
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), _and, _and, alloc_bool);
        } else if (node->operator_type == kOR and _enable_short_circuit) {
            if (not eval_expression(node->nodes[0])) {
                add_log("or op error in first");
                return false;
            }
            if (top()->type() == vBool and ((BoolValue*)top())->val) {
                return true;
            }
            if (not eval_expression(node->nodes[1])) {
                add_log("or op error in second");
                return false;
            }
            if (top()->type() == vBool and ((BoolValue*)top())->val) {
                auto v = top();
                pop2();
                return true;
            }
            // 执行两个值 or 操作
            __N_OP1_OP2_SEQ(get_q(2, 0), get_q(2, 1), _or, _or, alloc_bool);
        } else if (node->operator_type == kBRANCH) {
            if (not eval_expression(node->nodes[0])) {
                add_log("branch op error in first");
                return false;
            }
            if (not eval_expression(node->nodes[1])) {
                add_log("branch op error in second");
                return false;
            }
            return true;
        } else if (node->operator_type != kASIGN) {
            for (int i = 0; i < count; ++i) {
                if (not eval_expression(node->nodes[i])) {
                    add_log("eval error:" + std::to_string(i));
                    return false;
                }
            }
            // 求值
            if (not op_varible(node)) {
                add_log("op varbile error, type:" + to_string(node->operator_type));
                return false;
            }
            return true;
        } else {
            // 赋值
            if (not eval_expression(node->nodes[1])) {
                add_log("asign error");
                return false;
            }
            auto v = top()->clone();
            push(v, mm_Var);
            add_var(node->nodes[0]->result_varible, v);
            return true;
        }
    } else if (count == -1) {
        push(node->value, mm_Const);
        return true;
    } else {
        auto v = get_var(node->var_index);
        if (v == nullptr) {
            v = set_var(node->var_index);
            if (v == nullptr) {
                add_log("lack varible:" + node->result_varible);
                return false;
            }
        }
        push(v, mm_Var);
        return true;
    }
    return true;
}

bool Env::eval_leaf_expression(const Node* node) {
    const int count = node->op_count;
    // node->print("eval_leaf");
    // node->print(2);
    if (node->operator_type == kTERNARY) {
        if (eval_expression(node->nodes[0])) {
            auto entry = top()->get_bool();
            if (entry->type() == vBool) {
                if (((BoolValue*)entry)->val) {
                    return eval_expression(node->nodes[1]);
                } else {
                    return eval_expression(node->nodes[2]);
                }
            } else if (entry->type() == vBools) {
                auto bools = ((BoolArrayValue*)entry)->val;
                if (not eval_expression(node->nodes[1])) {
                    add_log("kTERNARY sub op1 error");
                    return false;
                }
                if (not eval_expression(node->nodes[2])) {
                    add_log("kTERNARY sub op2 error");
                    return false;
                }
                auto left = to_array(get_q(2, 0), bools.size());
                auto right = to_array(get_q(2, 1), bools.size());
                auto ret = merge_array(bools, left, right);
                push(ret, mm_Alloc);
                return true;
            } else {
                add_log("first type should be bool, actual:" + std::to_string(top()->type()));
                return false;
            }
        } else {
            add_log("eval ternary fail");
            return false;
        }
    } else if (node->operator_type == kBRANCH) {
        if (not eval_expression(node->nodes[0])) {
            add_log("branch op error in first");
            return false;
        }
        if (not eval_expression(node->nodes[1])) {
            add_log("branch op error in second");
            return false;
        }
        return true;
    } else if (node->operator_type != kASIGN) {
        for (int i = 0; i < count; ++i) {
            const auto& n = node->nodes[i];
            const auto& _c = n->op_count;
            if (_c == -1) {
                push(n->value, mm_Const);
            } else if (_c == 0) {
                auto v = get_var(n->var_index);
                if (v == nullptr) {
                    v = set_var(n->var_index);
                    if (v == nullptr) {
                        add_log("lack varible:" + n->result_varible);
                        return false;
                    }
                }
                push(get_var(n->var_index), mm_Var);
            } else {
                add_log("op count error");
                return false;
            }
        }
        // 求值
        if (not op_varible(node)) {
            add_log("op varbile error in leaf_eval");
            return false;
        }
        return true;
    } else {
        // 赋值
        const auto& n = node->nodes[1];
        const auto _c = n->op_count;
        if (_c == -1) {  // 常量
            push(n->value, mm_Const);
            add_var(node->nodes[0]->result_varible, n->value->clone());
        } else if (_c == 0) {
            auto v = get_var(node->nodes[1]->var_index);
            if (v == nullptr) {
                v = set_var(node->var_index);
                if (v == nullptr) {
                    add_log("lack varible:" + node->result_varible);
                    return false;
                }
            }
            push(v, mm_Var);
            add_var(node->nodes[0]->result_varible, top()->clone());
        } else {
        }
        return true;
    }
    return true;
}

}  // namespace libparse2
