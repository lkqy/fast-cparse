#include <gtest/gtest.h>
#include "libparse2/compile.h"
#include <thread>
#include <google/dense_hash_map>
using namespace libparse2;

TEST(PARSETest, Testxxxx) {
    std::cout<<"int64s:"<<typeid(std::vector<int64_t>).hash_code()<<"\n";
    std::cout<<"int64:"<<typeid(int64_t).hash_code()<<"\n";
    std::cout<<"int64:"<<typeid(int64_t).hash_code()<<"\n";
    std::cout<<std::numeric_limits<int64_t>::max()<<" max\n";
}

TEST(PARSETest, TestOpProcess) {
    std::vector<std::string> exps_is_16 = {"2^3 * 2", "3^2 * 2 - 2", "(2^3) * 2", "3^3/2+3"};
    Compile compile;
    for (const auto& e : exps_is_16) {
        std::cout<<e<<"\n";
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        env->clear();
        float ret;
        ASSERT_TRUE(env->eval(ret));
        ASSERT_EQ(ret, 16.0f);
    }
}

TEST(PARSETest, TestCompileGetVar) {
    std::vector<std::string> exps_is_one = {"a=1; b=a+1; c=b*2-1", "a=0+1; b=2-2+a+1; c=a+2"};
    Compile compile;
    for (const auto& e : exps_is_one) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        env->clear();
        float ret;
        env->eval(ret);
        ASSERT_TRUE(env->get("a", ret));
        ASSERT_TRUE(ret == 1);
        ASSERT_TRUE(env->get("b", ret));
        ASSERT_TRUE(ret == 2);
        ASSERT_TRUE(env->get("c", ret));
        ASSERT_TRUE(ret == 3);
        ASSERT_FALSE(env->get("d", ret));
    }
}

TEST(PARSETest, TestCompileGetEmptyVar) {
    std::vector<std::string> exps_is_one = {"1+2-1+1", "1+2"};
    Compile compile;
    for (const auto& e : exps_is_one) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        env->clear();
        float ret;
        env->eval(ret);
        ASSERT_TRUE(env->get("", ret));
        ASSERT_TRUE(ret == 3);
        ASSERT_FALSE(env->get("d", ret));
    }
}

TEST(PARSETest, TestCompileGetVarArray) {
    std::vector<std::string> exps_is_one = {"a=x; b=a+1; c=b*2-1", "a=0+1; b=2-2+a+1; c=a+2"};
    Compile compile;
    for (const auto& e : exps_is_one) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        env->clear();
        std::vector<float> x = {1.0f, 1.0f, 1.0f};
        env->add("x", x);
        std::vector<float> ret;
        ASSERT_TRUE(env->eval(ret));
        ret.clear();
        ASSERT_TRUE(env->get("a", ret));
        ASSERT_TRUE(ret.size() == x.size());
        ASSERT_TRUE(ret[0] == 1);
        ret.clear();
        ASSERT_TRUE(env->get("b", ret));
        ASSERT_TRUE(ret.size() == x.size());
        ASSERT_TRUE(ret[0] == 2);
        ret.clear();
        ASSERT_TRUE(env->get("c", ret));
        ASSERT_TRUE(ret.size() == x.size());
        ASSERT_TRUE(ret[0] == 3);
        ASSERT_FALSE(env->get("d", ret));
    }
}

TEST(PARSETest, TestTERNARY) {
    std::unordered_map<std::string, double> exps_values = {
            // {"(a > 0) ?(c7_1=1;c7_2=12) :(c7_1=-1;c7_2=-1)", 12},
            {"1 ? a : b", 15},
            {"0 ? b : a", 15},
            {" (1 > 0) ? a : b", 15},
            {" (0 > 0) ? b : a", 15},
            {"(a>b) ? a : b", 15},
            {"(a<b) ? b : a", 15},
            {"(a<(b+1)) ? b : a", 15},
            {"(a-1<(b+1)) ? b : a", 15},
            {"(a-1<(b+1)) ? b : a-5", 10},
            {"(a-1<(b+1)) ? b : (a + 1 -1) * 1.0", 15},
            {"(a-1>(b+1)) ? 15:1", 15},
            {"(a>1 and 1 > 0) ? 15:1", 15},
            {"(1 > 0 or a > 1) ? 15:1", 15},
            {"(1 > 0 or a < 1) ? 15:1", 15},
            {"(1 > 2 and a < 1) ? 15:25", 25},
            {"c7_1=(a>0 ? 15 : -1)", 15},
            {"c7_1=((a>0) ? 15 : -1)", 15},
            {"c7_1=((a>0) ? 15 : -1); c_72=(a<0 ? 10 : 11)", 11},
            {"c7_1=((a>0) ? 15 : -1); c_72=(a<0 ? 10.0 : a)", 15},
            {"c7_1=((a>0) ? 15.0 : a); c_72=(a<0 ? 10.0 : a)", 15},
            {"c=(b > 0 ? a*a : 0.0)", 225},
            {"c=(b < 0 ? a*a : 0.0)", 0},
            {"c=(b < 0 ? a*a : 0)", 0},
            {"c=(b < 0 ? a*a : 0)", 0},
    };
    Compile compile;
    for (auto e : exps_values) {
        {
            double v = e.second;
            auto env = compile.get(e.first);
            ASSERT_TRUE(env->is_valid());
            std::vector<double> a = {15.0, 15.0, 15.0};
            env->add("a", a);
            env->add("b", 5.0);
            std::vector<int32_t> b;
            ASSERT_TRUE(env->eval(b));
            ASSERT_TRUE(b.size() == a.size());
            for (size_t i = 0; i < a.size(); ++i) {
                ASSERT_TRUE(b[i] == e.second);
            }
        }
    }
}

TEST(PARSETest, TestArrayErrorSizeResult) {
    const size_t vec_size = 10;
    const size_t vec_size_1 = 5;
    const size_t vec_size_2 = 2;
    std::vector<std::string> exps_1 = {
            {"a/b"}, {"a-c"},     {"a+c"},     {"b+c"},   {"b+a - b"}, {"a*b"},
            {"a^b"}, {"a-b+c+a"}, {"c*b + a"}, {"a-d+b"}, {"d+b-a"},
    };
    Compile compile;
    for (auto e : exps_1) {
        std::cout << e << "\n";
        auto env = compile.get(e);
        std::vector<double> a(vec_size, 1.0);
        std::vector<double> b(vec_size_1, 1.0);
        std::vector<double> c(vec_size_2, 0.0);
        env->add("a", a);
        env->add("b", b);
        env->add("c", c);
        env->add("d", 1);
        std::vector<double> ret;
        ASSERT_TRUE(env->is_valid());
        ASSERT_FALSE(env->eval(ret));
    }
}

