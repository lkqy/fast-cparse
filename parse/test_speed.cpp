#include <gtest/gtest.h>
#include "libparse2/compile.h"
using namespace libparse2;
TEST(PARSETest, TestScalaOp) {
    std::string exp("(a^b+c+d/e-f+(h^i+j*k-m+n)) * (1.0 + o *p ^ q /r+s+t)");
    // std::string exp("a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t");
    // std::string exp("a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t");
    // std::string exp("(a^b+c*d");
    size_t count = 10000;
    size_t batch = 100;
    std::string a("a");
    std::string b("b");
    std::string c("c");
    std::string d("d");
    std::string e("e");
    std::string f("f");
    std::string g("g");
    std::string h("h");
    std::string _i("i");
    std::string _j("j");
    std::string k("k");
    std::string l("l");
    std::string m("m");
    std::string n("n");
    std::string o("o");
    std::string p("p");
    std::string q("q");
    std::string r("r");
    std::string s("s");
    std::string t("t");
    std::string u("u");
    {
        Compile compile;
        clock_t start = clock();
        auto env = compile.get(exp);
        for (size_t i = 0; i < count; ++i) {
            for (size_t j = 0; j < batch; ++j) {
                env->reset();
                env->add(a, 1.0 * i / count + 1);
                env->add(b, 1.0 * i / count + 2);
                env->add(c, 1.0 * i / count + 2);
                env->add(d, 1.0 * i / count + 2);
                env->add(e, 1.0 * i / count + 2);
                env->add(f, 1.0 * i / count + 2);
                env->add(g, 1.0 * i / count + 20);
                env->add(h, 1.0 * i / count + 21);
                env->add(_i, 1.0 * i / count + 23);
                env->add(_j, 1.0 * i / count + 24);
                env->add(k, 1.0 * i / count + 25);
                env->add(l, 1.0 * i / count + 2);
                env->add(m, 1.0 * i / count + 2);
                env->add(n, 1.0 * i / count + 2);
                env->add(o, 1.0 * i / count + 2);
                env->add(p, 1.0 * i / count + 20);
                env->add(q, 1.0 * i / count + 21);
                env->add(r, 1.0 * i / count + 29);
                env->add(s, 1.0 * i / count + 2);
                env->add(t, 1.0 * i / count + 2);
                double f;
                env->eval(f);
            }
        }
        clock_t end = clock();
        std::cout << "parse cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
    }
}

TEST(PARSETest, TestArrayOp) {
    // std::string exp("(a^b+c+d/e-f+(h^i+j*k-m+n)) * (a + o *p ^ q /r+s+t)+b+c");
    std::string exp("(a^b+c+d/e-f+(h^i+j*k-m+n)) * (1.0 + o *p ^ q /r+s+t)");
    // std::string exp("a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t");
    // std::string exp("(a^b+c*d");
    size_t count = 10000;
    size_t batch = 100;
    std::string a("a");
    std::string b("b");
    std::string c("c");
    std::string d("d");
    std::string e("e");
    std::string f("f");
    std::string g("g");
    std::string h("h");
    std::string _i("i");
    std::string _j("j");
    std::string k("k");
    std::string l("l");
    std::string m("m");
    std::string n("n");
    std::string o("o");
    std::string p("p");
    std::string q("q");
    std::string r("r");
    std::string s("s");
    std::string t("t");
    std::string u("u");
    Compile compile;
    clock_t start = clock();
    auto env = compile.get(exp);
    for (size_t i = 0; i < count; ++i) {
        env->clear();
        std::vector<double> a1(batch, 1.0 * i / count + 1);
        std::vector<double> b1(batch, 1.0 * i / count + 2);
        std::vector<double> c1(batch, 1.0 * i / count + 2);
        std::vector<double> d1(batch, 1.0 * i / count + 2);
        std::vector<double> e1(batch, 1.0 * i / count + 2);
        std::vector<double> f1(batch, 1.0 * i / count + 2);
        std::vector<double> g1(batch, 1.0 * i / count + 20);
        std::vector<double> h1(batch, 1.0 * i / count + 21);
        std::vector<double> _i1(batch, 1.0 * i / count + 23);
        std::vector<double> _j1(batch, 1.0 * i / count + 24);
        std::vector<double> k1(batch, 1.0 * i / count + 25);
        std::vector<double> l1(batch, 1.0 * i / count + 2);
        std::vector<double> m1(batch, 1.0 * i / count + 2);
        std::vector<double> n1(batch, 1.0 * i / count + 2);
        std::vector<double> o1(batch, 1.0 * i / count + 2);
        std::vector<double> p1(batch, 1.0 * i / count + 20);
        std::vector<double> q1(batch, 1.0 * i / count + 21);
        std::vector<double> r1(batch, 1.0 * i / count + 29);
        std::vector<double> s1(batch, 1.0 * i / count + 2);
        std::vector<double> t1(batch, 1.0 * i / count + 2);
        std::vector<double> _f;
        env->add(a, a1);
        env->add(b, b1);
        env->add(c, c1);
        env->add(d, d1);
        env->add(e, e1);
        env->add(f, f1);
        env->add(g, g1);
        env->add(h, h1);
        env->add(_i, _i1);
        env->add(_j, _j1);
        env->add(k, k1);
        env->add(l, l1);
        env->add(m, m1);
        env->add(n, n1);
        env->add(o, o1);
        env->add(p, p1);
        env->add(q, q1);
        env->add(r, r1);
        env->add(s, s1);
        env->add(t, t1);
        if (not env->eval(_f)) {
            std::cout << env->error_log() << "\n";
            break;
        };
    }
    clock_t end = clock();
    std::cout << "parse cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
}

