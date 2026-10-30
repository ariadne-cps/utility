/***************************************************************************
 *            test_lru_cache.cpp
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

#include "utility/string.hpp"
#include "utility/lru_cache.hpp"

#include "testing/fixture.hpp"

using namespace Ariadne;

using CacheType = LRUCache<String,int>;

class TestLRUCache : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;


    void test_construct() {
        ctx().expect_exception([&] { CacheType(0); }, "CacheType(0)");
        CacheType cache(1);
        ctx().expect_equal((cache.current_size()), (0), "cache.current_size(),0");
        ctx().expect_equal((cache.maximum_size()), (1), "cache.maximum_size(),1");
    }

    void test_find() {
        CacheType cache(2);
        ctx().prove((not cache.has_label("something")), "not cache.has_label(\"something\")");
    }

    void test_get_failure() {
        CacheType cache(2);
        ctx().expect_exception([&] { cache.get("something"); }, "cache.get(\"something\")");
        ctx().expect_exception([&] { cache.age("something"); }, "cache.age(\"something\")");
    }

    void test_string() {
        std::string std_string = "text";
        String string(std_string);
        ctx().prove((std::string(c_str(string)) == "text"), "std::string(c_str(string)) == \"text\"");
        ctx().prove((to_string(42) == "42"), "to_string(42) == \"42\"");
        ctx().prove((to_str(43) == "43"), "to_str(43) == \"43\"");
        ctx().prove((class_name<String>() == "String"), "class_name<String>() == \"String\"");
    }

    void test_put_single() {
        CacheType cache(2);
        cache.put("first",42);
        ctx().expect_equal((cache.current_size()), (1), "cache.current_size(),1");
        ctx().expect_equal((cache.age("first")), (0), "cache.age(\"first\"),0");
        auto val = cache.get("first");
        ctx().expect_equal((val), (42), "val,42");
    }

    void test_put_multiple() {
        CacheType cache(2);
        cache.put("first",42);
        ctx().expect_exception([&] { cache.put("first",10); }, "cache.put(\"first\",10)");
        cache.put("second",10);
        ctx().expect_equal((cache.current_size()), (2), "cache.current_size(),2");
        ctx().expect_equal((cache.age("first")), (1), "cache.age(\"first\"),1");
        ctx().expect_equal((cache.age("second")), (0), "cache.age(\"second\"),0");
    }

    void test_put_multiple_over() {
        CacheType cache(3);
        cache.put("first",42);
        cache.put("second",10);
        cache.put("third",5);
        cache.put("fourth",12);
        ctx().expect_equal((cache.current_size()), (3), "cache.current_size(),3");
        ctx().prove((not cache.has_label("first")), "not cache.has_label(\"first\")");
        ctx().expect_equal((cache.age("second")), (2), "cache.age(\"second\"),2");
        ctx().expect_equal((cache.age("third")), (1), "cache.age(\"third\"),1");
        ctx().expect_equal((cache.age("fourth")), (0), "cache.age(\"fourth\"),0");
    }

    void test_get() {
        CacheType cache(4);
        cache.put("first",42);
        cache.put("second",10);
        cache.put("third",5);
        cache.put("fourth",12);
        cache.get("second");
        ctx().expect_equal((cache.age("second")), (0), "cache.age(\"second\"),0");
        ctx().expect_equal((cache.age("first")), (3), "cache.age(\"first\"),3");
        ctx().expect_equal((cache.age("third")), (2), "cache.age(\"third\"),2");
        ctx().expect_equal((cache.age("fourth")), (1), "cache.age(\"fourth\"),1");
    }

};

int main() {
    Ariadne::Testing::Suite suite("lru_cache");
    ARIADNE_ADD_TEST(suite, TestLRUCache, test_construct);
    ARIADNE_ADD_TEST(suite, TestLRUCache, test_find);
    ARIADNE_ADD_TEST(suite, TestLRUCache, test_get_failure);
    ARIADNE_ADD_TEST(suite, TestLRUCache, test_string);
    ARIADNE_ADD_TEST(suite, TestLRUCache, test_put_single);
    ARIADNE_ADD_TEST(suite, TestLRUCache, test_put_multiple);
    ARIADNE_ADD_TEST(suite, TestLRUCache, test_put_multiple_over);
    ARIADNE_ADD_TEST(suite, TestLRUCache, test_get);
    Ariadne::Testing::ConsoleReporter reporter(std::cout);
    return suite.run(reporter).successful() ? 0 : 1;
}
