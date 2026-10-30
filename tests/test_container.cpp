/***************************************************************************
 *            test_container.cpp
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

#include "utility/container.hpp"

#include "testing/testing.hpp"

using namespace Ariadne;

class TestContainer {
  public:
    Ariadne::Testing::Context* _ctx = nullptr;
    Ariadne::Testing::Context& ctx() { return *_ctx; }


    void test_map_get() {
        Map<int,int> im = {{1,10},{2,20}};
        ctx().expect_exception([&] { im.get(3); }, "im.get(3)");
        ctx().expect_equal((im.get(2)), (20), "im.get(2),20");
    }

    void test_map_convert() {
        Map<int,int> im = {{1,10},{2,20}};

        Map<int,double> dm(im);
        ctx().prove((dm.at(1) == im.at(1) and dm.at(2) == im.at(2)), "dm.at(1) == im.at(1) and dm.at(2) == im.at(2)");
    }

    void test_map_restrict_keys() {
        Set<int> s = {1,2};
        Map<int,double> m = {{1,1.2},{2,1.5},{3,1.0},{5,0.1}};
        auto restricted = restrict_keys(m,s);
        ctx().expect_equal((restricted.size()), (2), "restricted.size(),2");
        ctx().prove((restricted.has_key(1) and restricted.has_key(2)), "restricted.has_key(1) and restricted.has_key(2)");
        ctx().prove((not restricted.has_key(3)), "not restricted.has_key(3)");
    }

    void test_make_list_of_set() {
        Set<int> s = {1, 5, 3};
        auto l = make_list(s);
        ctx().expect_equal((l.size()), (3), "l.size(),3");
        ctx().prove((l.at(0) == 1 and l.at(1) == 3 and l.at(2) == 5), "l.at(0) == 1 and l.at(1) == 3 and l.at(2) == 5");
    }

    void test_print_vector() {
        std::vector<int> empty_int;
        std::vector<int> nonempty_int = {1};
        std::vector<unsigned int> empty_unsigned;
        std::vector<unsigned int> nonempty_unsigned = {1u};
        std::vector<double> empty_double;
        std::vector<double> nonempty_double = {1.0};
        std::cout << "empty_int" << " = " << (empty_int) << '\n';
        std::cout << "nonempty_int" << " = " << (nonempty_int) << '\n';
        std::cout << "empty_unsigned" << " = " << (empty_unsigned) << '\n';
        std::cout << "nonempty_unsigned" << " = " << (nonempty_unsigned) << '\n';
        std::cout << "empty_double" << " = " << (empty_double) << '\n';
        std::cout << "nonempty_double" << " = " << (nonempty_double) << '\n';
    }

};

int main() {
    Ariadne::Testing::Suite suite("container");
    suite.add("map_get", [&](Ariadne::Testing::Context& t) {
        TestContainer fixture;
        fixture._ctx = &t;
        fixture.test_map_get();
    });
    suite.add("map_convert", [&](Ariadne::Testing::Context& t) {
        TestContainer fixture;
        fixture._ctx = &t;
        fixture.test_map_convert();
    });
    suite.add("map_restrict_keys", [&](Ariadne::Testing::Context& t) {
        TestContainer fixture;
        fixture._ctx = &t;
        fixture.test_map_restrict_keys();
    });
    suite.add("make_list_of_set", [&](Ariadne::Testing::Context& t) {
        TestContainer fixture;
        fixture._ctx = &t;
        fixture.test_make_list_of_set();
    });
    suite.add("print_vector", [&](Ariadne::Testing::Context& t) {
        TestContainer fixture;
        fixture._ctx = &t;
        t.expect_no_throw([&] { fixture.test_print_vector(); }, "operation completes");
    });
    Ariadne::Testing::ConsoleReporter reporter(std::cout);
    return suite.run(reporter).successful() ? 0 : 1;
}