TEST(PARSETest, InvalidExpression) {
    Compile compile;
    std::vector<std::string> exps = {
            "abc#xxldk", "abc-", "10.-",
    };
    for (auto e : exps) {
        auto env = compile.get(e);
        ASSERT_TRUE(env != nullptr);
        ASSERT_FALSE(env->is_valid());
    }
}

TEST(PARSETest, ArrayIn) {
    Compile compile;
    std::vector<std::string> exps = {
            "a in SET(1, 2, 3)", "b in SET(1.0, 2.0, 3.0)", "c in SET('a', 'b', 'c')",
    };
    for (auto e : exps) {
        auto env = compile.get(e);
        std::vector<int> a = {1, 2, 3};
        env->add("a", a);
        std::vector<float> b = {1.0f, 2.0f, 3.0f};
        env->add("b", b);
        std::vector<std::string> c = {"a", "b", "c"};
        env->add("c", c);
        ASSERT_TRUE(env != nullptr);
        ASSERT_TRUE(env->is_valid());
        std::vector<bool> result;
        ASSERT_TRUE(env->eval(result));
        ASSERT_EQ(result.size(), 3);
        for (auto r : result) {
            ASSERT_TRUE(r);
        }
    }
    for (auto e : exps) {
        auto env = compile.get(e);
        std::vector<int> a = {11, 21, 31};
        env->add("a", a);
        std::vector<float> b = {12.0f, 22.0f, 32.0f};
        env->add("b", b);
        std::vector<std::string> c = {"a1", "b1", "c1"};
        env->add("c", c);
        ASSERT_TRUE(env != nullptr);
        ASSERT_TRUE(env->is_valid());
        std::vector<bool> result;
        ASSERT_TRUE(env->eval(result));
        ASSERT_EQ(result.size(), 3);
        for (auto r : result) {
            ASSERT_FALSE(r);
        }
    }
}

TEST(PARSETest, TestDataSource1) {
    Compile compile;
    typedef std::function<Value*(const std::unordered_map<std::string, float>&, const std::string& name)> Func;
    Func f = [](const std::unordered_map<std::string, float>& m, const std::string& name) -> Value* {
        if (m.find(name) == m.end())
            return nullptr;
        return new FloatValue(m.at(name));
    };

    std::unordered_map<std::string, float> data = {
            {"a", 1.0f}, {"b", 2.0f}, {"c", 3.0f},
    };

    for (size_t i = 0; i < 10000000; ++i) {
        auto env = compile.get("a+b+c");
        env->set_data_func<std::unordered_map<std::string, float>>(f);
        env->add_data_source(data);
        float result = 0;
        ASSERT_TRUE(env->is_valid());
        ASSERT_TRUE(env->eval(result));

        ASSERT_EQ(result, 6.0f);
    }
    for (size_t i = 0; i < 10000000; ++i) {
        auto env = compile.get("a+b+c");
        env->set_data_func<std::unordered_map<std::string, float>>(f);
        data.clear();
        env->add_data_source(data);
        float result = 0;
        ASSERT_TRUE(env->is_valid());
        ASSERT_FALSE(env->eval(result));
        ASSERT_EQ(result, 0.0f);
    }

    for (size_t i = 0; i < 10000000; ++i) {
        auto env = compile.get("a+b+c");
        env->add_data_source(data);
        float result = 0;
        ASSERT_TRUE(env->is_valid());
        ASSERT_FALSE(env->eval(result));
    }
}

TEST(PARSETest, TestArrayInResult) {
    std::unordered_map<std::string, std::vector<bool>> exps = {
            {"a == b", {true, true, true}},
            {"c == d", {true, false, false}},
            {"c in d", {true, true, true}},
            {"c in e", {true, false, false}},
    };
    Compile compile;
    for (auto e : exps) {
        auto env = compile.get(e.first);
        std::vector<int32_t> a = {1, 2, 3};
        std::vector<int32_t> b = {1, 2, 3};
        std::string c = "c";
        std::vector<std::string> d = {"c", "cd", "ec"};
        std::vector<std::string> e1 = {"c", "d", "e"};
        env->add("a", a);
        env->add("b", b);
        env->add("c", c);
        env->add("d", d);
        env->add("e", e1);
        ASSERT_TRUE(env->is_valid());
        std::vector<bool> ret;
        ASSERT_TRUE(env->eval(ret));
        std::cout << "exp:" << e.first << "\n";
        for (size_t i = 0; i < ret.size(); ++i) {
            ASSERT_EQ(ret[i], e.second[i]);
        }
    }
}

TEST(PARSETest, TestCompileCompile1) {
    std::unordered_map<std::string, bool> exps_values = {
            {"not ((msg_name == 'digg') or ((msg_name == 'follow') and fans_cnt > 10000))", true},
            {"not ((msg_name == 'digg'))", true},
            {"(msg_name == 'digg') ", false}};
    Compile compile;
    for (auto e : exps_values) {
        {
            auto env = compile.get(e.first);
            ASSERT_TRUE(env->is_valid());
            env->add("fans_cnt", 0);
            env->add("msg_name", "follow");
            bool v;
            ASSERT_TRUE(env->eval(v));
            ASSERT_EQ(v, e.second);
        }
    }
}
TEST(PARSETest, TestSpeedMap) {
    size_t count = 10000;
    size_t batch = 100;
    {
        clock_t start = clock();
        for (size_t i = 0; i < count; ++i) {
            google::dense_hash_map<std::string, size_t> m;
            m.set_empty_key(std::string(""));
            for (size_t j = 0; j < batch; ++j) {
                m[std::to_string(j)] = j;
            }
        }
        clock_t end = clock();
        std::cout << "parse cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
    }
    {
        clock_t start = clock();
        for (size_t i = 0; i < count; ++i) {
            std::map<std::string, size_t> m;
            for (size_t j = 0; j < batch; ++j) {
                m[std::to_string(j)] = j;
            }
        }
        clock_t end = clock();
        std::cout << "parse cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
    }
    {
        clock_t start = clock();
        for (size_t i = 0; i < count; ++i) {
            std::unordered_map<std::string, size_t> m;
            for (size_t j = 0; j < batch; ++j) {
                m[std::to_string(j)] = j;
            }
        }
        clock_t end = clock();
        std::cout << "parse cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
    }
}

