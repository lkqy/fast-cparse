#include <gtest/gtest.h>
#include "parse/parse.h"
using namespace parse;

TEST(PARSETest, TestNode) {
    Node *node = new Node();
    delete node;
    node = new Node();
    node->first = new Node();
    node->value = new IntValue(1);
    delete node;
}

TEST(PARSETest, TestShuntingYardBuild) {
    std::vector<std::string> exp_is_ok = {
        "1",
        "1.0",
        "\"abc\"",
        "a==11",
        "-1",
        "-1.0",
        "-1.02",
        "+1",
        "+1.0",
        "local_hour in VEC(10, 11, 12)"
    };
    std::vector<std::string> exp_is_fail = {
        "1a",
        "1a.0",
        "\"abc",
        "abc\"",
        "1a",
        "-a",
        "+a",
        "+\"1\"",
    };
    for(auto e : exp_is_ok) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        delete sy;
    }
    for(auto e : exp_is_fail) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_FALSE(sy->compile());
        delete sy;
    }
    std::vector<std::string> exps_is_true = {
        "1<=2.0",
        "1>-2.0",
        "1<2.0",
        "1>=-2.0",
        "1.0<2",
        "1.0<=2",
        "-1.0<=2",
        "1!=2",
        "-1!=2",
        "1<=2",
        "1<2",
        "-1<2",
        "1.0<2.0",
        "-1.0<2.0",
        "1.0<=2.0",
        "-1.0<=2.0",
        "not (1>2)",
        "not (-1>2)",
        "1==1",
        "1.0==1.0",
        "-1.0==-1.0",
        "\"a\"==\"a\"",
        "1.0!=2.0",
        "-1.0!=-2.0",
        "\"a\"!=\"b\"",
        "\"a\"==\"a\""
    };
    std::string log;
    for(auto e : exps_is_true) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env ;
        ASSERT_TRUE(sy->eval_bool(env, log));
        delete sy;
    }
    std::vector<std::string> exps_diff_type_false = {
        "1>2.0",
        "-1>2.0",
        "1>2",
        "-1>2",
        "1==1.0",
        "-1==1.0",
        "1==-1.0",
        "\"a\" != \"a\"",
        "1.0==1",
        "1.0==-1",
        "-1.0==-1",
        "\"a\"==1",
    };
    for(auto e : exps_diff_type_false) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env ;
        ASSERT_FALSE(sy->eval_bool(env, log));
        delete sy;
    }
    std::vector<std::string> exps_diff_type_true = {
        "1!=1.0",
        "\"a\" != \"b\"",
        "\"a\" != \"b\" and 1!=2",
        "(\"a\" != \"b\") and (1!=2)",
        "not (1!=1)",
        "1.0!=1",
        "-1.0!=1",
        "-1.0!=-1",
        "\"a\"!=1",
    };
    for(auto e : exps_diff_type_true) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env ;
        ASSERT_TRUE(sy->eval_bool(env, log));
        delete sy;
    }
}

TEST(PARSETest, TestShuntingYardBuildComplicate) {
    std::vector<std::string> exps_is_true = {
        "1!=2 and 2>1 and \"a\"!=3 and \"a\" != \"b\"",
        "(1==2) or (1>0 and \"a\" == \"a\")",
        "(((((2.0>1.0)))))",
        "(((((2.0>-1.0)))))"
    };
    std::string log;
    for(auto e : exps_is_true) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env ;
        ASSERT_TRUE(sy->eval_bool(env, log));
        delete sy;
    }

}

