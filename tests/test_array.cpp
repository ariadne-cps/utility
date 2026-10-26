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

#include "macro/testing.hpp"

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

    void test_convert() {
        Array<TestClass> tca = {TestClass(1), TestClass(2)};
        Array<TestConvertibleTo> tcta = {TestConvertibleTo(1), TestConvertibleTo(2)};
        ARIADNE_TEST_EXECUTE(Array<TestClass> tcac(tcta));
    }

    void test_print() {
        Array<int> a1;
        ARIADNE_TEST_PRINT(a1);
        Array<int> a2 = {1, 2};
        ARIADNE_TEST_PRINT(a2);
    }

    void test_complement() {
        Array<size_t> vars = {1, 3};
        auto cmpl = complement(5, vars);
        ARIADNE_TEST_EQUALS(cmpl.size(),3);
        ARIADNE_TEST_EQUALS(cmpl[0],0);
        ARIADNE_TEST_EQUALS(cmpl[1],2);
        ARIADNE_TEST_EQUALS(cmpl[2],4);
    }

    void test_move() {
        Array<size_t> source = {1, 3};
        Array<size_t> moved(std::move(source));
        ARIADNE_TEST_EQUALS(moved.size(),2);
        ARIADNE_TEST_EQUALS(moved[0],1);
        ARIADNE_TEST_EQUALS(moved[1],3);
        ARIADNE_TEST_ASSERT(source.empty());
    }

    void test_move_assignment_ownership() {
        {
            Array<Tracked> target(3u,Tracked(1));
            Array<Tracked> source(2u,Tracked(2));
            ARIADNE_TEST_EQUALS(Tracked::live,5);
            target=std::move(source);
            ARIADNE_TEST_EQUALS(Tracked::live,2);
            ARIADNE_TEST_EQUALS(target.size(),2u);
            ARIADNE_TEST_ASSERT(source.empty());
        }
        ARIADNE_TEST_EQUALS(Tracked::live,0);

        {
            UniformArray<Tracked> target(3u,Tracked(1));
            UniformArray<Tracked> source(2u,Tracked(2));
            ARIADNE_TEST_EQUALS(Tracked::live,5);
            target=std::move(source);
            ARIADNE_TEST_EQUALS(Tracked::live,2);
            ARIADNE_TEST_EQUALS(target.size(),2u);
            ARIADNE_TEST_ASSERT(source.empty());
        }
        ARIADNE_TEST_EQUALS(Tracked::live,0);
    }

    void test_exception_safe_construction() {
        Array<Tracked> array_source(3u,Tracked(1));
        int baseline=Tracked::live;
        Tracked::copies_until_throw=1;
        ARIADNE_TEST_THROWS((void)Array<Tracked>(array_source),std::runtime_error);
        Tracked::copies_until_throw=-1;
        ARIADNE_TEST_EQUALS(Tracked::live,baseline);

        UniformArray<Tracked> uniform_source(3u,Tracked(1));
        baseline=Tracked::live;
        Tracked::copies_until_throw=1;
        ARIADNE_TEST_THROWS((void)UniformArray<Tracked>(uniform_source),std::runtime_error);
        Tracked::copies_until_throw=-1;
        ARIADNE_TEST_EQUALS(Tracked::live,baseline);
    }

    void test() {
        ARIADNE_TEST_CALL(test_convert());
        ARIADNE_TEST_CALL(test_print());
        ARIADNE_TEST_CALL(test_complement());
        ARIADNE_TEST_CALL(test_move());
        ARIADNE_TEST_CALL(test_move_assignment_ownership());
        ARIADNE_TEST_CALL(test_exception_safe_construction());
    }

};

int main() {
    TestArray().test();
    return ARIADNE_TEST_FAILURES;
}
