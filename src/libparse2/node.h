#pragma once
#include "enum.h"
#include "parse.h"
#include "value_type.h"

namespace libparse2 {
using namespace gramma;

class Node {
public:
    Node() : value(nullptr) {
        var_index = std::numeric_limits<size_t>::max();
    };
    ~Node() {
        for (const auto& p : nodes) {
            if (p)
                delete p;
        }
        if (value)
            delete value;
    };

public:
    Value* value;

    int op_count;
    OperateType operator_type;
    //操作数
    std::vector<Node*> nodes;
    std::string result_varible;
    size_t var_index;  // 快速查找var的索引

    NodeLevel node_level;  // 是否是叶子的父节点

    // debug 使用
    void print(std::string key) const {
        std::cout << "key:" << key << ", op_count:" << op_count << ", operator_type:" << to_string(operator_type)
                  << ", result_varible:" << result_varible << ", var_index:" << var_index
                  << ", leaf_father:" << node_level << "\n";
    }
    // debug 使用
    void print(size_t i) const {
        for (size_t _i = 0; _i < i; ++_i) {
            std::cout << "\t";
        }
        std::cout << op_count << "," << to_string(operator_type) << "," << result_varible;
        if (value)
            std::cout << ", " << value->to_string() << "\n";
        else
            std::cout << "\n";

        for (auto n : nodes) {
            n->print(i + 1);
        }
    }
};

struct NodeHead {
    Node* node;
    size_t elements;
    size_t bucket_size;
    std::string expression;
    std::string status_log;
    std::vector<std::string> fast_find_vars;
    std::unordered_set<std::string> varibles;
    ~NodeHead() {
        if (node != nullptr) {
            delete node;
        }
    }
};

typedef std::shared_ptr<NodeHead> NodeHeadPtr;

}  // namespace libparse2
