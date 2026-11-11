/***************************************************************************
 *            utility/prototype.hpp
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

/** @file utility/prototype.hpp
 *  @brief Prototype-based conversion helpers.
 */

#ifndef ARIADNE_UTILITY_PROTOTYPE_HPP
#define ARIADNE_UTILITY_PROTOTYPE_HPP

#include <utility>

namespace Ariadne {

/** @brief Representative value used to construct objects with matching characteristics. */
template<class T>
class Prototype {
    T _prototype;

  public:
    /** @brief Construct a \c Prototype from t. */
    Prototype(T t) : _prototype(t) {}
    /** @brief Convert the supplied value to the requested representation. */
    template<class X>
    X convert(const X& x) { return _prototype.convert(x); }
};
/** @brief Create a prototype preserving the supplied value characteristics. */
template<class T>
inline Prototype<T> make_prototype(const T& t) {
    return Prototype<T>(t);
}

} // namespace Ariadne

#endif
