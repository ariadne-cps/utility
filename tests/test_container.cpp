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

#include "utility/test.hpp"

using namespace Ariadne;

class TestContainer {
  public:

    void test_map_get() {
        Map<int,int> im = {{1,10},{2,20}};
        ARIADNE_TEST_FAIL(im.get(3));
        ARIADNE_TEST_EQUALS(im.get(2),20);
    }

    void test_map_convert() {
        Map<int,int> im = {{1,10},{2,20}};

        Map<int,double> dm(im);
        ARIADNE_TEST_ASSERT(dm.at(1) == im.at(1) and dm.at(2) == im.at(2));
    }

    void test_map_restrict_keys() {
        Set<int> s = {1,2};
        Map<int,double> m = {{1,1.2},{2,1.5},{3,1.0},{5,0.1}};
        auto restricted = restrict_keys(m,s);
        ARIADNE_TEST_EQUALS(restricted.size(),2);
        ARIADNE_TEST_ASSERT(restricted.has_key(1) and restricted.has_key(2));
        ARIADNE_TEST_ASSERT(not restricted.has_key(3));
    }

    void test_make_list_of_set() {
        Set<int> s = {1, 5, 3};
        auto l = make_list(s);
        ARIADNE_TEST_EQUALS(l.size(),3);
        ARIADNE_TEST_ASSERT(l.at(0) == 1 and l.at(1) == 3 and l.at(2) == 5);
    }

    void test_print_vector() {
        std::vector<int> empty_int;
        std::vector<int> nonempty_int = {1};
        std::vector<unsigned int> empty_unsigned;
        std::vector<unsigned int> nonempty_unsigned = {1u};
        std::vector<double> empty_double;
        std::vector<double> nonempty_double = {1.0};
        ARIADNE_TEST_PRINT(empty_int);
        ARIADNE_TEST_PRINT(nonempty_int);
        ARIADNE_TEST_PRINT(empty_unsigned);
        ARIADNE_TEST_PRINT(nonempty_unsigned);
        ARIADNE_TEST_PRINT(empty_double);
        ARIADNE_TEST_PRINT(nonempty_double);
    }

    void test() {
        ARIADNE_TEST_CALL(test_map_get());
        ARIADNE_TEST_CALL(test_map_convert());
        ARIADNE_TEST_CALL(test_map_restrict_keys());
        ARIADNE_TEST_CALL(test_make_list_of_set());
        ARIADNE_TEST_CALL(test_print_vector());
    }

};

int main() {
    TestContainer().test();
    return ARIADNE_TEST_FAILURES;
}
