/***************************************************************************
 *            utility/attribute.hpp
 *
 *  Copyright  2011-26  Pieter Collins
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

/** @file utility/attribute.hpp
 *  @brief Generic attributes for named parameters.
 */

#ifndef ARIADNE_UTILITY_ATTRIBUTE_HPP
#define ARIADNE_UTILITY_ATTRIBUTE_HPP

#include "utility/typedefs.hpp"

namespace Ariadne {

/** @brief Indexed element generator used to construct containers. */
template<class T>
class Generator {
    using V = typename T::Type;

  public:
    /** @brief Assign the supplied value and return this object. */
    inline T operator=(const V &v) const;
};

/** @brief Strongly typed named attribute wrapping a value. */
template<class V>
class Attribute {
    template<class T>
    friend class Generator;

  private:
    V _v;

  protected:
    /** @brief Construct a \c Attribute from v. */
    explicit Attribute(const V &v) : _v(v) {}

  public:
    using Type = V;
    /** @brief Convert to v. */
    operator V() const { return this->_v; }
    /** @brief Return the stored value. */
    V value() const { return this->_v; }
};

template<class T>
inline T Generator<T>::operator=(const typename T::Type &v) const {
    Attribute<typename T::Type> attr(v);
    return static_cast<const T &>(attr);
}

/** @brief Strongly typed storage-capacity attribute. */
struct Capacity : Attribute<SizeType> {};
static const Generator<Capacity> capacity = Generator<Capacity>();
/** @brief Strongly typed element-count attribute. */
struct Size : Attribute<SizeType> {};
static const Generator<Size> size = Generator<Size>();
/** @brief Strongly typed result-dimension attribute. */
struct ResultSize : Attribute<SizeType> {};
static const Generator<ResultSize> result_size = Generator<ResultSize>();
/** @brief Strongly typed argument-dimension attribute. */
struct ArgumentSize : Attribute<SizeType> {};
static const Generator<ArgumentSize> argument_size = Generator<ArgumentSize>();
/** @brief Strongly typed polynomial-degree attribute. */
struct Degree : Attribute<DegreeType> {};
static const Generator<Degree> degree = Generator<Degree>();

} // namespace Ariadne

#endif /* ARIADNE_UTILITY_ATTRIBUTE_HPP */
