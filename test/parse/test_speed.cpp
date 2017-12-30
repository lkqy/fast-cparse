#include <gtest/gtest.h>
#include "parse/parse.h"
using namespace parse;



void test_user_recall(size_t count) {
    std::vector<std::string> langs = {"en", "ch", "jp", "ab", "us", ""};
    std::string exp("lang == \"en\" and region == \"jp\"");
    ShuntingYard sy(exp);
    sy.compile();
    std::cout<<sy.get_log()<<"\n";
    int t = 0;
    std::string _lang = "lang";
    std::string _region = "region";
    std::string _hour = "hour";
    std::string en = "en";
    std::string jp = "jp";

    for (size_t i = 0; i < count; ++i) {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>(_lang.c_str(),  new StringValue(en.c_str())),
            std::make_pair<const char*, Value*>(_region.c_str(),  new StringValue(jp.c_str())),
        };

        t+= sy.eval_bool(env);
    }
    std::cout<<"parse  eval:"<<exp<<", loop:"<<t<<"\n";
}

void test_frequency_control_1(size_t count) {
    std::string exp("today_send < 7 and today_show < 4 and not today_active");
    ShuntingYard sy(exp);
    sy.compile();
    int t = 0;
    std::string va("today_send");
    std::string vb("today_show");
    std::string vc("today_active");

    for (size_t i = 0; i < count; ++i) {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("today_send",  new LongValue(i%7)),
            std::make_pair<const char*, Value*>("today_show",  new LongValue(i%7)),
            std::make_pair<const char*, Value*>("today_active",  new BoolValue(false)),
        };

        t+= sy.eval_bool(env);
    }
    std::cout<<"parse  eval:"<<exp<<", loop:"<<t<<"\n";
}

void test_frequency_control_2(size_t count) {
    std::string exp("a < 7 and b < 4 and not c");
    ShuntingYard sy(exp);
    sy.compile();
    int t = 0;
    std::string va("a");
    std::string vb("b");
    std::string vd("d");
    std::string vc("c");
    ValuePtr a = std::make_shared<LongValue>((0+7)%3);
    ValuePtr b = std::make_shared<LongValue>(1);
    ValuePtr c = std::make_shared<BoolValue>(2 == 1);;
    ValuePtr d = std::make_shared<LongValue>(3);

    for (size_t i = 0; i < count; ++i) {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("a",  new LongValue(i%7)),
            std::make_pair<const char*, Value*>("b",  new LongValue(i%7)),
            std::make_pair<const char*, Value*>("c",  new BoolValue(false)),
        };

        t+= sy.eval_bool(env);
    }
    std::cout<<"parse  eval:"<<exp<<", loop:"<<t<<"\n";
}

void test_frequency_control_3(size_t count) {
    std::string exp("today_send < 7 and today_show < 4 and today_send < 8 and today_show < 3 and today_send < 3");
    ShuntingYard sy(exp);
    sy.compile();
    int t = 0;
    std::string va("today_send");
    std::string vb("today_show");
    std::string vc("today_active");

    for (size_t i = 0; i < count; ++i) {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("today_send",  new LongValue(i%7)),
            std::make_pair<const char*, Value*>("today_show",  new LongValue(i%7)),
            std::make_pair<const char*, Value*>("today_active",  new BoolValue(false)),
        };

        t+= sy.eval_bool(env);
    }
    std::cout<<"parse  eval:"<<exp<<", loop:"<<t<<"\n";
}

void test_frequency_control_4(size_t count) {
    std::string exp("3 < 7 and 2 < 4 and 1 < 8 and 2 < 3 and 1< 3");
    ShuntingYard sy(exp);
    sy.compile();
    int t = 0;
    std::string va("today_send");
    std::string vb("today_show");
    std::string vc("today_active");

    for (size_t i = 0; i < count; ++i) {
        std::vector<std::pair<const char*, Value*>> env = {
            std::make_pair<const char*, Value*>("today_send",  new LongValue(i%7)),
            std::make_pair<const char*, Value*>("today_show",  new LongValue(i%7)),
            std::make_pair<const char*, Value*>("today_active",  new BoolValue(false)),
        };

        t+= sy.eval_bool(env);
    }
    std::cout<<"parse  eval:"<<exp<<", loop:"<<t<<"\n";
}
TEST(PARSETest, TestPerformance) {
    size_t count = 1000001;

    {
        clock_t start = clock();
        test_user_recall(count);
        clock_t end = clock();
        std::cout<<"user recall cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_frequency_control_1(count);
        clock_t end = clock();
        std::cout<<"user recall cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_frequency_control_2(count);
        clock_t end = clock();
        std::cout<<"user recall cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_frequency_control_3(count);
        clock_t end = clock();
        std::cout<<"user recall cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        clock_t start = clock();
        test_frequency_control_4(count);
        clock_t end = clock();
        std::cout<<"user recall cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
}


