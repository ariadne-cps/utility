/***************************************************************************
 *            test_typedefs.cpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

#include "utility/typedefs.hpp"
#include "bootstrap.hpp"

using namespace Ariadne;

Int main() {
    bootstrap::equal((static_cast<SizeType>(SizeOne())), (1u), "static_cast<SizeType>(SizeOne()),1u");
    bootstrap::equal((static_cast<SizeType>(IndexZero())), (0u), "static_cast<SizeType>(IndexZero()),0u");
    return bootstrap::failures;
}