TEST(PARSETest, TestCompileVistQueue) {
    class Test : public Compile {
    public:
        Test() {
        }
        bool check(const std::string& e, std::queue<std::pair<std::string, int>>& q, std::string& l) {
            auto r = build_visit_queue(e, q, l);
            return r;
        }
    };
    Test test;
    std::vector<std::string> exps = {"F(1, 2, 4, 3)",
                                     "G('1', 'a' in 'ab')",
                                     "H('1', 'a' in 'ab', SET('a', 'b'))",
                                     "I('1,2,3,4', 'a' in 'ab')",
                                     "I('1,2,3,4', 'a' in 'ab', (a, b))",
                                     "I('1,2,3,4', 'a' in 'ab', H(a, b), I('a' > 'b,c', 1)",
                                     "A()",
                                     "a",
                                     "0",
                                     "''",
                                     "a?b:c",
                                     "a+b?c+d:e*f"};
    for (auto e : exps) {
        std::queue<std::pair<std::string, int>> q;
        std::string log;
        test.check(e, q, log);
        while (not q.empty()) {
            auto p = q.front();
            q.pop();
        }
    }
}

TEST(PARSETest, TestVaribles) {
    std::vector<std::string> exps = {"a+b+c", "F(a, b) + c", "F(a, b) -c * L()", "F(a, b, c) + H()"};

    std::unordered_set<std::string> vars = {"a", "b", "c"};
    Compile compile;
    for (auto e : exps) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        const auto& _vars = env->varibles();
        for (auto& s : _vars) {
            ASSERT_TRUE(vars.find(s) != vars.end());
        }
        for (auto& s : vars) {
            ASSERT_TRUE(_vars.find(s) != _vars.end());
        }
    }
}

TEST(PARSETest, TestIsPodType) {
    ASSERT_EQ(is_pod_type<int32_t>::value, true);
    ASSERT_EQ(is_pod_type<int64_t>::value, true);
    ASSERT_EQ(is_pod_type<bool>::value, true);
    ASSERT_EQ(is_pod_type<float>::value, true);
    ASSERT_EQ(is_pod_type<double>::value, true);
    ASSERT_EQ(is_pod_type<std::string>::value, true);
    ASSERT_EQ(is_pod_type<const char*>::value, false);
    ASSERT_EQ(is_pod_type<std::vector<int32_t>>::value, true);
    ASSERT_EQ(is_pod_type<std::vector<int64_t>>::value, true);
    ASSERT_EQ(is_pod_type<std::vector<bool>>::value, true);
    ASSERT_EQ(is_pod_type<std::vector<float>>::value, true);
    ASSERT_EQ(is_pod_type<std::vector<double>>::value, true);
    ASSERT_EQ(is_pod_type<std::vector<const char*>>::value, false);
    ASSERT_EQ(is_pod_type<std::vector<std::string>>::value, true);
    struct A {};
    class B {};
    ASSERT_EQ(is_pod_type<A>::value, false);
    ASSERT_EQ(is_pod_type<B>::value, false);
    ASSERT_NE(typeid(int*).hash_code(), typeid(char*).hash_code());
    ASSERT_NE(typeid(int*).hash_code(), typeid(long*).hash_code());
    ASSERT_NE(typeid(int*).hash_code(), typeid(float*).hash_code());
    ASSERT_NE(typeid(double*).hash_code(), typeid(float*).hash_code());
    ASSERT_NE(typeid(double*).hash_code(), typeid(void*).hash_code());
}

TEST(PARSETest, TestCompileCompile) {
    std::vector<std::string> exp_is_ok = {
            "1",
            "1e10",
            "-1e10",
            "-1e+10",
            "-1e-10",
            "1.0e10",
            "(fi*0.2+1)*(li*0.2+1)",
            "1.0e+10",
            "1.0e-10",
            "+1.0e-10",
            "-1.0e-10",
            "-1.e-10",
            "+1.e-10",
            "1++1.e-10",
            "1++1.e+10",
            "1-+1.e+10",
            "1- +1.e+10",
            "1>0?1:0",
            "a>b?c:d",
            "a>b?c:0",
            "a>b?c: 0",
            "a>b?c : 0",
            "a+b?c : 0",
            "a+e?c : 0",
            "a:b?c",
            "a+e? c : 1+0",
            "(2 < 1 or 1 < 1.5) ? 1 : 0",
            "2 < 1 or 1 < 1.5 ? 1 : 0",
            "a+b?",  // 这个语法是不是太trick了？
            "1- +1.e-10",
            "1- +0.e-10",
            "+1.-e-10.",
            "+1.+e-10.",
            "1.0",
            "\"abc\"",
            "'abc'",
            "'abc\"\"\"\"xab'",
            "\"abc''''ab\"",
            "a==11",
            "-1",
            "-1.0",
            "-1.02",
            "+1",
            "+1.0",
            "+1.0+1.0",
            "+1.0+1.0/3.0",
            "\"a\"==\"a\""
            "+1.0+1.0/3.0+-1.0",
            "(1.0+3.0/3.0)-1.0",
            "1.0^1.0",
            "_CTR-1.0",
            "b=1",
            "b=1; a=b; c=a",
            "b=1; a=b",
            "_CTR- 1.0",
            "_CTR -1.0",
            "_CTR + -1.0",
            "_CTR+ -1.0",
            "_CTR +-1.0",
            "(_CTR) -1.0",
            "(_CTR)-1.0",
            "(_CTR) +1.0",
            "(_CTR)+1.0",
            "local_hour in SET(10, 11, 12)",
            "1; 2",
            "1;2",
            "1;2.0",
            "1;'abc'",
            "1 ;'abc'",
            "1 ;('abc')",
            "1 ;('abc' > 'aef')",
            "IsFollow(uid, GetMsgUserId(ctx, 'predict_all', index)) ? 2 :1",
            "F1(a, b)",
            "F1(a, -1)",
            "F1(a,-1)",
            "F1(1,-1)",
            "F1(-1,-1)",
            "F1(-1, -1)",
            "F1( -1, -1, 2)",
            "F1( -1, -1,-2)",
    };
    std::vector<std::string> exp_is_fail = {"",
                                            "1a",
                                            "1a.0",
                                            "a not in SET(1, 2, 3)",
                                            "\"abc",
                                            "'' in SET()",
                                            "SET()",
                                            "abc\"",
                                            "1a",
                                            "a?b",
                                            "a:b",
                                            "a+b?c",
                                            "a+b:",
                                            "b:=1",
                                            "b:=1; a:=b; c:=a",
                                            "b:=1; a:=b",
                                            "+1.e-10.",
                                            "+1.e+10.",
                                            "+1.e-10.",
                                            "+1.e+10.",
                                            "1- +.e-10",
                                            "_CTR +- 1.0",
                                            "_CTR+- 1.0",
                                            "_CTR+ - 1.0",
                                            "_CTR-+ 1.0",
                                            "_CTR- + 1.0",
                                            "_CTR - + 1.0",
                                            "_CTR - + 1.0;",
                                            "_CTR - + 1.0 ;",
                                            "(_CTR - + 1.0) ;",
                                            "-a",
                                            "-a;",
                                            "+a",
                                            "a:=",
                                            ":=a",
                                            "'abc\"",
                                            "\"abc\'",
                                            "+\"1\"",
                                            "+\"1\";",
                                            "(like+follow",
                                            "(like+follow))",
                                            "like+follow))",
                                            "like+follow)"};
    Compile compile;
    for (auto e : exp_is_ok) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
    }
    for (auto e : exp_is_fail) {
        auto env = compile.get(e);
        if (env->is_valid()) {
            std::cout << e << "\n";
        }
        ASSERT_FALSE(env->is_valid());
    }
}

