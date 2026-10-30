/***************************************************************************
 *            test_array.cpp
 *
 *  Copyright  2009-21  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Ariadne Utility.
 *
 *  Ariadne Utility is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ariadne Utility is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne Utility.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <utility>

#include "utility/array.hpp"
#include "utility/uniform_array.hpp"
#include "utility/container.hpp"

#include "testing/testing.hpp"

using namespace Ariadne;

struct TestConvertibleTo {
    TestConvertibleTo(int a_) : a(a_) { }
    int a;
};

struct TestClass {
    TestClass(int a_) : a(a_) { }
    explicit TestClass(TestConvertibleTo const& c) : TestClass(c.a) { }
    int a;
};

struct Tracked {
    static int live;
    static int copies_until_throw;
    int value;

    Tracked(int value_=0) : value(value_) { ++live; }
    Tracked(Tracked const& other) : value(other.value) {
        if(copies_until_throw==0) { throw std::runtime_error("tracked copy failure"); }
        if(copies_until_throw>0) { --copies_until_throw; }
        ++live;
    }
    Tracked(Tracked&& other) noexcept : value(other.value) { ++live; }
    Tracked& operator=(Tracked const&)=default;
    Tracked& operator=(Tracked&&)=default;
    ~Tracked() { --live; }
};

int Tracked::live=0;
int Tracked::copies_until_throw=-1;

class TestArray {
  public:
    Ariadne::Testing::Context* _ctx = nullptr;
    Ariadne::Testing::Context& ctx() { return *_ctx; }


    void test_convert() {
        Array<TestClass> tca = {TestClass(1), TestClass(2)};
        Array<TestConvertibleTo> tcta = {TestConvertibleTo(1), TestConvertibleTo(2)};
        ctx().expect_no_throw([&] { Array<TestClass> tcac(tcta); }, "Array<TestClass> tcac(tcta)");
    }

    void test_print() {
        Array<int> a1;
        std::cout << "a1" << " = " << (a1) << '\n';
        Array<int> a2 = {1, 2};
        std::cout << "a2" << " = " << (a2) << '\n';
    }

    void test_complement() {
        Array<size_t> vars = {1, 3};
        auto cmpl = complement(5, vars);
        ctx().expect_equal((cmpl.size()), (3), "cmpl.size(),3");
        ctx().expect_equal((cmpl[0]), (0), "cmpl[0],0");
        ctx().expect_equal((cmpl[1]), (2), "cmpl[1],2");
        ctx().expect_equal((cmpl[2]), (4), "cmpl[2],4");
    }

    void test_move() {
        Array<size_t> source = {1, 3};
        Array<size_t> moved(std::move(source));
        ctx().expect_equal((moved.size()), (2), "moved.size(),2");
        ctx().expect_equal((moved[0]), (1), "moved[0],1");
        ctx().expect_equal((moved[1]), (3), "moved[1],3");
        ctx().prove((source.empty()), "source.empty()");
    }

    void test_move_assignment_ownership() {
        {
            Array<Tracked> target(3u,Tracked(1));
            Array<Tracked> source(2u,Tracked(2));
            ctx().expect_equal((Tracked::live), (5), "Tracked::live,5");
            target=std::move(source);
            ctx().expect_equal((Tracked::live), (2), "Tracked::live,2");
            ctx().expect_equal((target.size()), (2u), "target.size(),2u");
            ctx().prove((source.empty()), "source.empty()");
        }
        ctx().expect_equal((Tracked::live), (0), "Tracked::live,0");

        {
            UniformArray<Tracked> target(3u,Tracked(1));
            UniformArray<Tracked> source(2u,Tracked(2));
            ctx().expect_equal((Tracked::live), (5), "Tracked::live,5");
            target=std::move(source);
            ctx().expect_equal((Tracked::live), (2), "Tracked::live,2");
            ctx().expect_equal((target.size()), (2u), "target.size(),2u");
            ctx().prove((source.empty()), "source.empty()");
        }
        ctx().expect_equal((Tracked::live), (0), "Tracked::live,0");
    }

    void test_exception_safe_construction() {
        Array<Tracked> array_source(3u,Tracked(1));
        int baseline=Tracked::live;
        Tracked::copies_until_throw=1;
        ctx().expect_throws<std::runtime_error>([&] { (void)Array<Tracked>(array_source); }, "(void)Array<Tracked>(array_source),std::runtime_error");
        Tracked::copies_until_throw=-1;
        ctx().expect_equal((Tracked::live), (baseline), "Tracked::live,baseline");

        UniformArray<Tracked> uniform_source(3u,Tracked(1));
        baseline=Tracked::live;
        Tracked::copies_until_throw=1;
        ctx().expect_throws<std::runtime_error>([&] { (void)UniformArray<Tracked>(uniform_source); }, "(void)UniformArray<Tracked>(uniform_source),std::runtime_error");
        Tracked::copies_until_throw=-1;
        ctx().expect_equal((Tracked::live), (baseline), "Tracked::live,baseline");
    }

};

int main() {
    Ariadne::Testing::Suite suite("array");
    suite.add("convert", [&](Ariadne::Testing::Context& t) {
        TestArray fixture;
        fixture._ctx = &t;
        fixture.test_convert();
    });
    suite.add("print", [&](Ariadne::Testing::Context& t) {
        TestArray fixture;
        fixture._ctx = &t;
        t.expect_no_throw([&] { fixture.test_print(); }, "operation completes");
    });
    suite.add("complement", [&](Ariadne::Testing::Context& t) {
        TestArray fixture;
        fixture._ctx = &t;
        fixture.test_complement();
    });
    suite.add("move", [&](Ariadne::Testing::Context& t) {
        TestArray fixture;
        fixture._ctx = &t;
        fixture.test_move();
    });
    suite.add("move_assignment_ownership", [&](Ariadne::Testing::Context& t) {
        TestArray fixture;
        fixture._ctx = &t;
        fixture.test_move_assignment_ownership();
    });
    suite.add("exception_safe_construction", [&](Ariadne::Testing::Context& t) {
        TestArray fixture;
        fixture._ctx = &t;
        fixture.test_exception_safe_construction();
    });
    Ariadne::Testing::ConsoleReporter reporter(std::cout);
    return suite.run(reporter).successful() ? 0 : 1;
}
