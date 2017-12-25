#include <gtest/gtest.h>
#include "parse/parse.h"
using namespace parse;

TEST(PARSETest, TestParentHesis) {
    {
        std::string exp("a==1 and b==2 and c==3");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ValuePtr a = std::make_shared<LongValue>(1);
        ValuePtr b = std::make_shared<LongValue>(2);
        ValuePtr c = std::make_shared<LongValue>(3);
        std::string _a = "a";
        std::string _b = "b";
        std::string _c = "c";

        sy.set_varible(_a, a);
        sy.set_varible(_b, b);
        sy.set_varible(_c, c);
        ASSERT_TRUE(sy.eval_bool());
    }
}

TEST(PARSETest, TestSingleExpresion) {
    {
        std::string exp("1");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_TRUE(sy.eval_bool());
    }
    {
        std::string exp("1.0");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_TRUE(sy.eval_bool());
    }
    {
        std::string exp("\"abc\"");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_TRUE(sy.eval_bool());
    }
    {
        std::string exp("1 and 2");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_TRUE(sy.eval_bool());
    }
    {
        std::string exp("1 or 2");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_TRUE(sy.eval_bool());
    }
    {
        std::string exp("not 1");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_FALSE(sy.eval_bool());
    }
    {
        std::string exp("not 0.0");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_TRUE(sy.eval_bool());
    }
    {
        std::string exp("not 1.0");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_FALSE(sy.eval_bool());
    }
    {
        std::string exp("not \"sb\"");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_FALSE(sy.eval_bool());
    }
}
TEST(PARSETest, TestErrorExpresion) {
    {
        std::string exp("(a=1");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
    }
    {
        std::string exp("a=1)");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
    }
    {
        std::string exp("a=1");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
    }
    {
        std::string exp("not");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
        std::cout<<log<<"\n";
    }
    {
        std::string exp(" and ");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
        std::cout<<log<<"\n";
    }
    {
        std::string exp("or");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
        std::cout<<log<<"\n";
    }
    {
        std::string exp("(");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
        std::cout<<log<<"\n";
    }
    {
        std::string exp(")");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
        std::cout<<log<<"\n";
    }
    {
        std::string exp("[");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
        std::cout<<log<<"\n";
    }
    {
        std::string exp("[]");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_FALSE(sy.compile());
        std::cout<<log<<"\n";
    }
}

TEST(PARSETest, TestFunction) {
    {
        std::string exp("1 in VEC(1,2,3)");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        sy.eval_bool();
        std::cout<<sy.get_log()<<"\n";
        ASSERT_TRUE(sy.eval_bool());
    }
    {
        std::string exp("Abc(1,2,3)");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        auto Abc = [](const ValuePtr& v1, const ValuePtr& v2, const ValuePtr& v3) ->ValuePtr{
            return std::make_shared<IntValue>(1+2+3);
        };
        sy.set_func3("Abc", Abc);
      ValuePtr value = sy.eval();
      auto _v = std::dynamic_pointer_cast<IntValue>(value);
        ASSERT_TRUE(_v->val == 6);

    }
    {
        std::string exp("1 in Abc(1,2,3)+1");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
    }
    {
        std::string exp("1 in SET(1,2,3)");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_TRUE(sy.eval_bool());
    }
    {
        std::string exp("1 in SET(2,3)");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        ASSERT_FALSE(sy.eval_bool());
    }
    {
        std::string exp("1 in SET(1,2,3.0)+1");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
    }
}

TEST(PARSETest, TestEvalException) {
    {
        std::string exp("1 > 1.0");
        ShuntingYard sy(exp);
        std::string log;
        ASSERT_TRUE(sy.compile());
        sy.eval();
    }
}

