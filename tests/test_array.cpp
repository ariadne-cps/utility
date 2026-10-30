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

#include "bootstrap.hpp"

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
        bootstrap::call([&] { Array<TestClass> tcac(tcta); }, "Array<TestClass> tcac(tcta)");
    }

    void test_print() {
        Array<int> a1;
        bootstrap::print((a1), "a1");
        Array<int> a2 = {1, 2};
        bootstrap::print((a2), "a2");
    }

    void test_complement() {
        Array<size_t> vars = {1, 3};
        auto cmpl = complement(5, vars);
        bootstrap::equal((cmpl.size()), (3), "cmpl.size(),3");
        bootstrap::equal((cmpl[0]), (0), "cmpl[0],0");
        bootstrap::equal((cmpl[1]), (2), "cmpl[1],2");
        bootstrap::equal((cmpl[2]), (4), "cmpl[2],4");
    }

    void test_move() {
        Array<size_t> source = {1, 3};
        Array<size_t> moved(std::move(source));
        bootstrap::equal((moved.size()), (2), "moved.size(),2");
        bootstrap::equal((moved[0]), (1), "moved[0],1");
        bootstrap::equal((moved[1]), (3), "moved[1],3");
        bootstrap::require((source.empty()), "source.empty()");
    }

    void test_move_assignment_ownership() {
        {
            Array<Tracked> target(3u,Tracked(1));
            Array<Tracked> source(2u,Tracked(2));
            bootstrap::equal((Tracked::live), (5), "Tracked::live,5");
            target=std::move(source);
            bootstrap::equal((Tracked::live), (2), "Tracked::live,2");
            bootstrap::equal((target.size()), (2u), "target.size(),2u");
            bootstrap::require((source.empty()), "source.empty()");
        }
        bootstrap::equal((Tracked::live), (0), "Tracked::live,0");

        {
            UniformArray<Tracked> target(3u,Tracked(1));
            UniformArray<Tracked> source(2u,Tracked(2));
            bootstrap::equal((Tracked::live), (5), "Tracked::live,5");
            target=std::move(source);
            bootstrap::equal((Tracked::live), (2), "Tracked::live,2");
            bootstrap::equal((target.size()), (2u), "target.size(),2u");
            bootstrap::require((source.empty()), "source.empty()");
        }
        bootstrap::equal((Tracked::live), (0), "Tracked::live,0");
    }

    void test_exception_safe_construction() {
        Array<Tracked> array_source(3u,Tracked(1));
        int baseline=Tracked::live;
        Tracked::copies_until_throw=1;
        bootstrap::throws_type<std::runtime_error>([&] { (void)Array<Tracked>(array_source); }, "(void)Array<Tracked>(array_source),std::runtime_error");
        Tracked::copies_until_throw=-1;
        bootstrap::equal((Tracked::live), (baseline), "Tracked::live,baseline");

        UniformArray<Tracked> uniform_source(3u,Tracked(1));
        baseline=Tracked::live;
        Tracked::copies_until_throw=1;
        bootstrap::throws_type<std::runtime_error>([&] { (void)UniformArray<Tracked>(uniform_source); }, "(void)UniformArray<Tracked>(uniform_source),std::runtime_error");
        Tracked::copies_until_throw=-1;
        bootstrap::equal((Tracked::live), (baseline), "Tracked::live,baseline");
    }

    void test() {
        bootstrap::call([&] { test_convert(); }, "test_convert()");
        bootstrap::call([&] { test_print(); }, "test_print()");
        bootstrap::call([&] { test_complement(); }, "test_complement()");
        bootstrap::call([&] { test_move(); }, "test_move()");
        bootstrap::call([&] { test_move_assignment_ownership(); }, "test_move_assignment_ownership()");
        bootstrap::call([&] { test_exception_safe_construction(); }, "test_exception_safe_construction()");
    }

};

int main() {
    TestArray().test();
    return bootstrap::failures;
}