TEST(PARSETest, TestEvalFailed) {
    std::vector<std::string> exp_is_fail = {
            "a+b", "a=b", "a*b",
    };
    Compile compile;
    for (auto e : exp_is_fail) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        bool b;
        ASSERT_FALSE(env->eval(b));
    }
}

TEST(PARSETest, TestCompileArrayConstOp) {
    std::vector<std::string> exps_is_one = {
            "ARRAY(1, 1, 1)", "ARRAY(0, 0, 0) + 1",
    };
    Compile compile;
    for (const auto& e : exps_is_one) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        for (size_t i = 0; i < 3; ++i) {
            env->clear();
            std::vector<float> ret;
            ASSERT_TRUE(env->eval(ret));

            ASSERT_TRUE(ret.size() == 3);
            ASSERT_TRUE(ret[0] == 1);
        }
    }
}

TEST(PARSETest, TestCompileEvalBool) {
    std::vector<std::string> exps_is_true = {
            "1",
            "1.0",
            "'' in ''",
            "'' in 'a'",
            "1>0?1:0",
            "1.0 and 1.0",
            "1.0 or 1.0",
            "0 or 2.0",
            "1>0 or 1>0 and 1<0",
            "1>0 or 1<0 and 1<0",
            "not 0",
            "1<=2.0",
            "(1>0) or (2>0) or (3>4)",
            "A(B(C(D(E(1, 2), 2), 3), 4), 5, 6)",
            "2 < 1 or 1 < 1.5 ? 1 : 0",
            "(2 < 1 or 1 < 1.5) ? 0 : 1",
            "2 < 1 or 1 < 1.5 ? 0 : 1",
            "A(1, B(C(D(E(1, 2), 2), 3), 4), 5, 6)",
            "A(1, SET('1', '2', '3'), B(C(D(E(1, 2), 2), 3), 4), 5, 6)",
            "A(SET('1', '2', '3'), B(C(D(E(1, 2), 2), 3), 4), 5, 6)",
            "A('3', B(C(D(E(1, 2), 2), 3), 4), 5, 6)",
            "A('3', B(C(D(E(1, 2), 2), 3), 4), 5, 6) ? 1:0",
            "A('3', B(C(D(E(1, 2), 2), 3), 4), 5, 6) ? 1: 1+2",
            "A('3', B(C(D(E(1, 2), 2), 3), 4), 5, 6) ? 2*4: 1+2",
            "(-1>0) or (2>0) or (3>4)",
            "(-1>0) or (-2>0) or (3>2)",
            "(1>0) and (2>0)",
            "(1>0) and (2>0)",
            "GetSlot(slots, 2, 128) * 0.4+ user_embedding",
            "1 > 0 and 2 > 0",
            "-1 < 0 and 2 > 0",
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
            "1<=1.0",
            "1>=1.0",
            "1.0<2.0",
            "-1.0<2.0",
            "1.0<=2.0",
            "-1.0<=2.0",
            "not (1>2)",
            "not (-1>2)",
            "1==1",
            "2.0>1",
            "2.0<1; 2.0 > 1",
            "2.0>=1",
            "-2.0<-1",
            "-2.0<=-1",
            "1.0==1",
            "1.0==1.0",
            "1==1.0",
            "-1==-1.0",
            "'abc' in 'abcd'",
            "-1.0==-1.0",
            "\"a\"==\"a\"",
            "\"a\"!=\"a\"; 'a' == 'a'",
            "1.0!=2.0",
            "'a' in SET('a')",
            "'a' in SET('b', 'a', 'c')",
            "'' in SET('')",
            "1.0==2.0; 3.0==4.0; 5.0==6.0; 7.0==8.0; 9.0==10.0; 11.0!=12.0",
            "-1.0!=-2.0",
            "not ((msg_name == 'digg') or (msg_name == 'follow' and fans_cnt > 10000))",
            "-1.0==-1",
            "'a' != 'b'",
            "'a' == 'a'",
            "\"a\"!=\"b\"",
            "\"a\"==\"a\"",
            "a==0.0",
            "1!=2 and 2>1 and \"a\"!=3 and \"a\" != \"b\"",
            "(1==2) or (1>0 and \"a\" == \"a\")",
            "(1==2) or (1>0 and \"a\" == 'a')",
            "(((((2.0>1.0)))))",
            "1>0;(((((2.0>1.0)))))",
            "1>0 ;(((((2.0>1.0)))))",
            "1>0 ; (((((2.0>1.0)))))",
            "(1>0);(((((2.0>1.0)))))",
            "a=1;(((((2.0>1.0)))))",
            "(((((2.0>-1.0)))))"

    };
    std::vector<std::string> exps_is_false = {"0",
                                              "0.0",
                                              "0.0 and 1",
                                              "0.0 and 11",
                                              "1>0?0:1",
                                              "0.0 or 0.0",
                                              "not 1.0",
                                              "not 11",
                                              "1>2.0",
                                              "-1>2.0",
                                              "1>2",
                                              "-1>2",
                                              "-1==1.0",
                                              "1==-1.0",
                                              "\"a\" != \"a\"",
                                              "1.0==-1",
                                              "-1.0!=-1",
                                              "a=(1>0); b=(1<0)",
                                              "(2 < 1 or 1 < 1.5) ? 0 : 1",
                                              "1>0; -1.0!=-1",
                                              "1!=1.0",
                                              "1>=1.0; 1!=1.0",
                                              "a=1; a!=1.0",
                                              "1<=1.0; 1!=1.0",
                                              "1.0!=1",
                                              "\"a\"==1",
                                              "a!=0.0"};
    std::vector<std::string> exps_diff_type_true = {
            "\"a\" != \"b\"",      "\"a\" != \"b\" and 1!=2",
            "'a' != 'b' and 1!=2", "(\"a\" != \"b\") and (1!=2)",
            "not (1!=1)",          "-1.0!=1",
            "-1.0==1; -1.0!=1",    "b=1.0; b==1",
            "a=-1.0; a!=1",        "\"a\"!=1",
    };
    std::vector<std::string> exps_equal_1 = {
            "1.0",
            "1",
            "1.0+0",
            "1.0-0.0",
            "1e0",
            "1e+0",
            "1>0?1:0",
            "1>0?1.0:0",
            "1.0>0?1.0:0",
            "1.0>0?1.0+0:0",
            "1e-0",
            "1e-00",
            "1e+00",
            "1.000e+00",
            "1.000e-00",
            "+1.000e-00",
            "+1.000e-00-0",
            "+1.000e-00+0",
            "0.0+1.0",
            "0.0==0.0",
            "1.0!=0.0",
            "(0.0==0.0 or 1.0!=0.0)",
            "(0.0==0.0 or 1.0!=0.0) and 1.0 > 0.0",
    };
    std::vector<std::string> exps_equal_0 = {
            "0.0",
            "0",
            "0e0",
            "0e+0",
            "0e-0",
            "1.0>0?0.0+0:2",
            "2.0>0?0.0+0:2",
            "0e-00",
            "0e+00",
            "0.000e+00",
            "0.000e-00",
            "+0.000e-00",
            "+0.000e-00-0",
            "+0.000e-00+0",
            "+1.000e-00-1.000e-00",
            "+1.000e-00+ -1.000e-00",
            "0.0+0",
            "0.0-0.0",
            "0.0+0.0",
            "0.0!=0.0",
            "1.0==0.0",
            "(0.0!=0.0 or 1.0==0.0)",
            "(0.0!=0.0 or 1.0==0.0) and 0.0 > 0.0",
            "a>100",
            "a-10",
            "a-10.0",
            "a<9.0 and 1<0",
            "is_cold!=0 or is_asia!=0",
            "is_cold or is_asia",
            "is_asia!=0",
            "(is_cold==0.0 or cold_skip) and skip",
            "(not is_cold or cold_skip) and skip",
            "(is_cold==0.0 or cold_skip!=0.0) and 0!=0",
            "(not is_cold or cold_skip) and 0",
            "(is_cold==0.0 or cold_skip!=0.0) and skip!=0.0",
            "(is_cold==1.0 or cold_skip!=0.0) and skip!=0.0",
    };
    Compile compile;
    clock_t start = clock();
    const size_t loop = 1000;
    size_t count = 0;
    for (const auto& e : exps_equal_0) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        for (size_t i = 0; i < loop; ++i) {
            count += 1;
            env->clear();
            env->add("a", 10);
            env->add("is_cold", 0);
            env->add("is_asia", 0);
            env->add("cold_skip", 1);
            env->add("skip", 0);
            double b = 0.111;
            ASSERT_TRUE(env->eval(b));
            if (b != 0) {
                std::cout << "exp:" << e << ", b:=" << b << "\n";
            }
            ASSERT_TRUE(b == 0.0);
        }
        count += 1;
        env->clear();
        env->add("a", 10);
        env->add("is_cold", 0);
        env->add("is_asia", 0);
        env->add("cold_skip", 1);
        env->add("skip", 0);
        env->disable_short_circuit();
        double b = 0.111;
        ASSERT_TRUE(env->eval(b));
        if (b != 0) {
            std::cout << "exp:" << e << ", b:=" << b << "\n";
        }
        ASSERT_TRUE(b == 0.0);
    }
    for (const auto& e : exps_is_true) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        for (size_t i = 0; i < loop; ++i) {
            env->reset();
            count += 1;
            env->add("a", 0.0);
            env->add("msg_name", "follow");
            env->add("fans_cnt", 0);
            env->add("random", 20);
            bool b;
            env->eval(b);
        }
        env->reset();
        env->disable_short_circuit();
        count += 1;
        env->add("a", 0.0);
        env->add("msg_name", "follow");
        env->add("fans_cnt", 0);
        env->add("random", 20);
        bool b;
        env->eval(b);
    }
    for (const auto& e : exps_is_false) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        for (size_t i = 0; i < loop; ++i) {
            count += 1;
            env->reset();
            env->add("a", 0.0);
            bool b;
            ASSERT_TRUE(env->eval(b));
            ASSERT_FALSE(b);
        }
    }
    for (const auto& e : exps_diff_type_true) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        for (size_t i = 0; i < loop; ++i) {
            count += 1;
            env->reset();
            bool b;
            ASSERT_TRUE(env->eval(b));
            ASSERT_TRUE(b);
        }
    }
    for (const auto& e : exps_equal_1) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        for (size_t i = 0; i < loop; ++i) {
            count += 1;
            env->reset();
            double b;
            ASSERT_TRUE(env->eval(b));
            ASSERT_TRUE(b == 1.0);
        }
    }
    clock_t end = clock();
    std::cout << "loop:" << loop << ", count:" << count << ", cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
}