TEST(PARSETest, TestScalaSpeedOp) {
    // std::string exp("(a^b+c+d/e-f+(h^i+j*k-m+n)) * (1.0 + o *p ^ q /r+s+t)");
    std::string exp("a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t");
    size_t count = 10000;
    size_t batch = 100;
    std::string a("a");
    std::string b("b");
    std::string c("c");
    std::string d("d");
    std::string e("e");
    std::string f("f");
    std::string g("g");
    std::string h("h");
    std::string _i("i");
    std::string _j("j");
    std::string k("k");
    std::string l("l");
    std::string m("m");
    std::string n("n");
    std::string o("o");
    std::string p("p");
    std::string q("q");
    std::string r("r");
    std::string s("s");
    std::string t("t");
    std::string u("u");
    Compile compile;
    auto env = compile.get(exp);
    env->add(a, 1.0 * 100 / count + 1);
    env->add(b, 1.0 * 100 / count + 2);
    env->add(c, 1.0 * 100 / count + 2);
    env->add(d, 1.0 * 100 / count + 2);
    env->add(e, 1.0 * 100 / count + 2);
    env->add(f, 1.0 * 100 / count + 2);
    env->add(g, 1.0 * 100 / count + 20);
    env->add(h, 1.0 * 100 / count + 21);
    env->add(_i, 1.0 * 100 / count + 23);
    env->add(_j, 1.0 * 100 / count + 24);
    env->add(k, 1.0 * 100 / count + 25);
    env->add(l, 1.0 * 100 / count + 2);
    env->add(m, 1.0 * 100 / count + 2);
    env->add(n, 1.0 * 100 / count + 2);
    env->add(o, 1.0 * 100 / count + 2);
    env->add(p, 1.0 * 100 / count + 20);
    env->add(q, 1.0 * 100 / count + 21);
    env->add(r, 1.0 * 100 / count + 29);
    env->add(s, 1.0 * 100 / count + 2);
    env->add(t, 1.0 * 100 / count + 2);
    clock_t start = clock();
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = 0; j < batch; ++j) {
            env->clear();
            double f;
            env->eval(f);
        }
    }
    clock_t end = clock();
    std::cout << "parse cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
}

