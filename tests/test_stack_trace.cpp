/***************************************************************************
 *            test_stack_trace.cpp
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

#include "utility/stack_trace.hpp"

#include "utility/test.hpp"

using namespace Ariadne;

struct TestClass {
    void method() {
        stack_trace();
    }
};

class TestLRUCache {
  public:

    void test_free_function() {
        stack_trace();
    }

    void test_class_method() {
        TestClass().method();
    }

    void test() {
        ARIADNE_TEST_CALL(test_free_function());
        ARIADNE_TEST_CALL(test_class_method());
    }

};

int main() {
    TestLRUCache().test();
    return ARIADNE_TEST_FAILURES;
}
