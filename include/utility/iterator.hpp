/***************************************************************************
 *            utility/iterator.hpp
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

/** @file utility/iterator.hpp
 *  @brief Iterator support, similar to Boost's iterator package.
 */

#ifndef ARIADNE_UTILITY_ITERATOR_HPP
#define ARIADNE_UTILITY_ITERATOR_HPP

#include <iterator>

namespace Ariadne {

using std::ostream;

/** @brief Tag selecting random access traversal. */
struct RandomAccessTraversalTag {};
/** @brief Tag selecting forward traversal. */
struct ForwardTraversalTag {};

/** @brief Iterator implementation built from a small set of core operations. */
template<class I, class Val, class Cat, class Ref = Val&>
class IteratorFacade;

/** @brief Access bridge from iterator operators to private core operations. */
class IteratorCoreAccess {
    template<class I, class Val, class Cat, class Ref>
    friend class IteratorFacade;

    /** @brief Test equality with the supplied iterator or value. */
    template<class I, class II>
    static bool equal(const I& self, II const& other) { return self.equal(other); }
    /** @brief Return the signed distance to the supplied iterator. */
    template<class I, class II>
    static std::ptrdiff_t distance_to(const I& self, II const& other) { return self.distance_to(other); }
    /** @brief Advance to the next iterator position or index. */
    template<class I>
    static void increment(I& self) { self.increment(); }
    /** @brief Move the iterator by the specified offset. */
    template<class I>
    static void advance(I& self, std::ptrdiff_t n) { self.advance(n); }
    /** @brief Return the value at the current iterator position. */
    template<class I>
    static typename I::reference dereference(const I& self) { return self.dereference(); }
};

/** @brief Iterator implementation built from a small set of core operations. */
template<class I, class Val, class Ref>
class IteratorFacade<I, Val, RandomAccessTraversalTag, Ref> {
    typedef Val* Ptr;
    typedef RandomAccessTraversalTag Cat;

  private:
    /** @brief Test equality with the supplied iterator or value. */
    static bool _equal(const I& self, const I& other) { return IteratorCoreAccess::equal(self, other); }
    /** @brief Return the signed distance to the supplied iterator. */
    static std::ptrdiff_t _distance_to(const I& self, const I& other) { return IteratorCoreAccess::distance_to(self, other); }

  public:
    // Standard typedefs
    typedef typename std::remove_const<Val>::type value_type;
    typedef Ref reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;
    typedef Val* pointer;

    // Ariadne typedefs
    typedef typename std::remove_const<Val>::type ValueType;
    typedef Ref Reference;

    /** @brief Test equality of the operands. */
    friend bool operator==(const I& self, const I& other) { return _equal(self, other); }
    /** @brief Test inequality of the operands. */
    friend bool operator!=(const I& self, const I& other) { return !_equal(self, other); }
    /** @brief Test whether the first operand is less than the second. */
    friend bool operator<(const I& self, const I& other) { return _distance_to(self, other) > 0; }
    /** @brief Test whether the first operand is less than or equal to the second. */
    friend bool operator<=(const I& self, const I& other) { return _distance_to(self, other) >= 0; }
    /** @brief Test whether the first operand is greater than the second. */
    friend bool operator>(const I& self, const I& other) { return _distance_to(self, other) < 0; }
    /** @brief Test whether the first operand is greater than or equal to the second. */
    friend bool operator>=(const I& self, const I& other) { return _distance_to(self, other) <= 0; }
    /** @brief Return the difference or additive inverse. */
    std::ptrdiff_t operator-(const I& other) const { return -IteratorCoreAccess::distance_to(static_cast<const I&>(*this), other); }
    /** @brief Return the sum or unary positive value. */
    I operator+(std::ptrdiff_t n) const {
        I result(static_cast<const I&>(*this));
        IteratorCoreAccess::advance(result, n);
        return result;
    }
    /** @brief Return the difference or additive inverse. */
    I operator-(std::ptrdiff_t n) const {
        I result(static_cast<const I&>(*this));
        IteratorCoreAccess::advance(result, -n);
        return result;
    }
    /** @brief Add the operand in place. */
    I& operator+=(std::ptrdiff_t n) {
        IteratorCoreAccess::advance(static_cast<I&>(*this), n);
        return static_cast<I&>(*this);
    }
    /** @brief Subtract the operand in place. */
    I& operator-=(std::ptrdiff_t n) {
        IteratorCoreAccess::advance(static_cast<I&>(*this), -n);
        return static_cast<I&>(*this);
    }
    /** @brief Increment the value or iterator. */
    I& operator++() {
        IteratorCoreAccess::advance(static_cast<I&>(*this), 1);
        return static_cast<I&>(*this);
    }
    /** @brief Decrement the value or iterator. */
    I& operator--() {
        IteratorCoreAccess::advance(static_cast<I&>(*this), -1);
        return static_cast<I&>(*this);
    }
    /** @brief Return the value referenced by the iterator or pointer. */
    Ref operator*() const { return IteratorCoreAccess::dereference(static_cast<const I&>(*this)); }
    /** @brief Return the selected element or component. */
    Ref operator[](std::ptrdiff_t n) const {
        I result(static_cast<const I&>(*this));
        IteratorCoreAccess::advance(result, n);
        return IteratorCoreAccess::dereference(result);
    }
    /** @brief Return a pointer to the referenced object. */
    Ptr operator->() const { return &IteratorCoreAccess::dereference(static_cast<const I&>(*this)); }
};

/** @brief Iterator implementation built from a small set of core operations. */
template<class I, class Val, class Ref>
class IteratorFacade<I, Val, ForwardTraversalTag, Ref> {
    typedef Val* Ptr;

