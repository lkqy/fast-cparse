#include "parse/value_type.h"

namespace parse {
Value *to_vec_value(std::vector<Value *> &values) {
  switch (values[0]->type()) {
  case vInt:
    return new IntVecValue(values);
    break;
  case vLong:
    return new LongVecValue(values);
    break;
  case vFloat:
    return new FloatVecValue(values);
    break;
  case vDouble:
    return new DoubleVecValue(values);
    break;
  case vString:
    return new StringVecValue(values);
    break;
  default:
    return new Value();
  };
  return new Value();
}

Value *to_set_value(std::vector<Value *> &values) {
  switch (values[0]->type()) {
  case vInt:
    return new IntSetValue(values);
    break;
  case vLong:
    return new LongSetValue(values);
    break;
  case vFloat:
    return new FloatSetValue(values);
    break;
  case vDouble:
    return new DoubleSetValue(values);
    break;
  case vString:
    return new StringSetValue(values);
    break;
  default:
    return new Value();
  };
  return new Value();
}
} // namespace parse
