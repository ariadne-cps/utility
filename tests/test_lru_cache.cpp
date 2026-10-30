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

#include "bootstrap.hpp"

using namespace Ariadne;

using CacheType = LRUCache<String,int>;

class TestLRUCache {
  public:

    void test_construct() {
        bootstrap::throws([&] { CacheType(0); }, "CacheType(0)");
        CacheType cache(1);
        bootstrap::equal((cache.current_size()), (0), "cache.current_size(),0");
        bootstrap::equal((cache.maximum_size()), (1), "cache.maximum_size(),1");
    }

    void test_find() {
        CacheType cache(2);
        bootstrap::require((not cache.has_label("something")), "not cache.has_label(\"something\")");
    }

    void test_get_failure() {
        CacheType cache(2);
        bootstrap::throws([&] { cache.get("something"); }, "cache.get(\"something\")");
        bootstrap::throws([&] { cache.age("something"); }, "cache.age(\"something\")");
    }

    void test_string() {
        std::string std_string = "text";
        String string(std_string);
        bootstrap::require((std::string(c_str(string)) == "text"), "std::string(c_str(string)) == \"text\"");
        bootstrap::require((to_string(42) == "42"), "to_string(42) == \"42\"");
        bootstrap::require((to_str(43) == "43"), "to_str(43) == \"43\"");
        bootstrap::require((class_name<String>() == "String"), "class_name<String>() == \"String\"");
    }

    void test_put_single() {
        CacheType cache(2);
        cache.put("first",42);
        bootstrap::equal((cache.current_size()), (1), "cache.current_size(),1");
        bootstrap::equal((cache.age("first")), (0), "cache.age(\"first\"),0");
        auto val = cache.get("first");
        bootstrap::equal((val), (42), "val,42");
    }

    void test_put_multiple() {
        CacheType cache(2);
        cache.put("first",42);
        bootstrap::throws([&] { cache.put("first",10); }, "cache.put(\"first\",10)");
        cache.put("second",10);
        bootstrap::equal((cache.current_size()), (2), "cache.current_size(),2");
        bootstrap::equal((cache.age("first")), (1), "cache.age(\"first\"),1");
        bootstrap::equal((cache.age("second")), (0), "cache.age(\"second\"),0");
    }

    void test_put_multiple_over() {
        CacheType cache(3);
        cache.put("first",42);
        cache.put("second",10);
        cache.put("third",5);
        cache.put("fourth",12);
        bootstrap::equal((cache.current_size()), (3), "cache.current_size(),3");
        bootstrap::require((not cache.has_label("first")), "not cache.has_label(\"first\")");
        bootstrap::equal((cache.age("second")), (2), "cache.age(\"second\"),2");
        bootstrap::equal((cache.age("third")), (1), "cache.age(\"third\"),1");
        bootstrap::equal((cache.age("fourth")), (0), "cache.age(\"fourth\"),0");
    }

    void test_get() {
        CacheType cache(4);
        cache.put("first",42);
        cache.put("second",10);
        cache.put("third",5);
        cache.put("fourth",12);
        cache.get("second");
        bootstrap::equal((cache.age("second")), (0), "cache.age(\"second\"),0");
        bootstrap::equal((cache.age("first")), (3), "cache.age(\"first\"),3");
        bootstrap::equal((cache.age("third")), (2), "cache.age(\"third\"),2");
        bootstrap::equal((cache.age("fourth")), (1), "cache.age(\"fourth\"),1");
    }

    void test() {
        bootstrap::call([&] { test_construct(); }, "test_construct()");
        bootstrap::call([&] { test_find(); }, "test_find()");
        bootstrap::call([&] { test_get_failure(); }, "test_get_failure()");
        bootstrap::call([&] { test_string(); }, "test_string()");
        bootstrap::call([&] { test_put_single(); }, "test_put_single()");
        bootstrap::call([&] { test_put_multiple(); }, "test_put_multiple()");
        bootstrap::call([&] { test_put_multiple_over(); }, "test_put_multiple_over()");
        bootstrap::call([&] { test_get(); }, "test_get()");
    }

};

int main() {
    TestLRUCache().test();
    return bootstrap::failures;
}
