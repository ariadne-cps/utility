/***************************************************************************
 *            typedefs.hpp
 *
 *  Copyright  2013-26  Pieter Collins
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

#ifndef ARIADNE_UTILITY_TYPEDEFS_HPP
#define ARIADNE_UTILITY_TYPEDEFS_HPP

#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <sstream>
#include <string>
#include <memory>
#include <initializer_list>
#include <utility>
#include <tuple>

namespace Ariadne::Utility {

using uchar = unsigned char;
using uint = unsigned int;

using OutputStream = std::ostream;
using InputStream = std::istream;
using StringStream = std::stringstream;

using Void = void;
using Bool = bool;
using Char = char;
using Byte = std::int8_t;
using Nat = uint;
using Int = int;
using Dbl = double;
using Double = double;
using StringType = std::string;

using SizeType = std::size_t;
using PointerDifferenceType = std::ptrdiff_t;
using CounterType = std::uint32_t;
using DegreeType = std::uint16_t;
using DimensionType = SizeType;

#if (defined __arm || defined __aarch64__)
using ComparableEnumerationType = short;
#else
using ComparableEnumerationType = char;
#endif

template<class T> class Array;

template<class T> using UniquePointer = std::unique_ptr<T>;
template<class T> using SharedPointer = std::shared_ptr<T>;
template<class T> using InitializerList = std::initializer_list<T>;
template<class T1, class T2> using Pair = std::pair<T1,T2>;
template<class... TS> using Tuple = std::tuple<TS...>;

struct SizeOne { operator SizeType() const { return 1u; } };
struct IndexZero { operator SizeType() const { return 0u; } };

} // namespace Ariadne::Utility

#endif /* ARIADNE_UTILITY_TYPEDEFS_HPP */
