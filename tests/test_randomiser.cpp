/***************************************************************************
 *            test_randomiser.cpp
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

#include "utility/randomiser.hpp"
#include "utility/container.hpp"

#include "testing/testing.hpp"

using namespace Ariadne;
using namespace std;

class TestRandomiser {
  public:
    Ariadne::Testing::Context* _ctx = nullptr;
    Ariadne::Testing::Context& ctx() { return *_ctx; }
  private:
    size_t _num_tries;
  public:

    TestRandomiser(size_t num_tries) : _num_tries(num_tries) { }

    void test_int() {
        RandomGenerator generator;

        auto rnd = UniformIntRandomiser<unsigned int>(0,255);

        List<unsigned int> values;
        for (size_t i=0; i<_num_tries; ++i)
            values.push_back(rnd.get());

        std::cout << "values" << " = " << (values) << '\n';
    }

    void test_real() {
        RandomGenerator generator;

        auto rnd = UniformRealRandomiser<double>(0.0,1.0);

        List<double> values;
        for (size_t i=0; i<_num_tries; ++i)
            values.push_back(rnd.get());

        std::cout << "values" << " = " << (values) << '\n';
    }

    void test() {
        ctx().expect_no_throw([&] { test_int(); }, "test_int()");
        ctx().expect_no_throw([&] { test_real(); }, "test_real()");
    }

};

int main() {
    Ariadne::Testing::Suite suite("randomiser");
    TestRandomiser fixture(10);
    suite.add("scenarios", [&](Ariadne::Testing::Context& ctx) {
        fixture._ctx = &ctx;
        fixture.test();
    });
    Ariadne::Testing::ConsoleReporter reporter(std::cout);
    return suite.run(reporter).successful() ? 0 : 1;
}
