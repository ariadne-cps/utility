/***************************************************************************
 *            test_lazy.cpp
 *
 *  Copyright  2023  Luca Geretti
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
#include "utility/lazy.hpp"


using namespace Ariadne;

class TestClass {
  public:

    TestClass(double a) {
        _value = a*a;
        std::cout << "\"TestClass object created\"" << " = " << ("TestClass object created") << '\n';
    }

    double value() { return _value; }

  private:
    double _value;
};

class TestLazy : public Ariadne::ContextFixture {
  public:
    using ContextFixture::ContextFixture;

    void test_creation() {
        double arg = 2.0;
        int creations = 0;
        Lazy<TestClass> lazy([arg,&creations]{ ++creations; return new TestClass(arg); });

        std::cout << "\"Lazy created\"" << " = " << ("Lazy created") << '\n';
        TestClass obj = lazy();
        ARIADNE_EXPECT_EQUAL(obj.value(), 4.0);
        TestClass same_obj = lazy();
        ARIADNE_EXPECT_EQUAL(same_obj.value(), 4.0);
        ARIADNE_EXPECT_EQUAL(creations, 1);
    }

};

int main() {
    ARIADNE_TEST_SUITE("lazy");
    ARIADNE_ADD_TEST(TestLazy, test_creation);
    return ARIADNE_RUN_TESTS();
}
