/***************************************************************************
 *            range.hpp
 *
 *  Copyright  2023  Pieter Collins
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

/*! \file range.hpp
 *  \brief Index ranges.
 */

#ifndef ARIADNE_UTILITY_RANGE_HPP
#define ARIADNE_UTILITY_RANGE_HPP

#include <cstddef>

namespace Ariadne {

//! \ingroup LinearAlgebraModule
//! \brief A range of integer values from a \em start value up to, but not including, a \em stop value.
class Range {
    size_t _start; size_t _stop;
  public:
    Range(size_t start, size_t stop) : _start(start), _stop(stop) { } //!< .
    size_t operator[](size_t i) const { return _start+i; } //!< .
    size_t size() const { return this->_stop-this->_start; } //!< .
    size_t start() const { return this->_start; } //!< .
    size_t stop() const { return this->_stop; } //!< .
    size_t stride() const { return 1u; } //!< Always equal to \a 1.
};
inline Range range(size_t stop) { return Range(0u,stop); } //!< \relates Range
inline Range range(size_t start, size_t stop) { return Range(start,stop); } //!< \relates Range

struct RangeIterator {
    explicit inline RangeIterator(size_t i) : _i(i) { }
    inline RangeIterator& operator++() { ++this->_i; return *this; }
    inline size_t operator*() const { return this->_i; }
    friend inline bool operator!=(RangeIterator iter1, RangeIterator iter2) { return iter1._i != iter2._i; }
  private:
    size_t _i;
};
inline RangeIterator begin(Range rng) { return RangeIterator(rng.start()); }
inline RangeIterator end(Range rng) { return RangeIterator(rng.stop()); }

} // namespace Ariadne

#endif // ARIADNE_UTILITY_RANGE_HPP