TEST(PARSETest, TestCompileComplicateCase) {
    std::vector<std::string> exps_is_true = {
            "a!=1",
            "b!=2",
            "not( lang in SET(\"ja\", \"vi\", \"in\"))",
            "a>b",
            "c in SET(5)",
            "5 in SET(5)",
            "b in SET(5)",
            "1>2;a>b",
            "1>0? 2 > 0 : 1>0",
            "1>0? 2 > 0 : a!=1",
            "0>0? 2 > 0 : a!=1",
            "1>0? 2 > 0 : a==1",
            "local_hour in SET(10, 11, 12)",
            "2!=2;local_hour in SET(10, 11, 12)",
            "lang == \"jp\"",
            "(lang == \"jp\" and tz_hour in SET(8, 12, 18))",
            "(lang == \"jp\" and tz_hour in SET(8, 12, 18)) or (lang==\"us\" and tz_hour in SET(11, "
            "15, 21)) or not (lang in SET(\"jp\", \"us\"))",
            "(lang == 'jp' and tz_hour in SET(8, 12, 18)) or (lang=='us' and tz_hour in SET(11, 15, "
            "21)) or not (lang in SET('jp', 'us'))",
            "(1>2);(lang == 'jp' and tz_hour in SET(8, 12, 18)) or (lang=='us' and tz_hour in SET(11, "
            "15, 21)) or not (lang in SET('jp', 'us'))",
            "(1>2);(lang == \"jp\" and tz_hour in SET(8, 12, 18)) or (lang==\"us\" and tz_hour in "
            "SET(11, 15, 21)) or lang != \"jp\"",
            "not (lang != \"jp\")",
            "(1>2);(lang == \"jp1\" and tz_hour in SET(8, 12, 18)) or (lang==\"us\" and tz_hour in "
            "SET(11, 15, 21)) or not (lang in SET(\"jp1\", \"us\"))",
            "(lang == \"jp1\" and tz_hour in SET(8, 12, 18)) or (lang==\"us\" and tz_hour in SET(11, "
            "15, 21)) or not (lang in SET(\"jp1\", \"us\"))",
            "(lang == 'jp1' and tz_hour in SET(8, 12, 18)) or (lang=='us' and tz_hour in SET(11, 15, "
            "21)) or not (lang in SET('jp1', 'us'))",
            "(region in SET('in') and (type_last_send > 1 or type_send_cnt >6))",
            "(region in SET('in') and (type_last_send > 1 or type_send_cnt >6)) or (type_last_send > 2 or type_send_cnt>20)",
            "((not user_vip) and ( (region in SET('in') and (type_last_send > 1 or type_send_cnt >6)) or (type_last_send> 2 or type_send_cnt >20)) )",
            "1>0  and province_id in SET(31) and carrier in SET('CUCC')",
            "((not user_vip) and ( (region in SET('in') and (type_last_send > 1 or type_send_cnt >6)) or(type_last_send> 2 or type_send_cnt >20)) ) or app_name == 'flipagram'"};

    std::vector<std::string> exps_is_false = {
            "'abcd' in 'abc'",
            "'a' in ''",
            "1>0? 'a' in '':1",
            "0>0? 'a' in '':0",
            "10>0? 0:'a' in ''",
            "10<0? 0:'a' in ''",
            "'' in SET('a')",
            "'' in SET('a')",
            "(region in SET('in') and (type_last_send > 1 or type_send_cnt >6))",
            "(region in SET('in') and (type_last_send > 1 or type_send_cnt >6)) or (type_last_send > 2 or type_send_cnt>20)",
            "((not user_vip) and ( (region in SET('in') and (type_last_send > 1 or type_send_cnt >6)) or (type_last_send> 2 or type_send_cnt >20)) )",
            "((not user_vip) and ( (region in SET('in') and (type_last_send > 1 or type_send_cnt >6)) or (type_last_send> 2 or type_send_cnt >20)) ) or app_name == 'flipagram'"};
    Compile compile;
    for (auto e : exps_is_true) {
        {
            auto env = compile.get(e);
            ASSERT_TRUE(env->is_valid());
            env->add("a", 10L);
            env->add("b", 5L);
            int32_t c = 5;
            env->add("c", c);
            env->add("local_hour", 11L);
            env->add("tz_hour", 12L);
            env->add("lang", "jp");
            env->add("app_name", "vigo");
            env->add("type_last_send", 2L);
            env->add("type_send_cnt", 4L);
            env->add("user_vip", false);
            env->add("region", "in");
            env->add("carrier", "CUCC");
            env->add("province_id", 31L);
            ASSERT_TRUE(env->is_valid());
            bool b;
            ASSERT_TRUE(env->eval(b));
            ASSERT_TRUE(b);
        }
        {
            auto env = compile.get(e);
            ASSERT_TRUE(env->is_valid());
            env->add("a", 10L);
            env->add("b", 5L);
            int32_t c = 5;
            env->add("c", c);
            env->add("local_hour", 11L);
            env->add("tz_hour", 12L);
            env->add("lang", "jp");
            env->add("app_name", "vigo");
            env->add("type_last_send", 2L);
            env->add("type_send_cnt", 4L);
            env->add("user_vip", false);
            env->add("region", "in");
            env->add("carrier", "CUCC");
            env->add("province_id", 31L);
            env->disable_short_circuit();
            ASSERT_TRUE(env->is_valid());
            bool b;
            ASSERT_TRUE(env->eval(b));
            ASSERT_TRUE(b);
        }
    }
    for (auto e : exps_is_false) {
        auto env = compile.get(e);
        ASSERT_TRUE(env->is_valid());
        env->add("app_name", "vigo");
        env->add("type_last_send", 1L);
        env->add("type_send_cnt", 6L);
        env->add("user_vip", false);
        env->add("region", "in");
        bool b;
        ASSERT_TRUE(env->eval(b));
        ASSERT_FALSE(b);
    }
}