TEST(PARSETest, TestArraySpeedOp) {
    std::string exp("a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t");
    // std::string exp("(a^b+c*d");
    size_t count = 10000;
    size_t batch = 100;
    std::string a("a");
    std::string b("b");
    std::string c("c");
    std::string d("d");
    std::string e("e");
    std::string f("f");
    std::string g("g");
    std::string h("h");
    std::string _i("i");
    std::string _j("j");
    std::string k("k");
    std::string l("l");
    std::string m("m");
    std::string n("n");
    std::string o("o");
    std::string p("p");
    std::string q("q");
    std::string r("r");
    std::string s("s");
    std::string t("t");
    std::string u("u");
    Compile compile;
    clock_t start = clock();
    auto env = compile.get(exp);
    std::vector<double> a1(batch, 1.0 * 100 / count + 1);
    std::vector<double> b1(batch, 1.0 * 100 / count + 1);
    std::vector<double> c1(batch, 1.0 * 100 / count + 1);
    std::vector<double> d1(batch, 1.0 * 100 / count + 1);
    std::vector<double> e1(batch, 1.0 * 100 / count + 1);
    std::vector<double> f1(batch, 1.0 * 100 / count + 1);
    std::vector<double> g1(batch, 1.0 * 100 / count + 1);
    std::vector<double> h1(batch, 1.0 * 100 / count + 1);
    std::vector<double> _i1(batch, 1.0 * 100 / count + 1);
    std::vector<double> _j1(batch, 1.0 * 100 / count + 1);
    std::vector<double> k1(batch, 1.0 * 100 / count + 1);
    std::vector<double> l1(batch, 1.0 * 100 / count + 1);
    std::vector<double> m1(batch, 1.0 * 100 / count + 1);
    std::vector<double> n1(batch, 1.0 * 100 / count + 1);
    std::vector<double> o1(batch, 1.0 * 100 / count + 1);
    std::vector<double> p1(batch, 1.0 * 100 / count + 1);
    std::vector<double> q1(batch, 1.0 * 100 / count + 1);
    std::vector<double> r1(batch, 1.0 * 100 / count + 1);
    std::vector<double> s1(batch, 1.0 * 100 / count + 1);
    std::vector<double> t1(batch, 1.0 * 100 / count + 1);
    std::vector<double> _f;
    env->add(a, a1);
    env->add(b, b1);
    env->add(c, c1);
    env->add(d, d1);
    env->add(e, e1);
    env->add(f, f1);
    env->add(g, g1);
    env->add(h, h1);
    env->add(_i, _i1);
    env->add(_j, _j1);
    env->add(k, k1);
    env->add(l, l1);
    env->add(m, m1);
    env->add(n, n1);
    env->add(o, o1);
    env->add(p, p1);
    env->add(q, q1);
    env->add(r, r1);
    env->add(s, s1);
    env->add(t, t1);
    for (size_t i = 0; i < count; ++i) {
        env->clear();
        if (not env->eval(_f)) {
            std::cout << env->error_log() << "\n";
            break;
        };
    }
    clock_t end = clock();
    std::cout << "parse cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
}
TEST(PARSETest, TestRawCPPSpeedOp) {
    size_t count = 10000;
    size_t batch = 100;
    double _s = 0;
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = 0; j < batch; ++j) {
            double a = 1.0 * i + j / count + 1;
            double b = 1.0 * i + j / count + 1;
            double c = 1.0 * i + j / count + 1;
            double d = 1.0 * i + j / count + 1;
            double e = 1.0 * i + j / count + 1;
            double f = 1.0 * i + j / count + 1;
            double g = 1.0 * i + j / count + 1;
            double h = 1.0 * i + j / count + 1;
            double _i = 1.0 * i + j / count + 1;
            double _j = 1.0 * i + j / count + 1;
            double k = 1.0 * i + j / count + 1;
            double l = 1.0 * i + j / count + 1;
            double m = 1.0 * i + j / count + 1;
            double n = 1.0 * i + j / count + 1;
            double o = 1.0 * i + j / count + 1;
            double p = 1.0 * i + j / count + 1;
            double q = 1.0 * i + j / count + 1;
            double r = 1.0 * i + j / count + 1;
            double s = 1.0 * i + j / count + 1;
            double t = 1.0 * i + j / count + 1;
            _s += (a / b + c + d + e + f + (h + _i + _j + k + m + n)) + (a + o + p + q + r + s + t) + b + c;
        }
    }
    std::cout << "sum:" << _s << "\n";
}