  private:
    /** @brief Test equality with the supplied iterator or value. */
    static bool _equal(const I& self, const I& other) { return IteratorCoreAccess::equal(self, other); }

  public:
    // Standard typedefs
    typedef typename std::remove_const<Val>::type value_type;
    typedef Ref reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;
    typedef Val* pointer;

    // Ariadne typedefs
    typedef typename std::remove_const<Val>::type ValueType;
    typedef Ref Reference;

    /** @brief Test equality of the operands. */
    friend bool operator==(const I& self, const I& other) { return _equal(self, other); }
    /** @brief Test inequality of the operands. */
    friend bool operator!=(const I& self, const I& other) { return !_equal(self, other); }
    /** @brief Increment the value or iterator. */
    I& operator++() {
        IteratorCoreAccess::increment(static_cast<I&>(*this));
        return static_cast<I&>(*this);
    }
    /** @brief Increment the value or iterator. */
    I operator++(int) {
        I r = static_cast<const I&>(*this);
        IteratorCoreAccess::increment(static_cast<I&>(*this));
        return r;
    }
    /** @brief Return the value referenced by the iterator or pointer. */
    Ref operator*() const { return IteratorCoreAccess::dereference(static_cast<const I&>(*this)); }
    /** @brief Return a pointer to the referenced object. */
    Ptr operator->() const { return &IteratorCoreAccess::dereference(static_cast<const I&>(*this)); }
};

/** @brief Iterator over pair entries. */
template<class I1, class I2>
class PairIterator
//    : public IteratorFacade<PairIterator<I1,I2>, MultivariateMonomial<X>, RandomAccessTag, MultivariateMonomialReference<X>>
{
    typedef std::pair<I1*, I2*> Reference;

  public:
    I1 _iter1;
    I2 _iter2;

  public:
    typedef std::ptrdiff_t DifferenceType;

    /** @brief Construct a \c PairIterator from iter1, iter2. */
    PairIterator(I1 iter1, I2 iter2);
    /** @brief Copy-construct the object from the supplied instance. */
    template<class II1, class II2>
    PairIterator(const PairIterator<II1, II2>&);

  public:
    /** @brief Test equality with the supplied iterator or value. */
    template<class II1, class II2>
    bool equal(const PairIterator<II1, II2>&) const;
    /** @brief Return the signed distance to the supplied iterator. */
    template<class II1, class II2>
    DifferenceType distance_to(const PairIterator<II1, II2>&) const;
    /** @brief Move the iterator by the specified offset. */
    void advance(DifferenceType k);
    /** @brief Return the value at the current iterator position. */
    Reference dereference();
};
template<class I1, class I2>
inline PairIterator<I1, I2>::PairIterator(I1 iter1, I2 iter2)
    : _iter1(iter1), _iter2(iter2) {}
template<class I1, class I2>
template<class II1, class II2>
inline PairIterator<I1, I2>::PairIterator(const PairIterator<II1, II2>& iter)
    : _iter1(iter._iter1), _iter2(iter._iter2) {}
template<class I1, class I2>
template<class II1, class II2>
inline bool PairIterator<I1, I2>::equal(const PairIterator<II1, II2>& other) const {
    return this->_iter1 == other._iter1 && this->_iter2 == other._iter2;
}
template<class I1, class I2>
inline void PairIterator<I1, I2>::advance(DifferenceType k) {
    _iter1 += (k), _iter2 += (k);
}
template<class I1, class I2>
inline auto PairIterator<I1, I2>::dereference() -> Reference {
    return std::make_pair(*this->_iter1, *this->_iter2);
}
/** @brief Write the value to the output stream. */
template<class I1, class I2>
ostream& operator<<(ostream& os, const PairIterator<I1, I2>& e) {
    return os << "{" << e._iter1 << "," << e._iter2 << "}";
}

} // namespace Ariadne

#endif /* ARIADNE_UTILITY_ITERATOR_HPP */
