/***************************************************************************
 *            ariadne-utility.hpp
 *
 *  Copyright  2026  Luca Geretti
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

#ifndef ARIADNE_UTILITY_UMBRELLA_HPP
#define ARIADNE_UTILITY_UMBRELLA_HPP

// Utility has no public runtime submodule dependencies; Build is CMake
// infrastructure and Testing is used only by the test suite.
#include "utility/module.hpp"

#include "utility/attribute.hpp"
#include "utility/binary_word.hpp"
#include "utility/dispatching.hpp"
#include "utility/exceptions.hpp"
#include "utility/iterator.hpp"
#include "utility/lazy.hpp"
#include "utility/lru_cache.hpp"
#include "utility/path.hpp"
#include "utility/pointer.hpp"
#include "utility/prototype.hpp"
#include "utility/randomiser.hpp"
#include "utility/range.hpp"
#include "utility/stack_trace.hpp"
#include "utility/stopwatch.hpp"
#include "utility/uniform_array.hpp"
#include "utility/uniform_list.hpp"
#include "utility/variant.inl.hpp"

#endif // ARIADNE_UTILITY_UMBRELLA_HPP
