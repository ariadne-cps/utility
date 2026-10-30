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
#include "ariadne-testing.hpp"
#include <utility>

#include "utility/array.hpp"
#include "utility/uniform_array.hpp"
#include "utility/container.hpp"


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

class TestArray : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;


    void test_convert() {
        Array<TestClass> tca = {TestClass(1), TestClass(2)};
        Array<TestConvertibleTo> tcta = {TestConvertibleTo(1), TestConvertibleTo(2)};
        ARIADNE_EXPECT_NO_THROW(Array<TestClass> tcac(tcta));
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
        ARIADNE_EXPECT_EQUAL(cmpl.size(), 3);
        ARIADNE_EXPECT_EQUAL(cmpl[0], 0);
        ARIADNE_EXPECT_EQUAL(cmpl[1], 2);
        ARIADNE_EXPECT_EQUAL(cmpl[2], 4);
    }

    void test_move() {
        Array<size_t> source = {1, 3};
        Array<size_t> moved(std::move(source));
        ARIADNE_EXPECT_EQUAL(moved.size(), 2);
        ARIADNE_EXPECT_EQUAL(moved[0], 1);
        ARIADNE_EXPECT_EQUAL(moved[1], 3);
        ARIADNE_PROVE(source.empty());
    }

    void test_move_assignment_ownership() {
        {
            Array<Tracked> target(3u,Tracked(1));
            Array<Tracked> source(2u,Tracked(2));
            ARIADNE_EXPECT_EQUAL(Tracked::live, 5);
            target=std::move(source);
            ARIADNE_EXPECT_EQUAL(Tracked::live, 2);
            ARIADNE_EXPECT_EQUAL(target.size(), 2u);
            ARIADNE_PROVE(source.empty());
        }
        ARIADNE_EXPECT_EQUAL(Tracked::live, 0);

        {
            UniformArray<Tracked> target(3u,Tracked(1));
            UniformArray<Tracked> source(2u,Tracked(2));
            ARIADNE_EXPECT_EQUAL(Tracked::live, 5);
            target=std::move(source);
            ARIADNE_EXPECT_EQUAL(Tracked::live, 2);
            ARIADNE_EXPECT_EQUAL(target.size(), 2u);
            ARIADNE_PROVE(source.empty());
        }
        ARIADNE_EXPECT_EQUAL(Tracked::live, 0);
    }

    void test_exception_safe_construction() {
        Array<Tracked> array_source(3u,Tracked(1));
        int baseline=Tracked::live;
        Tracked::copies_until_throw=1;
        ARIADNE_EXPECT_THROWS(std::runtime_error, (void)Array<Tracked>(array_source));
        Tracked::copies_until_throw=-1;
        ARIADNE_EXPECT_EQUAL(Tracked::live, baseline);

        UniformArray<Tracked> uniform_source(3u,Tracked(1));
        baseline=Tracked::live;
        Tracked::copies_until_throw=1;
        ARIADNE_EXPECT_THROWS(std::runtime_error, (void)UniformArray<Tracked>(uniform_source));
        Tracked::copies_until_throw=-1;
        ARIADNE_EXPECT_EQUAL(Tracked::live, baseline);
    }

};

int main() {
    ARIADNE_TEST_SUITE("array");
    ARIADNE_ADD_TEST(TestArray, test_convert);
    ARIADNE_ADD_TEST_NO_THROW(TestArray, test_print);
    ARIADNE_ADD_TEST(TestArray, test_complement);
    ARIADNE_ADD_TEST(TestArray, test_move);
    ARIADNE_ADD_TEST(TestArray, test_move_assignment_ownership);
    ARIADNE_ADD_TEST(TestArray, test_exception_safe_construction);
    return ARIADNE_RUN_TESTS();
}