#define CHECK_RESULT_TRUE(env, type, v) \
    {                                   \
        type b;                         \
        ASSERT_TRUE(env->eval(b));      \
        ASSERT_EQ(b, type(v));          \
    }

#define CHECK_RESULT_FALSE(env, type, v) \
    {                                    \
        type b;                          \
        ASSERT_TRUE(env->eval(b));       \
        ASSERT_NE(b, type(v));           \
    }

TEST(PARSETest, TestArrayOpResult) {
    const size_t vec_size = 10;
    std::vector<std::string> exps_1 = {
            {"a/b"},     {"a-c"}, {"a+c"}, {"b+c"},     {"b+a - b"}, {"a*b"}, {"a^b"},
            {"a-b+c+a"}, {"c+d"}, {"d+c"}, {"c*b + a"}, {"a-d+b"},   {"a*d"}, {"d+b-a"},
    };
    Compile compile;
    for (auto e : exps_1) {
        auto env = compile.get(e);
        std::vector<double> a(vec_size, 1.0);
        std::vector<double> b(vec_size, 1.0);
        std::vector<double> c(vec_size, 0.0);
        env->add("a", a);
        env->add("b", b);
        env->add("c", c);
        env->add("d", 1);
        std::vector<double> ret;
        ASSERT_TRUE(env->is_valid());
        ASSERT_TRUE(env->eval(ret));
        for (size_t i = 0; i < vec_size; ++i) {
            if (ret[i] != 1) {
                std::cout << "exp:" << e << "\n";
            }
            ASSERT_EQ(ret[i], 1);
        }
    }
}

