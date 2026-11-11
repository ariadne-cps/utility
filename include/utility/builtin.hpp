/***************************************************************************
 *            utility/builtin.hpp
 *
 *  Copyright  2013-20  Pieter Collins
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

/** @file utility/builtin.hpp
 *  @brief Wrappers for built-in integral and floating-point values.
 */

#ifndef ARIADNE_UTILITY_BUILTIN_HPP
#define ARIADNE_UTILITY_BUILTIN_HPP

#include "utility/metaprogramming.hpp"

namespace Ariadne {

/** @brief Wrapper for a built-in unsigned integer. */
class builtin_uint {
    long unsigned int _m;

  public:
    /** @brief Convert to the built-in unsigned integer wrapper. */
    template<BuiltinUnsigned M>
    builtin_uint(M m) : _m(m) {}
    /** @brief Convert to long unsigned int. */
    explicit operator long unsigned int() const { return _m; }
    /** @brief Convert to a built-in unsigned long integer. */
    long unsigned int get_ui() const { return _m; }
};

/** @brief Wrapper for a built-in signed integer. */
class builtin_int {
    long int _n;

  public:
    /** @brief Convert to the built-in signed integer wrapper. */
    template<BuiltinIntegral N>
    builtin_int(N n) : _n(n) {}
    /** @brief Convert to long int. */
    explicit operator long int() const { return _n; }
    /** @brief Convert to a built-in signed long integer. */
    long int get_si() const { return _n; }
};

/** @brief Wrapper for a built-in double-precision value. */
class builtin_double {
    double _x;

  public:
    /** @brief Convert to the built-in double wrapper. */
    template<BuiltinFloatingPoint X>
    builtin_double(X x) : _x(x) {}
    /** @brief Convert to double. */
    explicit operator double() const { return _x; }
    /** @brief Convert to a built-in double. */
    double get_d() const { return _x; }
};

/** @brief Double-precision value treated as an exact binary number. */
class exact_double {
    double _x;

  private:
    /** @brief Convert to the exact double wrapper. */
    exact_double(double x) : _x(x) {}

  public:
    /** @brief Convert to the exact representation. */
    friend exact_double cast_exact(double);
    /** @brief Convert to the exact double wrapper. */
    exact_double() : _x(0.0) {}
    /** @brief Convert to double. */
    explicit operator double() const { return _x; }
    /** @brief Convert to a built-in double. */
    double get_d() const { return _x; }
};

/** @brief Convert to the exact representation. */
exact_double cast_exact(double x) { return exact_double(x); }

} // namespace Ariadne

#endif

