#include <gtest/gtest.h>
#include "libparse2/parse.h"
using namespace libparse2;
/*
extern "C"
{
    #include "lauxlib.h"
    #include "lua.h"
    #include "lualib.h"
}

TEST(PARSETest, TestVsLua) {
    std::string exp("(a^b+c+d/e-f+(h^i+j*k-m+n)) * (1.0 + o *p ^ q /r+s+t)");
    size_t count = 10000;
    size_t batch = 100;
    {
        ShuntingYard sy(exp);
        sy.compile();
        double t = 0;
        std::string log;
        clock_t start = clock();
        for (size_t i = 0; i < count; ++i) {
            for (size_t j = 0; j < batch; ++j) {
                std::unordered_map<std::string, Value*> env = {
                    {"a", new DoubleValue(1.0*i/count+1)},
                    {"b", new DoubleValue(0.1*i/count+2)},
                    {"c", new DoubleValue(0.1*i/count+2)},
                    {"d", new DoubleValue(0.1*i/count+2)},
                    {"e", new DoubleValue(0.1*i/count+2)},
                    {"f", new DoubleValue(0.1*i/count+2)},
                    {"g", new DoubleValue(0.1*i/count+20)},
                    {"h", new DoubleValue(0.1*i/count+21)},
                    {"i", new DoubleValue(0.1*i/count+23)},
                    {"j", new DoubleValue(0.1*i/count+24)},
                    {"k", new DoubleValue(0.1*i/count+25)},
                    {"l", new DoubleValue(0.1*i/count+2)},
                    {"m", new DoubleValue(0.1*i/count+2)},
                    {"n", new DoubleValue(0.1*i/count+2)},
                    {"o", new DoubleValue(0.1*i/count+2)},
                    {"p", new DoubleValue(0.1*i/count+20)},
                    {"q", new DoubleValue(0.1*i/count+21)},
                    {"r", new DoubleValue(0.1*i/count+29)},
                    {"s", new DoubleValue(0.1*i/count+2)},
                    {"t", new DoubleValue(0.1*i/count+2)},
                };
                t+= sy.eval_double(env, log);
                if(!log.empty()) {
                    std::cout<<"error:"<<log<<"\n";
                }
            }
        }
        clock_t end = clock();
        std::cout<<"ret:"<<t<<"\n";
        std::cout<<"parse cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        lua_State * L = luaL_newstate();
        const std::string _exp = "function f1()\n\treturn " + exp + ";\nend";
        int f = luaL_dostring(L, _exp.c_str());
        double t = 0;
        clock_t start = clock();
        for (size_t i = 0; i < count; ++i) {
            for (size_t j = 0; j < batch; ++j) {
                lua_pushnumber(L, 1.0*i/count+ 1); lua_setglobal(L, "a");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "b");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "c");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "d");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "e");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "f");
                lua_pushnumber(L, 0.1*i/count+ 20); lua_setglobal(L, "g");
                lua_pushnumber(L, 0.1*i/count+ 21); lua_setglobal(L, "h");
                lua_pushnumber(L, 0.1*i/count+ 23); lua_setglobal(L, "i");
                lua_pushnumber(L, 0.1*i/count+ 24); lua_setglobal(L, "j");
                lua_pushnumber(L, 0.1*i/count+ 25); lua_setglobal(L, "k");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "l");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "m");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "n");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "o");
                lua_pushnumber(L, 0.1*i/count+ 20); lua_setglobal(L, "p");
                lua_pushnumber(L, 0.1*i/count+ 21); lua_setglobal(L, "q");
                lua_pushnumber(L, 0.1*i/count+ 29); lua_setglobal(L, "r");
                lua_pushnumber(L, 0.1*i /count+ 2); lua_setglobal(L, "s");
                lua_pushnumber(L, 0.1*i /count+ 2); lua_setglobal(L, "t");
                lua_getglobal(L, "f1");
                int ret = lua_pcall(L, 0, 1, 0);
                if (ret != 0) {
                    std::cout<<lua_tostring(L, -1)<<"\n";
                } else {
                    t += lua_tonumber(L, -1);
                }
            }
        }
        clock_t end = clock();
        std::cout<<"ret:"<<t<<"\n";
        std::cout<<"lua cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        double t = 0;
        clock_t start = clock();
        for (size_t i = 0; i < count; ++i) {
            lua_State * L = luaL_newstate();
            const std::string _exp = "function f1()\n\treturn " + exp + ";\nend";
            int f = luaL_dostring(L, _exp.c_str());
            for (size_t j = 0; j < batch; ++j) {
                lua_pushnumber(L, 1.0*i/count+ 1); lua_setglobal(L, "a");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "b");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "c");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "d");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "e");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "f");
                lua_pushnumber(L, 0.1*i/count+ 20); lua_setglobal(L, "g");
                lua_pushnumber(L, 0.1*i/count+ 21); lua_setglobal(L, "h");
                lua_pushnumber(L, 0.1*i/count+ 23); lua_setglobal(L, "i");
                lua_pushnumber(L, 0.1*i/count+ 24); lua_setglobal(L, "j");
                lua_pushnumber(L, 0.1*i/count+ 25); lua_setglobal(L, "k");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "l");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "m");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "n");
                lua_pushnumber(L, 0.1*i/count+ 2); lua_setglobal(L, "o");
                lua_pushnumber(L, 0.1*i/count+ 20); lua_setglobal(L, "p");
                lua_pushnumber(L, 0.1*i/count+ 21); lua_setglobal(L, "q");
                lua_pushnumber(L, 0.1*i/count+ 29); lua_setglobal(L, "r");
                lua_pushnumber(L, 0.1*i /count+ 2); lua_setglobal(L, "s");
                lua_pushnumber(L, 0.1*i /count+ 2); lua_setglobal(L, "t");
                lua_getglobal(L, "f1");
                int ret = lua_pcall(L, 0, 1, 0);
                if (ret != 0) {
                    std::cout<<lua_tostring(L, -1)<<"\n";
                } else {
                    t += lua_tonumber(L, -1);
                }
            }
        }
        clock_t end = clock();
        std::cout<<"ret:"<<t<<"\n";
        std::cout<<"thread safe lua cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
    {
        ShuntingYard sy(exp);
        sy.compile();
        double t = 0;
        std::string log;
        clock_t start = clock();
        for (size_t i = 0; i < count; ++i) {
            auto a = std::vector<double>(batch,1.0*i/count+2);
            auto b = std::vector<double>(batch,0.1*i/count+2);
            auto c = std::vector<double>(batch,0.1*i/count+2);
            auto d = std::vector<double>(batch,0.1*i/count+2);
            auto e = std::vector<double>(batch,0.1*i/count+2);
            auto f = std::vector<double>(batch,0.1*i/count+2);
            auto g = std::vector<double>(batch,0.1*i/count+20);
            auto h = std::vector<double>(batch,0.1*i/count+21);
            auto _i = std::vector<double>(batch,0.1*i/count+23);
            auto j = std::vector<double>(batch,0.1*i/count+24);
            auto k = std::vector<double>(batch,0.1*i/count+25);
            auto l = std::vector<double>(batch,0.1*i/count+2);
            auto m = std::vector<double>(batch,0.1*i/count+2);
            auto n = std::vector<double>(batch,0.1*i/count+2);
            auto o = std::vector<double>(batch,0.1*i/count+2);
            auto p = std::vector<double>(batch,0.1*i/count+20);
            auto q = std::vector<double>(batch,0.1*i/count+21);
            auto r = std::vector<double>(batch,0.1*i/count+29);
            auto s = std::vector<double>(batch,0.1*i/count+2);
            auto _t = std::vector<double>(batch,0.1*i/count+2);
            std::unordered_map<std::string, Value*> env = {
                {"a", new DoubleArrayValue(a)},
                {"b", new DoubleArrayValue(b)},
                {"c", new DoubleArrayValue(c)},
                {"d", new DoubleArrayValue(d)},
                {"e", new DoubleArrayValue(e)},
                {"f", new DoubleArrayValue(f)},
                {"g", new DoubleArrayValue(g)},
                {"h", new DoubleArrayValue(h)},
                {"i", new DoubleArrayValue(_i)},
                {"j", new DoubleArrayValue(j)},
                {"k", new DoubleArrayValue(k)},
                {"l", new DoubleArrayValue(l)},
                {"m", new DoubleArrayValue(m)},
                {"n", new DoubleArrayValue(n)},
                {"o", new DoubleArrayValue(o)},
                {"p", new DoubleArrayValue(p)},
                {"q", new DoubleArrayValue(q)},
                {"r", new DoubleArrayValue(r)},
                {"s", new DoubleArrayValue(s)},
                {"t", new DoubleArrayValue(_t)},
            };
            std::vector<double> fs = sy.eval_doubles(env, log, batch);
            for (double f :  fs) {
                t += f;
            }
            if(!log.empty()) {
                std::cout<<"error:"<<log<<"\n";
            }
        }
        clock_t end = clock();
        std::cout<<"ret:"<<t<<"\n";
        std::cout<<"parse cost:"<<(float)(end-start)/CLOCKS_PER_SEC<<"\n";
    }
}

*/
