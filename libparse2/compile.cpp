#include "compile.h"

namespace libparse2 {

using namespace gramma;

NewEnvPtr Compile::get_instance(const std::string expression) {
    auto it = expression_trees.find(expression);
    if (it == expression_trees.end()) {
        // 初始化
        auto head = std::make_shared<NodeHead>();
        head->expression = expression;
        if (compile(head)) {
            expression_trees[expression] = head;
        }
        auto p = new Env(head, this);
        NewEnvPtr env(p);
        return env;
    } else {
        auto p = new Env(it->second, this);
        NewEnvPtr env(p);
        return env;
    }
    return nullptr;
};

// 函数类型
std::unordered_map<int, OperateType> Compile::function_map = {{1, kFUNC1}, {2, kFUNC2}, {3, kFUNC3}};

bool Compile::compile(NodeHeadPtr head) {
    std::unordered_set<std::string> varible_names;
    std::queue<std::pair<std::string, int>> queue;
    if (build_visit_queue(head->expression, queue, head->status_log)) {
        size_t q_size = queue.size();
        auto expression_root = build_expression_tree(queue, head->varibles, head->status_log);
        if (expression_root) {
            head->status_log += "init expression success";
            head->node = expression_root;
            head->elements = q_size;
            head->bucket_size = find_perfect_bucket_size(head->varibles, head);
            return true;
        } else {
            head->status_log += " -> init node tree failed";
        }
    } else {
        head->status_log += " -> init expression failed";
    }
    return false;
}

bool Compile::build_visit_queue(const std::string& expression, std::queue<std::pair<std::string, int>>& queue,
                                std::string& compile_log) {
    const char* input = expression.c_str();
    const char* end = input + strlen(input);
    const char* start = input;

    std::stack<std::pair<std::string, int>> stack;
    std::stack<int> stack_op_count;
    StrTokenType cur_type;
    StrTokenType pre_type = TYPE_PRE;
    int params_number = 0;
    stack_op_count.push(0);
    std::string pre_token = "";
    while (start < end) {
        const char* p = nullptr;
        std::string token = next_token(start, &p, cur_type, pre_type);
        if (token == "in" and pre_token == "not") {
            compile_log += " not support 'not in'";
            return false;
        }
        pre_type = cur_type;
        start = p;
        if (token.empty()) {
            break;
        } else if (is_operator(token)) {
            while (!stack.empty() && is_operator(stack.top().first)) {
                if ((op_left_assoc(token) && op_preced(token) <= op_preced(stack.top().first)) ||
                    (!op_left_assoc(token) && op_preced(token) < op_preced(stack.top().first))) {
                    queue.push(stack.top());
                    stack.pop();
                    params_number = stack_op_count.top();
                    stack_op_count.pop();
                } else {
                    break;
                }
            }
            // ToDo: 三目运算太扎眼了
            if (token == "?") {
                continue;
            }
            stack.push(std::make_pair(token, tOperator));
            stack_op_count.push(0);
        } else if (is_const(token)) {
            queue.push(std::make_pair(token, tConst));
            stack_op_count.top() += 1;
        } else if (is_inner_function(token)) {
            stack_op_count.top() += 1;
            stack.push(std::make_pair(token, tInnerFunction));
            stack_op_count.push(0);
        } else if (is_function(token)) {
            stack_op_count.top() += 1;
            stack.push(std::make_pair(token, tFunctionBeginIndex));
            stack_op_count.push(0);
        } else if (is_varible(token)) {
            queue.push(std::make_pair(token, tVarible));
            stack_op_count.top() += 1;
        } else if (token == ",") {
            bool find = false;
            while (!stack.empty()) {
                if (stack.top().first == "(") {
                    find = true;
                    break;
                } else {
                    queue.push(stack.top());
                    stack.pop();
                    params_number = stack_op_count.top();
                    stack_op_count.pop();
                }
            }
            if (!find) {
                compile_log += "Error: function parentheses mismatched";
                return false;
            }
        } else if (token == "(") {
            stack.push(std::make_pair(token, tLeftParenthesis));
            stack_op_count.push(0);
        } else if (token == ")") {
            bool find = false;
            while (!stack.empty()) {
                if (stack.top().first == "(") {
                    find = true;
                    stack.pop();
                    params_number = stack_op_count.top();
                    stack_op_count.pop();
                    break;
                } else {
                    queue.push(stack.top());
                    stack.pop();
                    params_number = stack_op_count.top();
                    stack_op_count.pop();
                }
            }
            if (!find) {
                compile_log += "Error: parentheses mismatched";
                return false;
            }
            if (!stack.empty()) {
                int token_type = stack.top().second;
                if (token_type >= tInnerFunction || token_type >= tFunctionBeginIndex) {
                    stack.top().second += params_number;
                    queue.push(stack.top());
                    stack.pop();
                    params_number = stack_op_count.top();
                    stack_op_count.pop();
                }
            }
        } else {
            compile_log += "error expression: unknown token:" + token;
            return false;
        }
        pre_token = token;
    }
    while (!stack.empty()) {
        if (stack.top().first == "(" || stack.top().first == ")") {
            compile_log += "Error: parentheses mismatched";
            return false;
        }
        queue.push(stack.top());
        stack.pop();
        stack_op_count.pop();
    }
    return true;
}

bool Compile::pre_build_varible_index(Node* tree, size_t bucket_size) {
    bool has_leaf = false;
    bool has_non_leaf = false;
    if (tree) {
        if (not tree->result_varible.empty()) {
            tree->var_index = vars_index(tree->result_varible, bucket_size);
        }
        for (Node* node : tree->nodes) {
            if (pre_build_varible_index(node, bucket_size)) {
                has_leaf = true;
            } else {
                has_non_leaf = true;
            }
        }
    }
    if (has_leaf and not has_non_leaf) {
        tree->node_level = nAllChildIsLeaf;
    } else if (has_leaf and has_non_leaf) {
        tree->node_level = nEtherChildIsLeaf;
    } else if (not has_leaf and not has_non_leaf) {
        tree->node_level = nLeaf;
    } else if (not has_leaf and has_non_leaf) {
        tree->node_level = nEtherChildIsLeaf;
    }
    return tree->nodes.size() == 0;
}

// 寻找完美hash桶
size_t Compile::find_perfect_bucket_size(std::unordered_set<std::string>& varible_names, NodeHeadPtr head) {
    size_t begin_size = (varible_names.size() + 2);
    size_t limit_size = (varible_names.size() + 1) * 50;
    std::vector<bool> h_vec;
    h_vec.reserve(limit_size);
    size_t fast_find_vars_size = 0;
    while (begin_size < limit_size) {
        bool conflicts = false;

        h_vec.assign(begin_size, false);
        for (const auto& s : varible_names) {
            size_t index = hash_f(s) % begin_size;
            if (h_vec[index]) {
                conflicts = true;
                break;
            } else {
                h_vec[index] = true;
            }
        }
        if (conflicts) {
            begin_size += 1;
        } else {
            break;
        }
    }
    if (begin_size < limit_size) {
        fast_find_vars_size = begin_size;
        head->fast_find_vars.resize(fast_find_vars_size, "");
        for (const auto& s : varible_names) {
            size_t index = hash_f(s) % begin_size;
            head->fast_find_vars[index] = s;
        }
        pre_build_varible_index(head->node, fast_find_vars_size);
    } else {
        // ToDo: error
        fast_find_vars_size = 0;
    }
    return fast_find_vars_size;
}

Node* Compile::build_expression_tree(std::queue<std::pair<std::string, int>>& queue,
                                     std::unordered_set<std::string>& varible_names, std::string& compile_log) {
    std::stack<Node*> stack;
    int temporal_varible_index = 0;

    auto asign_nodes = [&](Node* node) {
        std::stack<Node*> temp_stack;
        for (int i = 0; i < node->op_count; ++i) {
            temp_stack.push(stack.top());
            stack.pop();
        }
        while (!temp_stack.empty()) {
            node->nodes.push_back(temp_stack.top());
            temp_stack.pop();
        }
        stack.push(node);
    };

    auto process_const = [&](const auto& token) {
        Node* node = new Node();
        node->op_count = -1;
        node->result_varible = token;
        node->value = paser_const_value(token);
        if (not node->value) {
            compile_log += "; paser const value failed. ";
            return false;
        }
        stack.push(node);
        return true;
    };
    auto process_varible = [&](const auto& token) {
        //如果变量是同一个，就索引
        Node* node = new Node();
        node->op_count = 0;
        node->result_varible = token;
        varible_names.insert(token);
        stack.push(node);
        return true;
    };
    auto process_operator = [&](const auto& token) {
        size_t count = op_arg_count(token);
        if (stack.size() < count) {
            compile_log += "  operator count is wrong" + std::to_string(stack.size()) + " " + std::to_string(count);
            return false;
        }
        Node* node = new Node();
        node->op_count = count;
        node->operator_type = get_operator(token);
        //不存结果
        // node->value = std::make_shared<BoolValue>(false);
        asign_nodes(node);
        return true;
    };

    //内置函数都是常量表达式
    auto process_inner_function = [&](auto& token, int param_number) {
        long stack_size = stack.size();
        if (stack_size < param_number) {
            return false;
        }
        auto free_vector_node = [](const std::vector<Value*>& values) {
            for (const auto& v : values) {
                delete v;
            }
        };

        std::stack<Node*> temp_stack;
        for (int i = 0; i < param_number; ++i) {
            temp_stack.push(stack.top());
            stack.pop();
        }
        std::vector<Value*> values;
        while (!temp_stack.empty()) {
            Node* node = temp_stack.top();
            values.push_back(node->value->clone());
            delete node;
            temp_stack.pop();
        }
        if (values.size() == 0) {
            return false;
        }
        Node* node = new Node();
        node->op_count = -1;
        if (token == "SET") {
            // ToDo: check values类型
            node->value = to_set_value(values);

            if (node->value->type() == vNull) {
                compile_log += "all value not the same type in SET";
                free_vector_node(values);
                return false;
            }
        } else if (token == "ARRAY") {
            // ToDo: check values类型
            node->value = to_array_value(values);

            if (node->value->type() == vNull) {
                compile_log += "all value not the same type in ARRAY";
                free_vector_node(values);
                return false;
            }
        } else {
            compile_log += "not implemented";
            free_vector_node(values);
            return false;
        }
        free_vector_node(values);
        stack.push(node);
        return true;
    };

    auto process_function = [&](auto& token, int param_number) {
        long stack_size = stack.size();
        if (stack_size < param_number)
            return false;
        Node* node = new Node();
        node->op_count = param_number;
        node->result_varible = token;
        if (false) {
            if (param_number == 1) {
                if (func1_map.find(token) == func1_map.end())
                    return false;
            } else if (param_number == 2) {
                if (func2_map.find(token) == func2_map.end())
                    return false;
            } else if (param_number == 3) {
                if (func3_map.find(token) == func3_map.end())
                    return false;
            } else {
                return false;
            }
        }
        node->operator_type = function_map[param_number];
        asign_nodes(node);
        return true;
    };

    while (!queue.empty()) {
        ++temporal_varible_index;
        const std::pair<std::string, int>& token = queue.front();
        if (token.second == tConst) {
            if (!process_const(token.first))
                return nullptr;
        } else if (token.second == tOperator) {
            if (!process_operator(token.first)) {
                compile_log += "operator '" + token.first + "' missing valid operant";
                return nullptr;
            }
        } else if (token.second == tVarible) {
            if (!process_varible(token.first)) {
                compile_log += " invalid varible:" + token.first;
                return nullptr;
            }
        } else if (token.second >= tFunctionBeginIndex) {  //按函数处理
            if (!process_function(token.first, token.second - tFunctionBeginIndex)) {
                compile_log += " invalid function:" + token.first;
                return nullptr;
            }
        } else if (token.second >= tInnerFunction) {
            //内置函数，转换常量
            if (!process_inner_function(token.first, token.second - tInnerFunction)) {
                compile_log += " invalid inner_function:" + token.first;
                return nullptr;
            }
        } else {
            compile_log += "eror_type:" + token.first + " " + std::to_string(token.second);
            return nullptr;
        }
        queue.pop();
    }
    if (stack.size() == 1) {
        return stack.top();
    }
    std::string elements = "";
    while (stack.size() > 1) {
        stack.pop();
        elements += "'" + stack.top()->result_varible + "' ";
    }

    compile_log += " dangling elements [" + elements + "]";
    return nullptr;
}

};  // namespace libparse2