TEST(PARSETest, TestArrayOpByHardCode) {
    size_t count = 10000;
    size_t batch = 100;
    clock_t start = clock();
    std::vector<double> a1(batch, 1.0 * 100 / count + 1);
    std::vector<double> b1(batch, 1.0 * 100 / count + 1);
    std::vector<double> c1(batch, 1.0 * 100 / count + 1);
    std::vector<double> d1(batch, 1.0 * 100 / count + 1);
    std::vector<double> e1(batch, 1.0 * 100 / count + 1);
    std::vector<double> f1(batch, 1.0 * 100 / count + 1);
    std::vector<double> g1(batch, 1.0 * 100 / count + 1);
    std::vector<double> h1(batch, 1.0 * 100 / count + 1);
    std::vector<double> _i1(batch, 1.0 * 100 / count + 1);
    std::vector<double> _j1(batch, 1.0 * 100 / count + 1);
    std::vector<double> k1(batch, 1.0 * 100 / count + 1);
    std::vector<double> l1(batch, 1.0 * 100 / count + 1);
    std::vector<double> m1(batch, 1.0 * 100 / count + 1);
    std::vector<double> n1(batch, 1.0 * 100 / count + 1);
    std::vector<double> o1(batch, 1.0 * 100 / count + 1);
    std::vector<double> p1(batch, 1.0 * 100 / count + 1);
    std::vector<double> q1(batch, 1.0 * 100 / count + 1);
    std::vector<double> r1(batch, 1.0 * 100 / count + 1);
    std::vector<double> s1(batch, 1.0 * 100 / count + 1);
    std::vector<double> t1(batch, 1.0 * 100 / count + 1);
    DoubleArrayValue a2(std::move(a1));
    DoubleArrayValue b2(std::move(b1));
    DoubleArrayValue c2(std::move(c1));
    DoubleArrayValue d2(std::move(d1));
    DoubleArrayValue e2(std::move(e1));
    DoubleArrayValue f2(std::move(f1));
    DoubleArrayValue g2(std::move(g1));
    DoubleArrayValue h2(std::move(h1));
    DoubleArrayValue i2(std::move(_i1));
    DoubleArrayValue j2(std::move(_j1));
    DoubleArrayValue k2(std::move(k1));
    DoubleArrayValue l2(std::move(l1));
    DoubleArrayValue m2(std::move(m1));
    DoubleArrayValue n2(std::move(n1));
    DoubleArrayValue o2(std::move(o1));
    DoubleArrayValue p2(std::move(p1));
    DoubleArrayValue q2(std::move(q1));
    DoubleArrayValue r2(std::move(r1));
    DoubleArrayValue s2(std::move(s1));
    DoubleArrayValue t2(std::move(t1));

    std::vector<double> _r(batch, 1.0 * 100 / count + 1);
    DoubleArrayValue r(_r);
    for (size_t i = 0; i < count; ++i) {
        a2.add(&b2, &r);
        c2.add(&r, &r);
        d2.add(&r, &r);
        e2.add(&r, &r);
        f2.add(&r, &r);
        g2.add(&r, &r);
        h2.add(&r, &r);
        i2.add(&r, &r);
        j2.add(&r, &r);
        k2.add(&r, &r);
        l2.add(&r, &r);
        m2.add(&r, &r);
        n2.add(&r, &r);
        o2.add(&r, &r);
        p2.add(&r, &r);
        q2.add(&r, &r);
        r2.add(&r, &r);
        s2.add(&r, &r);
        t2.add(&r, &r);
    }
    clock_t end = clock();
    std::cout << "parse cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
}