TEST(PARSETest, TestString) {
    std::unordered_map<std::string, std::string> exps_values = {
            {"'a' + 'b'", "ab"}, {"'a' + ''", "a"}, {"'' + 'ab'", "ab"}, {"'' + ''", ""}, {"'abc' + d", "abcd"},
    };
    Compile compile;
    for (auto e : exps_values) {
        {
            auto env = compile.get(e.first);
            ASSERT_TRUE(env->is_valid());
            env->add("a", 10);
            env->add("b", 5);
            env->add("d", "d");
            std::string v("123");
            ASSERT_TRUE(env->eval(v));
            ASSERT_EQ(v, e.second);
        }
    }
}

TEST(PARSETest, TestCompileComplicateCase1) {
    std::unordered_map<std::string, double> exps_values = {
            {"1;a", 10},
            {"b", 10.3},
            {"10", 10},
            {"10.3", 10.3},
            {"10.0", 10.0},
            {"10.3+1.3", 10.3 + 1.3},
            {"10.3^1.3", pow(10.3, 1.3)},
            {"0>0?0:10.3^1.3", pow(10.3, 1.3)},
            {"0>0?0:10.3^1.3", pow(10.3, 1.3)},
            {"0;10.3+1.3*-2.0/3.0", 10.3 + 1.3 * -2.0 / 3.0},
            {"10.3+1.3*-2.0/3.0", 10.3 + 1.3 * -2.0 / 3.0},
            {"10.3*(1.0+2.0/3.0+1.0)+1.2+3.4/3.7", 10.3 * (1.0 + 2.0 / 3.0 + 1.0) + 1.2 + 3.4 / 3.7},
            {"2 ^ 3", pow(2, 3)},
            {"2 ^ 3 + 1.3 * 10 + 1", pow(2, 3) + 1.3 * 10 + 1},
            {"10.3 ^ 1.3", pow(10.3, 1.3)},
            {"10.3 + 1.3 *-2.0 / 3.0", 10.3 + 1.3 * -2.0 / 3.0},
            {"10.3 * ( 1.0 + 2.0 / 3.0 + 1.0) + 1.2 + 3.4 / 3.7", 10.3 * (1.0 + 2.0 / 3.0 + 1.0) + 1.2 + 3.4 / 3.7},
            {"0;10.3 * ( 1.0 + 2.0 / 3.0 +-1.0) + 1.2 + 3.4 / -3.7",
             10.3 * (1.0 + 2.0 / 3.0 + -1.0) + 1.2 + 3.4 / -3.7},
            {"10.3 * ( 1.0 + 2.0 / 3.0 +-1.0) + 1.2 + 3.4 / -3.7", 10.3 * (1.0 + 2.0 / 3.0 + -1.0) + 1.2 + 3.4 / -3.7}

    };
    Compile compile;
    for (auto e : exps_values) {
        {
            double v = e.second;
            auto env = compile.get(e.first);
            ASSERT_TRUE(env->is_valid());
            env->add("a", v);
            env->add("b", v);
            CHECK_RESULT_TRUE(env, int32_t, v);
            CHECK_RESULT_TRUE(env, int64_t, v);
            CHECK_RESULT_TRUE(env, float, v);
            CHECK_RESULT_TRUE(env, double, v);
        }
    }
}

TEST(PARSETest, TestCompileComplicateCase2) {
    std::unordered_map<std::string, double> exps_values = {
            {"1;a+b", 15},
            {"a+b", 15},
            {"a + b", 15},
            {"a + b + 0.0", 15},
            {"a - b + 0.0", 5},
            {"a - b", 5},
            {"b-a", -5},
            {"a-b", 5},
            {"a/b", 2},
            {"a/b+0", 2},
            {"a*b+0", 50},
            {"b*-100", -500},
            {"b^10", pow(5, 10)},
            {"a^b", pow(10, 5)},
            {"b^a", pow(5, 10)},
            {"a+b^5", pow(5, 5) + 10},
            {"b+a^2", pow(10, 2) + 5},
            {"(1>0) and (1<0) or (1>0)", 1.0},
            {"(1>0) and (1<0) or (1<0)", 0.0},
            {"(1<0) or (1>0) and (1<0)", 0.0},
            {"(1>0) or (1>0) and (1<0)", 1.0},
    };
    Compile compile;
    for (auto e : exps_values) {
        {
            double v = e.second;
            auto env = compile.get(e.first);
            ASSERT_TRUE(env->is_valid());
            env->add("a", 10);
            env->add("b", 5);
            CHECK_RESULT_TRUE(env, int32_t, v);
            CHECK_RESULT_TRUE(env, int64_t, v);
            CHECK_RESULT_TRUE(env, float, v);
            CHECK_RESULT_TRUE(env, double, v);
        }
        {
            double v = e.second;
            auto env = compile.get(e.first);
            ASSERT_TRUE(env->is_valid());
            env->add("a", 10.0);
            env->add("b", 5.0);
            CHECK_RESULT_TRUE(env, int32_t, v);
            CHECK_RESULT_TRUE(env, int64_t, v);
            CHECK_RESULT_TRUE(env, float, v);
            CHECK_RESULT_TRUE(env, double, v);
        }
        {
            double v = e.second;
            auto env = compile.get(e.first);
            ASSERT_TRUE(env->is_valid());
            env->add("a", 10.0);
            env->add("b", 5L);
            CHECK_RESULT_TRUE(env, int32_t, v);
            CHECK_RESULT_TRUE(env, int64_t, v);
            CHECK_RESULT_TRUE(env, float, v);
            CHECK_RESULT_TRUE(env, double, v);
        }
        {
            double v = e.second;
            auto env = compile.get(e.first);
            ASSERT_TRUE(env->is_valid());
            env->add("a", 10.0f);
            env->add("b", 5L);
            CHECK_RESULT_TRUE(env, int32_t, v);
            CHECK_RESULT_TRUE(env, int64_t, v);
            CHECK_RESULT_TRUE(env, float, v);
            CHECK_RESULT_TRUE(env, double, v);
        }
    }
}

