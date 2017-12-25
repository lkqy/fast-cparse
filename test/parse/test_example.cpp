#include <gtest/gtest.h>
#include "parse/parse.h"
using namespace parse;

TEST(PARSETest, TestExample) {
    std::string exp("Abc(1,2,a)");
    ShuntingYard sy(exp);
    ASSERT_TRUE(sy.compile());
    auto Abc = [](const ValuePtr& v1, const ValuePtr& v2, const ValuePtr& v3) ->ValuePtr{
        auto _v1 = std::dynamic_pointer_cast<LongValue>(v1);
        auto _v2 = std::dynamic_pointer_cast<LongValue>(v2);
        auto _v3 = std::dynamic_pointer_cast<LongValue>(v3);
        return std::make_shared<IntValue>(_v1->val + _v2->val + _v3->val);
    };
    std::string func_name = "Abc";
    std::string a = "a";
    ValuePtr _a =  std::make_shared<LongValue>(30);
    sy.set_func3(func_name, Abc);
    sy.set_varible(a, _a);
    ValuePtr value = sy.eval();
    auto _v = std::dynamic_pointer_cast<IntValue>(value);
    ASSERT_TRUE(_v->val == 33);
    std::cout<<sy.get_log()<<"\n";
}