TEST(PARSETest, TestRawArraySpeedOp) {
    std::string exp("(a^b+c+d/e-f+(h^i+j*k-m+n)) * (a + o *p ^ q /r+s+t)+b+c");

#define _AND(a, b) (a && b)
#define _OR(a, b) (a || b)
#define _NOT(a) (!a)
#define _EQUAL(a, b) (a == b)
#define _GT(a, b) (a > b)
#define _GET(a, b) (a >= b)
#define _LT(a, b) (a < b)
#define _LET(a, b) (a <= b)
#define _DIV(a, b) (b != 0 ? a / b : a)
#define __SAFE_DIV(a, b) (a != 0 ? b / a : b)
#define _ADD(a, b) (a + b)
#define _SUB(a, b) (a - b)
#define _MUL(a, b) (a * b)
#define _POW(a, b) (std::pow(a, b))
#define _S_POW(a, b) (std::pow(b, a))

#define _BATCH(a, b, c, op)               \
    {                                     \
        const size_t _s = a.size();       \
        for (size_t i = 0; i < _s; ++i) { \
            c[i] = op(a[i], b[i]);        \
        }                                 \
    }

    // std::string exp("(a^b+c*d");
    size_t count = 10000;
    size_t batch = 100;
    clock_t start = clock();
    double _s = 0;
    std::vector<double> a(batch, 1.0 * 100 / count + 1);
    std::vector<double> b(batch, 1.0 * 100 / count + 1);
    std::vector<double> c(batch, 1.0 * 100 / count + 1);
    std::vector<double> d(batch, 1.0 * 100 / count + 1);
    std::vector<double> e(batch, 1.0 * 100 / count + 1);
    std::vector<double> f(batch, 1.0 * 100 / count + 1);
    std::vector<double> g(batch, 1.0 * 100 / count + 1);
    std::vector<double> h(batch, 1.0 * 100 / count + 1);
    std::vector<double> _i(batch, 1.0 * 100 / count + 1);
    std::vector<double> _j(batch, 1.0 * 100 / count + 1);
    std::vector<double> k(batch, 1.0 * 100 / count + 1);
    std::vector<double> l(batch, 1.0 * 100 / count + 1);
    std::vector<double> m(batch, 1.0 * 100 / count + 1);
    std::vector<double> n(batch, 1.0 * 100 / count + 1);
    std::vector<double> o(batch, 1.0 * 100 / count + 1);
    std::vector<double> p(batch, 1.0 * 100 / count + 1);
    std::vector<double> q(batch, 1.0 * 100 / count + 1);
    std::vector<double> r(batch, 1.0 * 100 / count + 1);
    std::vector<double> s(batch, 1.0 * 100 / count + 1);
    std::vector<double> t(batch, 1.0 * 100 / count + 1);
    std::vector<double> re(batch, 1.0 * 100 / count + 1);
    for (size_t i = 0; i < count; ++i) {
        _BATCH(a, b, re, _ADD);
        _BATCH(c, re, re, _ADD);
        _BATCH(d, re, re, _ADD);
        _BATCH(e, re, re, _ADD);
        _BATCH(f, re, re, _ADD);
        _BATCH(g, re, re, _ADD);
        _BATCH(h, re, re, _ADD);
        _BATCH(_i, re, re, _ADD);
        _BATCH(_j, re, re, _ADD);
        _BATCH(k, re, re, _ADD);
        _BATCH(l, re, re, _ADD);
        _BATCH(m, re, re, _ADD);
        _BATCH(n, re, re, _ADD);
        _BATCH(o, re, re, _ADD);
        _BATCH(p, re, re, _ADD);
        _BATCH(q, re, re, _ADD);
        _BATCH(r, re, re, _ADD);
        _BATCH(s, re, re, _ADD);
        _BATCH(t, re, re, _ADD);
    }
    std::cout << "sum:" << _s << "\n";
}
TEST(PARSETest, TestBoolSpeed) {
    clock_t start = clock();
    Compile compile;
    const size_t loop = 10000;
    size_t count = 0;
    std::string region("region");
    std::string lang("lang");
    std::string en("en");
    std::string jp("jp");
    std::string exp("lang=='en' and region=='jp'");
    for (size_t i = 0; i < loop; ++i) {
        auto env = compile.get(exp);
        env->add(lang, en.c_str());
        env->add(region, jp.c_str());
        bool b;
        ASSERT_TRUE(env->eval(b));
    }
    clock_t end = clock();
    std::cout << "loop:" << loop << ", cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
}

TEST(PARSETest, TestMallocAllocMethod) {
    const size_t loop = 100000;
    const size_t batch = 10;
    int64_t s = 0;
    std::vector<Value*> buffers(loop);
    for (size_t j = 0; j < batch; ++j) {
        const size_t s = j % 3;
        for (size_t i = 0; i < loop; ++i) {
            Value* v;
            if (s == 0) {
                v = new IntValue(0);
            } else if (s == 1) {
                v = new LongValue(0);
            } else if (s == 2) {
                v = new DoubleValue(0);
            } else {
                std::cout << "error\n";
            }
            buffers[i] = v;
        }
        for (auto v : buffers) {
            delete v;
        }
    }
    std::cout << "sum:" << s << "\n";
}

