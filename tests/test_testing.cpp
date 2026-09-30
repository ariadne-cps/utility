/***************************************************************************
 *            test_testing.cpp
 *
 *  Copyright  2026  Ariadne contributors
 ****************************************************************************/

#include <ostream>

#include "utility/test.hpp"

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

int main() {
    using TestLogic::Truth;
    using TestLogic::Value;

    ARIADNE_TEST_ASSERT(true);
    ARIADNE_TEST_ASSERT(Truth::True);
    ARIADNE_TEST_ASSERT(Truth::Indeterminate);

    Value one{1};
    Value two{2};

    ARIADNE_TEST_CHECK(one,one);
    ARIADNE_TEST_SAME(one,one);
    ARIADNE_TEST_EQUAL(one,one);
    ARIADNE_TEST_NOT_EQUAL(one,two);
    ARIADNE_TEST_EQUALS(one,one);
    ARIADNE_TEST_LESS(one,two);
    ARIADNE_TEST_UNARY_PREDICATE(TestLogic::is_positive,one);
    ARIADNE_TEST_BINARY_PREDICATE(TestLogic::same_value,one,one);
    ARIADNE_TEST_COMPARE(one,<,two);
    ARIADNE_TEST_WITHIN(one,two,1);

    return ARIADNE_TEST_FAILURES;
}
