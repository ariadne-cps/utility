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
#include "utility/container.hpp"

#include "utility/test.hpp"

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

    void test() {
        ARIADNE_TEST_CALL(test_convert());
        ARIADNE_TEST_CALL(test_print());
        ARIADNE_TEST_CALL(test_complement());
        ARIADNE_TEST_CALL(test_move());
    }

};

int main() {
    TestArray().test();
    return ARIADNE_TEST_FAILURES;
}
