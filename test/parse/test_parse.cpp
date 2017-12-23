#include <gtest/gtest.h>
#include "parse/parse.h"
using namespace parse;

void test_string(size_t count, bool use_exp) {
    std::string exp("a==\"abcddddddddd\" and ccc == \"xxxxxxxy1\" or ddd == \"22222222222221\"");
    ShuntingYard sy(exp);
    int t = 0;

    std::unordered_map<std::string, ValuePtr> env;
    sy.set_varible("a", std::make_shared<StringValue>("abcddddddddd"));
    sy.set_varible("ccc", std::make_shared<StringValue>("xxxxxxxy"));
    sy.set_varible("ddd", std::make_shared<StringValue>("22222222222221"));
    std::unordered_map<int, std::string> env1;
    env1[1] = "abcddddddddd";
    env1[2] = "xxxxxxxy";
    env1[3] = "22222222222221";
    std::string a("abcddddddddd");
    std::string ccc("xxxxxxxy");
    std::string ddd("22222222222221");
    for (size_t i = 0; i < count; ++i) {
        if(use_exp) {
            t += sy.eval_bool();
        }else {
            if ((env1[1] == a && env1[2] == ccc) || env1[3] == ddd){
                t += 1;
            }
        }
    }
    std::cout<<"parse:"<<use_exp<<", eval:"<<exp<<", loop:"<<t<<"\n";
}

void test_number(size_t count, bool use_exp) {
    std::string exp("11111+11 ");
    ShuntingYard sy(exp);
    int t = 0;
    std::unordered_map<int, long> env1;
    env1[1] = 1111;
    env1[2] = 1;

    std::unordered_map<std::string, ValuePtr> env;
    for (size_t i = 0; i < count; ++i) {
        if (use_exp) {
              ValuePtr value = sy.eval();
                  auto _v = std::dynamic_pointer_cast<LongValue>(value);
                 t += _v->get_bool();
        }else {
            if(env1[1] > env1[2]) t+=1;
        }
    }
    std::cout<<"parse:"<<use_exp<<", eval:"<<exp<<", loop:"<<t<<"\n";
}

void test_bool_ext(size_t count, bool use_exp) {
    std::string exp("a<b and c < b and aaa == \"abcddddddddd\" and ccc == \"xxxxxxxy\" or a > c");
    ShuntingYard sy(exp);
    std::unordered_map<std::string, ValuePtr> env;
    sy.set_varible("a", std::make_shared<DoubleValue>(23.3));
    sy.set_varible("b", std::make_shared<DoubleValue>(23.31));
    sy.set_varible("c", std::make_shared<DoubleValue>(3.31));
    sy.set_varible("aaa", std::make_shared<StringValue>("abcddddddddd"));
    sy.set_varible("ccc", std::make_shared<StringValue>("xxxxxxxy"));
    sy.set_varible("ddd", std::make_shared<StringValue>("22222222222221"));
    double a = 23.3;
    double b = 23.31;
    double c = 3.31;
    std::string aaa("abcddddddddd");
    std::string ccc("xxxxxxxy");
    std::string ddd("22222222222221");
    int t = 0;

    for (size_t i = 0; i < count; ++i) {
        if (use_exp) {
            t += sy.eval_bool();
        }else {
            if(a < b && c < b && (aaa == "abcddddddddd") 
                    &&  (ccc == "xxxxxxxy") && (a > c)) {
                t+= 1;
            }
        }
    }
    std::cout<<"parse:"<<use_exp<<", eval:"<<exp<<", loop:"<<t<<"\n";
}

void test_bool(size_t count, bool use_exp) {
    //std::string exp("a<b and c < b");
    std::string exp("a > c ");
    ShuntingYard sy(exp);
    std::unordered_map<std::string, ValuePtr> env;
    sy.set_varible("a", std::make_shared<DoubleValue>(23.3));
    sy.set_varible("b", std::make_shared<DoubleValue>(23.31));
    sy.set_varible("c",  std::make_shared<DoubleValue>(3.31));
    double a = 23.3;
    double b = 23.31;
    double c = 3.31;
    int t = 0;
    std::unordered_map<std::string, double> test_env;
    test_env["a"] = 23.3;
    test_env["b"] = 23.31;
    test_env["c"] = 3.31;

    std::unordered_map<int, double> test_env1;
    test_env1[1] = 23.3;
    test_env1[2] = 23.31;
    test_env1[3] = 3.31;

    for (size_t i = 0; i < count; ++i) {
        if(use_exp) {
            t += sy.eval_bool();
        }else {
            if (test_env1[1] <test_env1[2] && test_env1[3] < test_env1[2]){
                t += 1;
            }
        }
    }
    std::cout<<"parse:"<<use_exp<<", eval:"<<exp<<", loop:"<<t<<"\n";
}

void test_bool_number(size_t count, bool use_exp) {
    //std::string exp("a<b and c < b");
    std::string exp(" 23.3 < 23.31 and 3.31 < 23.31 ");
    ShuntingYard sy(exp);
    std::unordered_map<std::string, ValuePtr> env;
    sy.set_varible("a", std::make_shared<DoubleValue>(23.3));
    sy.set_varible("b", std::make_shared<DoubleValue>(23.31));
    sy.set_varible("c", std::make_shared<DoubleValue>(3.31));
    double a = 23.3;
    double b = 23.31;
    double c = 3.31;
    int t = 0;
    std::unordered_map<std::string, double> test_env;
    test_env["a"] = 23.3;
    test_env["b"] = 23.31;
    test_env["c"] = 3.31;

    std::unordered_map<int, double> test_env1;
    test_env1[1] = 23.3;
    test_env1[2] = 23.31;
    test_env1[3] = 3.31;

    for (size_t i = 0; i < count; ++i) {
        if(use_exp) {
            t += sy.eval_bool();
        }else {
            if (test_env1[1] <test_env1[2] && test_env1[3] < test_env1[2]){
                t += 1;
            }
        }
    }
    std::cout<<"parse:"<<use_exp<<", eval:"<<exp<<", loop:"<<t<<"\n";
}

TEST(PARSETest, TestPerformance) {
    size_t count = 1000001;

    {
        clock_t start = clock();
        test_number(count, true);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_number(count, false);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_string(count, true);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_string(count, false);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_bool(count, true);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_bool(count, false);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_bool_number(count, true);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_bool_number(count, false);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_bool_ext(count, true);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }

    {
        clock_t start = clock();
        test_bool_ext(count, false);
        clock_t end = clock();
        std::cout<<"cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
}

