/***************************************************************************
 *            test_stopwatch.cpp
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
#include <thread>
#include "utility/stopwatch.hpp"

#include "testing/fixture.hpp"
#include "testing/checks.hpp"

using namespace Ariadne;
using namespace std::chrono_literals;

class TestStopwatch : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;


    void test_create() {
        Stopwatch<Milliseconds> sw;
    }

    void test_duration() {
        Stopwatch<Microseconds> sw;
        std::this_thread::sleep_for(10ms);
        auto duration = sw.click().duration();
        ARIADNE_PROVE(ctx(), duration.count()>10000);
        ARIADNE_PROVE(ctx(), sw.elapsed_seconds() > 0.01);
    }

};

int main() {
    Ariadne::Testing::Suite suite("stopwatch");
    ARIADNE_ADD_TEST_NO_THROW(suite, TestStopwatch, test_create);
    ARIADNE_ADD_TEST(suite, TestStopwatch, test_duration);
    Ariadne::Testing::ConsoleReporter reporter(std::cout);
    return suite.run(reporter).successful() ? 0 : 1;
}
