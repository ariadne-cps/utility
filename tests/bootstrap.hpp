#ifndef ARIADNE_UTILITY_TEST_BOOTSTRAP_HPP
#define ARIADNE_UTILITY_TEST_BOOTSTRAP_HPP
#include <cmath>
#include <exception>
#include <iostream>
#include <sstream>
#include <string_view>
#include <utility>
namespace bootstrap {
inline int failures = 0;
template<class T> bool decision(T const& v) {
    if constexpr (requires { decide(v); }) return static_cast<bool>(decide(v));
    else if constexpr (requires { definitely(v); }) return static_cast<bool>(definitely(v));
    else return static_cast<bool>(v);
}
inline void check(bool ok, std::string_view label) {
    if (!ok) { ++failures; std::cerr << "FAIL: " << label << '\n'; }
}
template<class T> void require(T const& value, std::string_view label) { check(decision(value),label); }
template<class A,class B> void equal(A const& a,B const& b,std::string_view label) { require(a==b,label); }
template<class A,class B> void unequal(A const& a,B const& b,std::string_view label) { check(!decision(a==b),label); }
template<class A,class B> void identical(A const& a,B const& b,std::string_view label) { require(same(a,b),label); }
template<class A,class B> void less(A const& a,B const& b,std::string_view label) { require(a<b,label); }
template<class A,class B,class T> void within(A const& a,B const& b,T const& tolerance,std::string_view label) {
    using std::abs;
    if constexpr (requires { mag(a-b); }) require(mag(a-b)<=tolerance,label);
    else require(abs(a-b)<=tolerance,label);
}
template<class F> void call(F&& f,std::string_view label) {
    try { std::forward<F>(f)(); }
    catch(std::exception const& e) { ++failures; std::cerr<<"EXCEPTION in "<<label<<": "<<e.what()<<'\n'; }
    catch(...) { ++failures; std::cerr<<"UNKNOWN EXCEPTION in "<<label<<'\n'; }
}
template<class F> void throws(F&& f,std::string_view label) {
    bool caught=false;
    try { std::forward<F>(f)(); } catch(...) { caught=true; }
    check(caught,label);
}
template<class E,class F> void throws_type(F&& f,std::string_view label) {
    bool caught=false;
    try { std::forward<F>(f)(); }
    catch(E const&) { caught=true; }
    catch(...) { }
    check(caught,label);
}
template<class T> void print(T const& value,std::string_view label) { std::cout<<label<<" = "<<value<<'\n'; }
}
#endif