TEST(PARSETest, TestEnvAllocMethod) {
    class MyEnv : public NewEnv {
    public:
        MyEnv(NodeHeadPtr head, Compile* sy) : NewEnv(head, sy){};
        void add(Value* v) {
            _unused_vars.push_back(v);
        }
        inline Value* myalloc(Value* v) {
            auto _v = alloc(v);
            return _v;
        }
        void clear() {
            _unused_vars.clear();
            //_unused_vars.resize(0);
        };
        void size() {
            std::cout << "size:" << _unused_vars.size() << "\n";
        }
    };
    Compile s;
    NodeHeadPtr head = std::make_shared<NodeHead>();
    MyEnv e(head, &s);
    auto v1 = new IntValue(0);
    auto v2 = new LongValue(0);
    auto v3 = new DoubleValue(0);
    const size_t loop = 100000;
    const size_t batch = 10;
    for (size_t i = 0; i < batch; ++i) {
        Value* v;
        size_t s = i % 3;
        if (s == 0) {
            v = new IntValue(i);
        } else if (s == 1) {
            v = new LongValue(i);
        } else if (s == 2) {
            v = new DoubleValue(i);
        }
        e.add(v);
    }
    MyEnv _e(head, &s);
    MyEnv* e1 = &e;
    MyEnv* e2 = &_e;

    clock_t start = clock();
    for (size_t i = 0; i < loop; ++i) {
        for (size_t j = 0; j < batch; ++j) {
            size_t s = j % 3;
            Value* v;
            if (s == 0) {
                v = e1->myalloc(v1);
            } else if (s == 1) {
                v = e1->myalloc(v2);
            } else if (s == 2) {
                v = e1->myalloc(v3);
            }
            e2->add(v);
        }
        e1->clear();
        std::swap(e1, e2);
    }
    clock_t end = clock();
    std::cout << "loop:" << loop << ", cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
}

TEST(PARSETest, TestArraySpeedOpTestBatchSize) {
    std::string exp("a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t");
    std::vector<size_t> batchs = {1, 2, 5, 10, 20, 30, 50, 100, 200, 500, 1000, 2000, 10000, 100000};
    for (auto batch : batchs) {
        size_t count = 10000000 / batch;
        std::string a("a");
        std::string b("b");
        std::string c("c");
        std::string d("d");
        std::string e("e");
        std::string f("f");
        std::string g("g");
        std::string h("h");
        std::string _i("i");
        std::string _j("j");
        std::string k("k");
        std::string l("l");
        std::string m("m");
        std::string n("n");
        std::string o("o");
        std::string p("p");
        std::string q("q");
        std::string r("r");
        std::string s("s");
        std::string t("t");
        std::string u("u");
        Compile compile;
        clock_t start = clock();
        auto env = compile.get(exp);
        std::vector<double> a1(batch, 1.0 * 100 / count + 1);
        std::vector<double> b1(batch, 1.0 * 100 / count + 1);
        std::vector<double> c1(batch, 1.0 * 100 / count + 1);
        std::vector<double> d1(batch, 1.0 * 100 / count + 1);
        std::vector<double> e1(batch, 1.0 * 100 / count + 1);
        std::vector<double> f1(batch, 1.0 * 100 / count + 1);
        std::vector<double> g1(batch, 1.0 * 100 / count + 1);
        std::vector<double> h1(batch, 1.0 * 100 / count + 1);
        std::vector<double> _i1(batch, 1.0 * 100 / count + 1);
        std::vector<double> _j1(batch, 1.0 * 100 / count + 1);
        std::vector<double> k1(batch, 1.0 * 100 / count + 1);
        std::vector<double> l1(batch, 1.0 * 100 / count + 1);
        std::vector<double> m1(batch, 1.0 * 100 / count + 1);
        std::vector<double> n1(batch, 1.0 * 100 / count + 1);
        std::vector<double> o1(batch, 1.0 * 100 / count + 1);
        std::vector<double> p1(batch, 1.0 * 100 / count + 1);
        std::vector<double> q1(batch, 1.0 * 100 / count + 1);
        std::vector<double> r1(batch, 1.0 * 100 / count + 1);
        std::vector<double> s1(batch, 1.0 * 100 / count + 1);
        std::vector<double> t1(batch, 1.0 * 100 / count + 1);
        std::vector<double> _f;
        env->add(a, a1);
        env->add(b, b1);
        env->add(c, c1);
        env->add(d, d1);
        env->add(e, e1);
        env->add(f, f1);
        env->add(g, g1);
        env->add(h, h1);
        env->add(_i, _i1);
        env->add(_j, _j1);
        env->add(k, k1);
        env->add(l, l1);
        env->add(m, m1);
        env->add(n, n1);
        env->add(o, o1);
        env->add(p, p1);
        env->add(q, q1);
        env->add(r, r1);
        env->add(s, s1);
        env->add(t, t1);
        for (size_t i = 0; i < count; ++i) {
            env->clear();
            if (not env->eval(_f)) {
                std::cout << env->error_log() << "\n";
                break;
            };
        }
        clock_t end = clock();
        std::cout << "batch size:" << batch << ", parse cost:" << (float)(end - start) / CLOCKS_PER_SEC << "\n";
    }
}
