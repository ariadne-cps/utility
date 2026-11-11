/***************************************************************************
 *            utility/exceptions.hpp
 *
 *  Copyright  2008-20  Pieter Collins
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

/** @file utility/exceptions.hpp
 *  @brief Exception classes
 */

#ifndef ARIADNE_UTILITY_EXCEPTIONS_HPP
#define ARIADNE_UTILITY_EXCEPTIONS_HPP

#include <exception>
#include <stdexcept>

#include "utility/string.hpp"

namespace Ariadne {

/** @brief Exception reporting . */
class Exception {
    String _what;

  public:
    /** @brief Construct a \c Exception from what. */
    Exception(const String& what);
};

/** @brief Exception reporting an unavailable operation. */
class NotImplemented : public std::logic_error {
  public:
    /** @brief Construct a \c NotImplemented from str. */
    NotImplemented(const String& str) : std::logic_error(str) {}
};

/** @brief Exception reporting an invalid runtime type conversion. */
class BadCast : public std::bad_cast {
  public:
    /** @brief Construct a \c BadCast with the specified characteristics. */
    BadCast(const String&) : std::bad_cast() {}
};

/** @brief Exception reporting divide by zero. */
class DivideByZeroException : public std::runtime_error {
  public:
    /** @brief Construct a \c DivideByZeroException from str. */
    DivideByZeroException(const String& str) : std::runtime_error(str) {}
};

/** @brief Exception reporting domain. */
class DomainException : public std::runtime_error {
  public:
    /** @brief Construct a \c DomainException from str. */
    DomainException(const String& str) : std::runtime_error(str) {}
};

/** @brief Exception reporting incompatible argument or container dimensions. */
class IncompatibleSizes : public std::runtime_error {
  public:
    /** @brief Construct a \c IncompatibleSizes from str. */
    IncompatibleSizes(const String& str) : std::runtime_error(str) {}
};

/** @brief Exception reporting an invalid grid position. */
class InvalidGridPosition : public std::runtime_error {
  public:
    /** @brief Construct a \c InvalidGridPosition from str. */
    InvalidGridPosition(const String& str) : std::runtime_error(str) {}
};

/** @brief Exception reporting a coordinate outside its valid range. */
class InvalidCoordinate : public std::runtime_error {
  public:
    /** @brief Construct a \c InvalidCoordinate from str. */
    InvalidCoordinate(const String& str) : std::runtime_error(str) {}
};

/** @brief Exception reporting invalid input data. */
class InvalidInput : public std::runtime_error {
  public:
    /** @brief Construct a \c InvalidInput from str. */
    InvalidInput(const String& str) : std::runtime_error(str) {}
};

/** @brief Exception reporting overspill from the outer chain. */
class OuterChainOverspill : public std::runtime_error {
  public:
    /** @brief Construct a \c OuterChainOverspill from str. */
    OuterChainOverspill(const String& str) : std::runtime_error(str) {}
};

} // namespace Ariadne

#endif