TEST(PARSETest, TestClassInhriet) {
    struct A {};

    Compile compile;
    class StringView {
        typedef std::function<std::string(A*)> BindFunc;

    public:
        StringView(Compile* c, const std::string& name = "string_base") : name(name) {
        }
        void add(Compile* c) {
            func = [&](A* ctx) -> std::string { return this->do_get(*ctx); };
            c->set_func<A*, std::string>(name, func);
        }
        virtual ~StringView(){};

        std::string get(A* ctx) {
            return do_get(*ctx);
        }

    protected:
        BindFunc func;
        std::string name;
        virtual std::string do_get(const A& context) const {
            return "";
        }
    };

    class Test : public StringView {
    public:
        Test(Compile* c, const std::string& n) : StringView(c, n){};
        virtual ~Test(){};

    protected:
        std::string do_get(const A& context) const {
            return "11";
        }
    };

    Test t1(&compile, "T1");
    Test t2(&compile, "T2");
    t1.add(&compile);
    t2.add(&compile);
    auto env = compile.get("T1(x)");
    A a;
    A* _a = &a;
    env->add("x", _a);
}

TEST(PARSETest, TestCompileFunction) {
    Compile compile;
    int64_t v = 1110;
    {
        compile.set_func<int64_t, int64_t>("A", [&](const int64_t& x) -> int64_t { return x + 1; });
        compile.set_func<int64_t, int64_t>("A1", [&](const int64_t& x) -> int64_t { return x + 2; });
        compile.set_func<int64_t, int64_t>("A2", [&](const int64_t& x) -> int64_t { return x + 3; });

        compile.set_func<double, double, int64_t>(
                "B", [&](const double& x, const double& y) -> int64_t { return x + y + 1; });
        compile.set_func<double, double, double, int64_t>(
                "C", [&](const double& x, const double& y, const double& z) -> int64_t { return x + y + z + 1; });
        compile.set_func<double, double, double, double, int64_t>(
                "D", [&](const double& x, const double& y, const double& z, const double& h) -> int64_t {
                    return x + y + z + 1;
                });
        compile.set_func<double, double, double, double, double, int64_t>(
                "E", [&](const double& x, const double& y, const double& z, const double& h,
                         const double& l) -> int64_t { return x + y + z + 1; });
    }

    std::unordered_map<std::string, double> exps_values = {
            {"A(0)", 1},
            {"B(0.1, 0.2)", 1},
            {"B(1.1, 0.2)", 2},
            {"C(1.1, 0.2, 0.7)", 3},
            {"C(1.1, 0.2, 0.7)+1", 4},
            {"C(1.1, 0.2, 0.7)*2", 6},
    };
    for (auto it : exps_values) {
        auto env = compile.get(it.first);
        auto v = it.second;
        CHECK_RESULT_TRUE(env, int32_t, v);
        CHECK_RESULT_TRUE(env, int64_t, v);
        CHECK_RESULT_TRUE(env, float, v);
        CHECK_RESULT_TRUE(env, double, v);
    }
    struct A {};
    auto a = new A;
    auto e = compile.get("1+1");
    e->add("a", a);
}

TEST(PARSETest, TestLackVarible) {
    Compile compile;

    std::unordered_map<std::string, double> exps_values = {
            {"a+b+c", 1}, {"a+b*c", 1}, {"a+b/c", 1},
    };
    for (auto it : exps_values) {
        std::cout << "exp:" << it.first << "\n";
        auto env = compile.get(it.first);
        env->add("a", 1);
        env->add("b", 1);
        double v;
        ASSERT_FALSE(env->eval(v));
    }
}

TEST(PARSETest, TestDataCollector) {
    Compile compile;
    for (int i = 0; i < 1000000; ++i) {
        DataCollector dc;
        dc.add("a", 1);
        dc.add("b", 1.0);
        dc.add("c", 2.1);

        auto env1 = compile.get("a+b");
        env1->add(dc);
        double r;
        ASSERT_TRUE(env1->eval(r));
        ASSERT_EQ(r, 2.0);
        auto env2 = compile.get("a+b+c");
        env2->add(dc);
        ASSERT_TRUE(env2->eval(r));
        ASSERT_EQ(r, 4.1);
        auto env3 = compile.get("a*b+c");
        env3->add(dc);
        ASSERT_TRUE(env3->eval(r));
        ASSERT_EQ(r, 3.1);
        auto env4 = compile.get("a*b+c+1");
        env4->add(dc);
        ASSERT_TRUE(env4->eval(r));
        ASSERT_EQ(r, 4.1);
        auto env5 = compile.get("a*b*c");
        env5->add(dc);
        ASSERT_TRUE(env5->eval(r));
        ASSERT_EQ(r, 2.1);
    }
}

TEST(PARSETest, TestCompileGet) {
    Compile compile;
    for (int i = 0; i < 1000000; ++i) {
        auto env1 = compile.get("a+b");
    }
    for (int i = 0; i < 100000000; ++i) {
        Compile compile;
        auto env1 = compile.get("a+b");
    }
}

TEST(PARSETest, TestCompileMultiThreadGet) {
    Compile compile;
    auto f = [&]() -> void {
        for (int i = 0; i < 1000000; ++i) {
            auto env1 = compile.get("a+b");
        }
    };
    std::thread t1(f);
    std::thread t2(f);
    t1.join();
    t2.join();
}

TEST(PARSETest, TestAssign) {
    Compile compile;
    DataCollector dc;
    dc.add("a", 1);
    dc.add("b", 1);

    bool b;
    auto env = compile.get("a==b");
    ASSERT_TRUE(env->is_valid());
    env->add(dc);
    ASSERT_TRUE(env->eval(b));
    ASSERT_TRUE(b);

    env = compile.get("a==c");
    env->add(dc);
    ASSERT_TRUE(env->is_valid());
    ASSERT_FALSE(env->eval(b));

    env = compile.get("a==b");
    ASSERT_TRUE(env->is_valid());
    env->add(dc);
    ASSERT_TRUE(env->eval(b));
    ASSERT_TRUE(b);

    env = compile.get("a=c");
    env->add(dc);
    ASSERT_TRUE(env->is_valid());
    ASSERT_FALSE(env->eval(b));
}
