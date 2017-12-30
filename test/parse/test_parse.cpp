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
    };
    std::vector<std::string> exp_is_fail = {
        "1a",
        "1a.0",
        "\"abc",
        "abc\"",
        "1a",
        "-1",
        "+1",
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
        "1!=2",
        "1<=2",
        "1<2",
        "1.0<2.0",
        "1.0<=2.0",
        "not (1>2)",
        "1==1",
        "1.0==1.0",
        "\"a\"==\"a\"",
        "1.0!=2.0",
        "\"a\"!=\"b\"",
    };
    for(auto e : exps_is_true) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env ;
        ASSERT_TRUE(sy->eval_bool(env));
        delete sy;
    }
    std::vector<std::string> exps_diff_type_false = {
        "1<=2.0",
        "1<2.0",
        "1.0<2",
        "1.0<=2",
        "1>2.0",
        "1==1.0",
        "1.0==1",
        "\"a\"==1",
    };
    for(auto e : exps_diff_type_false) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env ;
        ASSERT_FALSE(sy->eval_bool(env));
        delete sy;
    }
    std::vector<std::string> exps_diff_type_true = {
        "1!=1.0",
        "1.0!=1",
        "\"a\"!=1",
    };
    for(auto e : exps_diff_type_true) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env ;
        ASSERT_TRUE(sy->eval_bool(env));
        delete sy;
    }
}

TEST(PARSETest, TestShuntingYardBuildComplicate) {
    std::vector<std::string> exps_is_true = {
        "1!=2 and 2>1 and \"a\"!=3 and \"a\" != \"b\"",
        "(1==2) or (1>0 and \"a\" == \"a\")",
        "(((((2.0>1.0)))))"
    };
    for(auto e : exps_is_true) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env ;
        ASSERT_TRUE(sy->eval_bool(env));
        delete sy;
    }

}

TEST(PARSETest, TestShuntingYardBuildVarible) {
    std::vector<std::string> exps_is_true = {
        "a!=1",
        "b!=2",
        "a>b",
    };
    for(auto e : exps_is_true) {
        std::string exp(e);
        ShuntingYard *sy = new ShuntingYard(exp);
        ASSERT_TRUE(sy->compile());
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new IntValue(10)),
            std::make_pair<const char*, Value*>("b",  new IntValue(5)),
        };
        ASSERT_TRUE(sy->eval_bool(env));
        delete sy;
    }

}
