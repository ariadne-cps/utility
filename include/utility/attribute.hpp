/***************************************************************************
 *            attribute.hpp
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

/*! \file attribute.hpp
 *  \brief Generic attributes for named parameters.
 */

#ifndef ARIADNE_UTILITY_ATTRIBUTE_HPP
#define ARIADNE_UTILITY_ATTRIBUTE_HPP

namespace Ariadne::Utility {

template<class T> class Generator {
    using V = typename T::Type;
  public:
    inline T operator=(V const& v) const;
};

template<class V> class Attribute {
    template<class T> friend class Generator;
  private:
    V _v;
  protected:
    explicit Attribute(V const& v) : _v(v) { }
  public:
    using Type = V;
    operator V() const { return this->_v; }
    V value() const { return this->_v; }
};

template<class T>
inline T Generator<T>::operator=(typename T::Type const& v) const {
    Attribute<typename T::Type> attr(v);
    return static_cast<T const&>(attr);
}

} // namespace Ariadne::Utility

#endif /* ARIADNE_UTILITY_ATTRIBUTE_HPP */
