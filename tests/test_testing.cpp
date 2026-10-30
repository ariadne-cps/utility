/***************************************************************************
 *            test_testing.cpp
 *
 *  Copyright  2026  Luca Geretti
 *  Author     Luca Geretti
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne Utility. If not, see <https://www.gnu.org/licenses/>.
 */

#include "testing/testing.hpp"
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace TestLogic {

enum class Truth { False, Indeterminate, True };

inline bool definitely(Truth t) { return t == Truth::True; }
inline bool possibly(Truth t) { return t != Truth::False; }
inline bool decide(Truth t) { return t == Truth::True; }

struct Value {
    int value;
};

inline std::ostream& operator<<(std::ostream& os, Value v) {
    return os << v.value;
}

inline Truth operator==(Value lhs, Value rhs) {
    return lhs.value == rhs.value ? Truth::True : Truth::False;
}

inline Truth operator<(Value lhs, Value rhs) {
    return lhs.value < rhs.value ? Truth::True : Truth::False;
}

inline Truth same(Value lhs, Value rhs) {
    return lhs == rhs;
}

inline Truth is_positive(Value value) {
    return value.value > 0 ? Truth::True : Truth::False;
}

inline Truth same_value(Value lhs, Value rhs) {
    return lhs == rhs;
}

struct Difference {
    int value;
};

inline Difference operator-(Value lhs, Value rhs) {
    return Difference{lhs.value-rhs.value};
}

inline int mag(Difference difference) {
    return difference.value < 0 ? -difference.value : difference.value;
}

} // namespace TestLogic


struct CaptureReporter final : Ariadne::Testing::Reporter {
    std::vector<Ariadne::Testing::CaseResult> cases;
    void case_finished(Ariadne::Testing::CaseResult const& r) override { cases.push_back(r); }
    void suite_finished(std::string_view, Ariadne::Testing::Summary const&) override { }
};

int main() {
    using namespace Ariadne::Testing;
    using TestLogic::Value;
    using TestLogic::Truth;
    Value one{1}, two{2};
    Suite suite("validated semantics");
    suite.add("validated true", [&](Context& t) {
        t.prove(Truth::True, "true proposition");
        t.expect_equal(one, one);
        t.expect_same(one, one);
        t.prove(one < two, "strict less");
        t.prove(TestLogic::is_positive(one), "unary predicate");
        t.prove(TestLogic::same_value(one, one), "binary predicate");
        t.expect_within(one, two, 1);
        t.prove(one.value != two.value, "different values");
    });
    suite.add("validated false", [](Context& t) { t.prove(Truth::False, "refutation"); });
    suite.add("validated indeterminate", [](Context& t) { t.prove(Truth::Indeterminate, "undecidable"); });
    CaptureReporter results;
    Summary const summary = suite.run(results);
    if (summary.passed != 1 || summary.failed != 1 || summary.inconclusive != 1
        || summary.errors != 0 || summary.skipped != 0) return 1;
    if (results.cases.size() != 3) return 2;
    if (results.cases[0].assertions.size() != 8) return 3;
    if (results.cases[2].outcome != Outcome::Inconclusive) return 4;
    return 0;
}