TEST(PARSETest, TestShuntingYardBuildVarible) {
    std::vector<std::string> exps_is_true = {
        "a!=1",
        "b!=2",
        "not( lang in VEC(\"ja\", \"vi\", \"in\"))",
        "a>b",
        "local_hour in VEC(10, 11, 12)",
        "(lang == \"jp\" and tz_hour in VEC(8, 12, 18)) or (lang==\"us\" and tz_hour in VEC(11, 15, 21)) or not (lang in VEC(\"jp\", \"us\"))",
        "(lang == \"jp\" and tz_hour in VEC(8, 12, 18)) or (lang==\"us\" and tz_hour in VEC(11, 15, 21)) or lang != \"jp\"",
        "not (lang != \"jp\")",
        "(lang == \"jp1\" and tz_hour in VEC(8, 12, 18)) or (lang==\"us\" and tz_hour in VEC(11, 15, 21)) or not (lang in VEC(\"jp1\", \"us\"))",

    };
    std::string log;
    for(auto e : exps_is_true) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new IntValue(5)),
            std::make_pair<const char*, Value*>("local_hour",  new LongValue(11)),
            std::make_pair<const char*, Value*>("tz_hour",  new LongValue(12)),
            std::make_pair<const char*, Value*>("lang",  new StringValue("jp")),
        };
        for(auto& it : env) {
            std::cout<<it.first<<", "<<it.second->to_string()<<"\n";
        }

        ASSERT_TRUE(sy->eval_bool(env, log));
        delete sy;
    }

}

TEST(PARSETest, TestShuntingYardOrigin) {
    {
        std::string log;
        std::string exp("a");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
        };
        ASSERT_TRUE(sy->eval_int(env, log) == 10);
    }
    {
        std::string log;
        std::string exp("a");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new LongValue(10)),
        };
        ASSERT_TRUE(sy->eval_long(env, log) == 10);
    }
    {
        std::string log;
        std::string exp("a");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new DoubleValue(10)),
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 10);
    }
    {
        std::string log;
        std::string exp("a");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new FloatValue(10.3)),
        };
        EXPECT_FLOAT_EQ(sy->eval_float(env, log), 10.3);
    }
    {
        std::string log;
        std::string exp("a1/2.0 + 3.0 / 2.0");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a1",  new DoubleValue(10.3)),
        };
        EXPECT_FLOAT_EQ(sy->eval_double(env, log), 10.3/2.0 + 3.0 / 2.0);
    }
    {
        std::string log;
        std::string exp("10");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        ASSERT_TRUE(sy->eval_long(env, log) == 10);
    }
    {
        std::string log;
        std::string exp("10.0");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 10);
    }
    {
        std::string log;
        std::string exp("10.3");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 10.3);
    }
    {
        std::string log;
        std::string exp("10.3 + 1.3");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 11.6);
    }
    {
        std::string log;
        std::string exp("2 ^ 3");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), pow(2, 3));
    }
    {
        std::string log;
        std::string exp("2 ^ 3 + 1.3 * 10 + 1");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), pow(2, 3) + 1.3 * 10 + 1);
    }
    {
        std::string log;
        std::string exp("10.3 ^ 1.3");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), pow(10.3, 1.3));
    }
    {
        std::string log;
        std::string exp("10.3 + 1.3 *-2.0 / 3.0");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 10.3 + 1.3 * -2.0 / 3.0);
    }
    {
        std::string log;
        std::string exp("10.3 + 1.3 * 2.0 / 3.0");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 10.3 + 1.3 * 2.0 / 3.0);
    }
    {
        std::string log;
        std::string exp("10.3 * ( 1.0 + 2.0 / 3.0 + 1.0) + 1.2 + 3.4 / 3.7");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 10.3 * ( 1.0 + 2.0 / 3.0 + 1.0) + 1.2 + 3.4 / 3.7);
    }
    {
        std::string log;
        std::string exp("10.3 * ( 1.0 + 2.0 / 3.0 +-1.0) + 1.2 + 3.4 / -3.7");
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
        };
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 10.3 * ( 1.0 + 2.0 / 3.0 + -1.0) + 1.2 + 3.4 / -3.7);
    }
}

TEST(PARSETest, TestShuntingYardNumberType) {
    std::string exp("a+b");
    ShuntingYard *sy = new ShuntingYard(exp);
    ASSERT_TRUE(sy->compile());
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new IntValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 15);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 15);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 15);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 15);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 15);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 15);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 15);
    }
}

TEST(PARSETest, TestShuntingYardNumberTypeSub) {
    std::string exp("a-b");
    ShuntingYard *sy = new ShuntingYard(exp);
    ASSERT_TRUE(sy->compile());
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new IntValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 5);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 5);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 5);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 5);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), -5);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), -5);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), -5);
    }
}

