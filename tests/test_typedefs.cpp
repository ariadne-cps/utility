/***************************************************************************
 *            test_typedefs.cpp
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

#include "utility/typedefs.hpp"
#include "testing/checks.hpp"
#include <iostream>

using namespace Ariadne;

int main() {
    Ariadne::Testing::Suite suite("typedefs");
    suite.add("typedef conversions", [&](Ariadne::Testing::Context& ctx) {
        ARIADNE_EXPECT_EQUAL(ctx, static_cast<Ariadne::SizeType>(Ariadne::SizeOne()), 1u);
        ARIADNE_EXPECT_EQUAL(ctx, static_cast<Ariadne::SizeType>(Ariadne::IndexZero()), 0u);
    });
    Ariadne::Testing::ConsoleReporter reporter(std::cout);
    return suite.run(reporter).successful() ? 0 : 1;
}
