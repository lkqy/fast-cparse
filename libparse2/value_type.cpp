#include "libparse2/value_type.h"

namespace libparse2 {

Value* merge_array(const std::vector<bool>& bools, const Value* left, const Value* right) {
    if (not left->is_vec() or not right->is_vec()) {
        return nullptr;
    }
    auto size = bools.size();
    switch (left->type()) {
        case vInts: {
            auto v1 = ((IntArrayValue*)left)->val;
            switch (right->type()) {
                case vBools: {
                    auto v2 = ((BoolArrayValue*)right)->val;
                    std::vector<int32_t> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new IntArrayValue(val);
                } break;
                case vInts: {
                    auto v2 = ((IntArrayValue*)right)->val;
                    std::vector<int32_t> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new IntArrayValue(val);
                } break;
                case vLongs: {
                    auto v2 = ((LongArrayValue*)right)->val;
                    std::vector<int64_t> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new LongArrayValue(val);
                } break;
                case vFloats: {
                    auto v2 = ((FloatArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                case vDoubles: {
                    auto v2 = ((DoubleArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                default:
                    return nullptr;
            };
            return nullptr;
        } break;
        case vLongs: {
            auto v1 = ((LongArrayValue*)left)->val;
            switch (right->type()) {
                case vBools: {
                    auto v2 = ((BoolArrayValue*)right)->val;
                    std::vector<int64_t> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new LongArrayValue(val);
                } break;
                case vInts: {
                    auto v2 = ((IntArrayValue*)right)->val;
                    std::vector<int64_t> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new LongArrayValue(val);
                } break;
                case vLongs: {
                    auto v2 = ((LongArrayValue*)right)->val;
                    std::vector<int64_t> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new LongArrayValue(val);
                } break;
                case vFloats: {
                    auto v2 = ((FloatArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                case vDoubles: {
                    auto v2 = ((DoubleArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                default:
                    return nullptr;
            };
            return nullptr;
        } break;
        case vFloats: {
            auto v1 = ((FloatArrayValue*)left)->val;
            switch (right->type()) {
                case vBools: {
                    auto v2 = ((BoolArrayValue*)right)->val;
                    std::vector<float> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new FloatArrayValue(val);
                } break;
                case vInts: {
                    auto v2 = ((IntArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                case vLongs: {
                    auto v2 = ((LongArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                case vFloats: {
                    auto v2 = ((FloatArrayValue*)right)->val;
                    std::vector<float> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new FloatArrayValue(val);
                } break;
                case vDoubles: {
                    auto v2 = ((DoubleArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                default:
                    return nullptr;
            };
            return nullptr;
        } break;
        case vDoubles: {
            auto v1 = ((DoubleArrayValue*)left)->val;
            switch (right->type()) {
                case vBools: {
                    auto v2 = ((BoolArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                case vInts: {
                    auto v2 = ((IntArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                case vLongs: {
                    auto v2 = ((LongArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                case vFloats: {
                    auto v2 = ((FloatArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                case vDoubles: {
                    auto v2 = ((DoubleArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                default:
                    return nullptr;
            };
        } break;
        case vBools: {
            auto v1 = ((BoolArrayValue*)left)->val;
            switch (right->type()) {
                case vBools: {
                    auto v2 = ((BoolArrayValue*)right)->val;
                    std::vector<bool> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new BoolArrayValue(val);
                } break;
                case vInts: {
                    auto v2 = ((IntArrayValue*)right)->val;
                    std::vector<int32_t> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new IntArrayValue(val);
                } break;
                case vLongs: {
                    auto v2 = ((LongArrayValue*)right)->val;
                    std::vector<int64_t> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new LongArrayValue(val);
                } break;
                case vFloats: {
                    auto v2 = ((FloatArrayValue*)right)->val;
                    std::vector<float> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new FloatArrayValue(val);
                } break;
                case vDoubles: {
                    auto v2 = ((DoubleArrayValue*)right)->val;
                    std::vector<double> val(size);
                    for (size_t i = 0; i < size; ++i) {
                        val[i] = bools[i] ? v1[i] : v2[i];
                    }
                    return new DoubleArrayValue(val);
                } break;
                default:
                    return nullptr;
            };
        } break;
        case vStrings: {
            auto v1 = ((StringArrayValue*)left)->val;
            auto v2 = ((StringArrayValue*)right)->val;
            std::vector<std::string> val(size);
            for (size_t i = 0; i < size; ++i) {
                val[i] = bools[i] ? v1[i] : v2[i];
            }
            return new StringArrayValue(val);
        } break;
        default:
            return nullptr;
    };
}

const Value* to_array(const Value* value, size_t size) {
    if (value->is_vec()) {
        return value;
    }
    switch (value->type()) {
        case vInt: {
            std::vector<int> val(size, ((IntValue*)value)->val);
            return new IntArrayValue(val);
        } break;
        case vLong: {
            std::vector<long> val(size, ((LongValue*)value)->val);
            return new LongArrayValue(val);
        } break;
        case vFloat: {
            std::vector<float> val(size, ((FloatValue*)value)->val);
            return new FloatArrayValue(val);
        } break;
        case vDouble: {
            std::vector<double> val(size, ((DoubleValue*)value)->val);
            return new DoubleArrayValue(val);
        } break;
        case vBool: {
            std::vector<bool> val(size, ((BoolValue*)value)->val);
            return new BoolArrayValue(val);
        } break;
        case vString: {
            std::vector<std::string> val(size, ((StringValue*)value)->val);
            return new StringArrayValue(val);
        } break;
        default:
            return nullptr;
    };
}

Value* to_set_value(std::vector<Value*>& values) {
    switch (values[0]->type()) {
        case vInt: {
            std::unordered_set<int32_t> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (IntValue*)v;
                    vals.insert(_v->val);
                }
            }
            return new IntSetValue(vals);
        } break;
        case vLong: {
            std::unordered_set<int64_t> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (LongValue*)v;
                    vals.insert(_v->val);
                }
            }
            return new LongSetValue(vals);
        } break;
        case vFloat: {
            std::unordered_set<float> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (FloatValue*)v;
                    vals.insert(_v->val);
                }
            }
            return new FloatSetValue(vals);
        } break;
        case vDouble: {
            std::unordered_set<double> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (DoubleValue*)v;
                    vals.insert(_v->val);
                }
            }
            return new DoubleSetValue(vals);
        } break;
        case vString: {
            std::unordered_set<std::string> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (StringValue*)v;
                    vals.insert(_v->val);
                }
            }
            return new StringSetValue(vals);
        } break;
        default:
            return nullptr;
    };
    return nullptr;
}

Value* to_array_value(std::vector<Value*>& values) {
    switch (values[0]->type()) {
        case vInt: {
            std::vector<int32_t> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (IntValue*)v;
                    vals.push_back(_v->val);
                }
            }
            return new IntArrayValue(vals);
        } break;
        case vLong: {
            std::vector<int64_t> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (LongValue*)v;
                    vals.push_back(_v->val);
                }
            }
            return new LongArrayValue(vals);
        } break;
        case vFloat: {
            std::vector<float> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (FloatValue*)v;
                    vals.push_back(_v->val);
                }
            }
            return new FloatArrayValue(vals);
        } break;
        case vDouble: {
            std::vector<double> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (DoubleValue*)v;
                    vals.push_back(_v->val);
                }
            }
            return new DoubleArrayValue(vals);
        } break;
        case vString: {
            std::vector<std::string> vals;
            for (auto& v : values) {
                if (v->type() == values[0]->type()) {
                    auto _v = (StringValue*)v;
                    vals.push_back(_v->val);
                }
            }
            return new StringArrayValue(vals);
        } break;
        default:
            return nullptr;
    };
    return nullptr;
}

Value* to_value(bool f) {
    return new BoolValue(f);
}
Value* to_value(int32_t f) {
    return new IntValue(f);
}
Value* to_value(int64_t f) {
    return new LongValue(f);
}
Value* to_value(float f) {
    return new FloatValue(f);
}
Value* to_value(double f) {
    return new DoubleValue(f);
}
Value* to_value(const char* f) {
    return new StringValue(f);
}
Value* to_value(std::string& f) {
    return new StringValue(f);
}
Value* to_value(std::vector<bool>& f) {
    return new BoolArrayValue(f);
}
Value* to_value(std::vector<int32_t>& f) {
    return new IntArrayValue(f);
}
Value* to_value(std::vector<int64_t>& f) {
    return new LongArrayValue(f);
}
Value* to_value(std::vector<float>& f) {
    return new FloatArrayValue(f);
}
Value* to_value(std::vector<double>& f) {
    return new DoubleArrayValue(f);
}
Value* to_value(std::vector<std::string>& f) {
    return new StringArrayValue(f);
}
Value* to_value(Value* f) {
    return f;
}

}  // namespace libparse2
