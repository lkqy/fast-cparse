#include <gtest/gtest.h>
#include "parse/parse.h"
using namespace parse;



void test_user_recall(size_t count) {
    std::vector<std::string> langs = {"en", "ch", "jp", "ab", "us", ""};
    std::string exp("lang == \"en\" and hour in SET(8,12, 14, 19) and region == \"jp\"");
    ShuntingYard sy(exp);
    ASSERT_TRUE(sy.compile());
    std::cout<<sy.get_log()<<"\n";
    int t = 0;
    ValuePtr lang = std::make_shared<StringValue>("");
    ValuePtr region = std::make_shared<StringValue>("");
    ValuePtr hour = std::make_shared<LongValue>(0);;
    std::string _lang = "lang";
    std::string _reigon = "region";
    std::string _hour = "hour";
    sy.set_varible(_lang, lang);
    sy.set_varible(_reigon, region);
    sy.set_varible(_hour, hour);

    for (size_t i = 0; i < count; ++i) {
        std::dynamic_pointer_cast<StringValue>(lang)->val = langs[i%6];
        std::dynamic_pointer_cast<StringValue>(region)->val = "jp";
        std::dynamic_pointer_cast<LongValue>(hour)->val = 14;
        t+= sy.eval_bool();
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
    ValuePtr a = std::make_shared<LongValue>((0+7)%3);
    ValuePtr b = std::make_shared<LongValue>(1);
    ValuePtr c = std::make_shared<BoolValue>(2 == 1);;
    ValuePtr d = std::make_shared<LongValue>(3);

    for (size_t i = 0; i < count; ++i) {
        std::dynamic_pointer_cast<LongValue>(a)->val = i%3;
        std::dynamic_pointer_cast<LongValue>(b)->val = i%7;
        std::dynamic_pointer_cast<BoolValue>(c)->val = i%2 == 0;
        sy.set_varible(va, a);
        sy.set_varible(vb, b);
        sy.set_varible(vc, c);
        t+= sy.eval_bool();
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
        std::dynamic_pointer_cast<LongValue>(a)->val = i%3;
        std::dynamic_pointer_cast<LongValue>(b)->val = i%7;
        std::dynamic_pointer_cast<BoolValue>(c)->val = i%2 == 0;
        sy.set_varible(va, a);
        sy.set_varible(vb, b);
        sy.set_varible(vc, c);
        t+= sy.eval_bool();
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
}


