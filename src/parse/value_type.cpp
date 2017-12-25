#include "parse/value_type.h"

namespace parse {
ValuePtr to_vec_value(std::vector<ValuePtr> &values) {
  switch (values[0]->type()) {
  case vInt:
    return std::make_shared<IntVecValue>(values);
    break;
  case vLong:
    return std::make_shared<LongVecValue>(values);
    break;
  case vFloat:
    return std::make_shared<FloatVecValue>(values);
    break;
  case vDouble:
    return std::make_shared<DoubleVecValue>(values);
    break;
  case vString:
    return std::make_shared<StringVecValue>(values);
    break;
  default:
      return std::make_shared<Value>();
  };
}

ValuePtr to_set_value(std::vector<ValuePtr> &values) {
  switch (values[0]->type()) {
  case vInt:
    return std::make_shared<IntSetValue>(values);
    break;
  case vLong:
    return std::make_shared<LongSetValue>(values);
    break;
  case vFloat:
    return std::make_shared<FloatSetValue>(values);
    break;
  case vDouble:
    return std::make_shared<DoubleSetValue>(values);
    break;
  case vString:
    return std::make_shared<StringSetValue>(values);
    break;
  default:
      return std::make_shared<Value>();
  };
}
} // namespace parse
