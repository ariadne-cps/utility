/***************************************************************************
 *            test_typedefs.cpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

#include "utility/typedefs.hpp"
#include "macro/testing.hpp"

using namespace Ariadne;

Int main() {
    ARIADNE_TEST_EQUALS(static_cast<SizeType>(SizeOne()),1u)
    ARIADNE_TEST_EQUALS(static_cast<SizeType>(IndexZero()),0u)
    return ARIADNE_TEST_FAILURES;
}
