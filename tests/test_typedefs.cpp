/***************************************************************************
 *            test_typedefs.cpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

#include "utility/typedefs.hpp"
#include "testing/testing.hpp"
#include <iostream>

using namespace Ariadne;

int main() {
    Ariadne::Testing::Suite suite("typedefs");
    suite.add("typedef conversions", [&](Ariadne::Testing::Context& ctx) {
        ctx.expect_equal(static_cast<Ariadne::SizeType>(Ariadne::SizeOne()), 1u);
        ctx.expect_equal(static_cast<Ariadne::SizeType>(Ariadne::IndexZero()), 0u);
    });
    Ariadne::Testing::ConsoleReporter reporter(std::cout);
    return suite.run(reporter).successful() ? 0 : 1;
}