TEST(PARSETest, TestShuntingYardNumberTypeDiv) {
    std::string exp("a/b");
    ShuntingYard *sy = new ShuntingYard(exp);
    ASSERT_TRUE(sy->compile());
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new IntValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 2);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 2);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 2);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 2);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 0);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 0);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 0.5);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), 0);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 0.5);
    }
}

TEST(PARSETest, TestShuntingYardNumberTypeMul) {
    std::string exp("a*b");
    ShuntingYard *sy = new ShuntingYard(exp);
    ASSERT_TRUE(sy->compile());
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new DoubleValue(10)),
            std::make_pair<const char*, Value*>("b",  new IntValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 50);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new DoubleValue(10)),
            std::make_pair<const char*, Value*>("b",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 50);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new DoubleValue(10)),
            std::make_pair<const char*, Value*>("b",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 50);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new DoubleValue(10)),
            std::make_pair<const char*, Value*>("b",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 50);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new DoubleValue(10)),
            std::make_pair<const char*, Value*>("a",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 50);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new DoubleValue(10)),
            std::make_pair<const char*, Value*>("a",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 50);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 50);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 50);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
            std::make_pair<const char*, Value*>("a",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 50);
    }
}

TEST(PARSETest, TestShuntingYardNumberTypeConstMul) {
    std::string exp("b*-100");
    ShuntingYard *sy = new ShuntingYard(exp);
    ASSERT_TRUE(sy->compile());
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(-10)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 1000);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new LongValue(-10)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 1000);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(-10)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 1000);
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new DoubleValue(-10)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 1000);
    }
}

TEST(PARSETest, TestShuntingYardNumberTypeConstPow) {
    std::string exp("b^10");
    ShuntingYard *sy = new ShuntingYard(exp);
    ASSERT_TRUE(sy->compile());
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), pow(10, 10));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new LongValue(10)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), pow(10, 10));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(10)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), pow(10, 10));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new DoubleValue(10)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), pow(10, 10));
    }
}

TEST(PARSETest, TestShuntingYardNumberTypePow1) {
    std::string exp("a^b");
    ShuntingYard *sy = new ShuntingYard(exp);
    ASSERT_TRUE(sy->compile());
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(2)),
            std::make_pair<const char*, Value*>("b",  new IntValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), pow(2, 5));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(2)),
            std::make_pair<const char*, Value*>("b",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), pow(2, 5));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(2)),
            std::make_pair<const char*, Value*>("b",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), pow(2, 5));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(2)),
            std::make_pair<const char*, Value*>("b",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), pow(2, 5));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(2)),
            std::make_pair<const char*, Value*>("a",  new LongValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), pow(5, 2));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(2)),
            std::make_pair<const char*, Value*>("a",  new FloatValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), pow(5, 2));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new IntValue(2)),
            std::make_pair<const char*, Value*>("a",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), pow(5, 2));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new DoubleValue(2)),
            std::make_pair<const char*, Value*>("a",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_int(env, log), pow(5, 2));
    }
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("b",  new FloatValue(2)),
            std::make_pair<const char*, Value*>("a",  new DoubleValue(5)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), pow(5, 2));
    }
}

TEST(PARSETest, TestShuntingYardNumberTypeConstMul1) {
    std::string exp("click *-100");
    ShuntingYard *sy = new ShuntingYard(exp);
    ASSERT_TRUE(sy->compile());
    {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("click",  new DoubleValue(0.244440)),
        };
        std::string log;
        EXPECT_DOUBLE_EQ(sy->eval_double(env, log), 0.244440*-100);
    }
}

TEST(PARSETest, TestShuntingYardConstStringCmp) {
    std::string exp("\"a\"==\"a\"");
    ShuntingYard *sy = new ShuntingYard(exp);
    ASSERT_TRUE(sy->compile());
    std::string log;
    std::vector<std::pair<const char*, Value*>> env;
    ASSERT_TRUE(sy->eval_bool(env, log));
}
