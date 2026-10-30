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

#include "bootstrap.hpp"

using namespace Ariadne;
using namespace std::chrono_literals;

class TestStopwatch {
  public:

    void test_create() {
        Stopwatch<Milliseconds> sw;
    }

    void test_duration() {
        Stopwatch<Microseconds> sw;
        std::this_thread::sleep_for(10ms);
        auto duration = sw.click().duration();
        bootstrap::require((duration.count()>10000), "duration.count()>10000");
        bootstrap::require((sw.elapsed_seconds() > 0.01), "sw.elapsed_seconds() > 0.01");
    }

    void test() {
        bootstrap::call([&] { test_create(); }, "test_create()");
        bootstrap::call([&] { test_duration(); }, "test_duration()");
    }

};

int main() {
    TestStopwatch().test();
    return bootstrap::failures;
}
