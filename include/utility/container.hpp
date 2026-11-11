/***************************************************************************
 *            utility/container.hpp
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

/** @file utility/container.hpp
 *  @brief Container adapters, lists, and container output helpers.
 */

#ifndef ARIADNE_UTILITY_CONTAINER_HPP
#define ARIADNE_UTILITY_CONTAINER_HPP

#include "utility/array.hpp"
#include "utility/macros.hpp"
#include "utility/metaprogramming.hpp"
#include "utility/stdlib.hpp"
#include "utility/stlio.hpp"

namespace Ariadne {

using std::make_pair;
using std::make_tuple;
using std::size_t;

/** @brief Write the value to the output stream. */
template<class T>
ostream& operator<<(ostream& os, const Array<T>& a) {
    bool first = true;
    for (auto x: a) {
        os << (first ? "[" : ",") << x;
        first = false;
    }
    if (first) {
        os << "[";
    }
    return os << "]";
}

/** @brief Write the value to the output stream. */
template<class T>
ostream& operator<<(ostream& os, const SharedArray<T>& a) {
    bool first = true;
    for (auto x: a) {
        os << (first ? "[" : ",") << x;
        first = false;
    }
    if (first) {
        os << "[";
    }
    return os << "]";
}

/** @brief Sequence container extending the standard vector interface. */
template<class T>
class List : public std::vector<T> {
  public:
    typedef typename std::vector<T>::value_type ValueType;
    typedef typename std::vector<T>::reference Reference;
    typedef typename std::vector<T>::const_reference ConstReference;
    typedef typename std::vector<T>::pointer Pointer;
    typedef typename std::vector<T>::const_pointer ConstPointer;
    typedef typename std::vector<T>::iterator Iterator;
    typedef typename std::vector<T>::const_iterator ConstIterator;
    using std::vector<T>::vector;
    /** @brief Construct a \c List with the specified characteristics. */
    List() : std::vector<T>() {}
    /** @brief Construct a \c List from lst. */
    List(const std::vector<T>& lst) : std::vector<T>(lst) {}
    /** @brief Construct a \c List from t. */
    List(const T& t) : std::vector<T>(1u, t) {}
    /** @brief Construct a \c List from l. */
    template<ConvertibleTo<T> TT>
    List(const std::vector<TT>& l) : std::vector<T>(l.begin(), l.end()) {}
    /** @brief Construct a \c List from l. */
    template<ExplicitlyConvertibleTo<T> TT>
    explicit List(const std::vector<TT>& l) : std::vector<T>(l.begin(), l.end()) {}
    /** @brief Append the supplied element or sequence. */
    void append(const T& t) { this->push_back(t); }
    /** @brief Append the supplied element or sequence. */
    void append(const std::vector<T>& t) {
        for (unsigned int i = 0; i != t.size(); ++i) {
            this->push_back(t[i]);
        }
    }
    /** @brief Return the concatenation of the sequences. */
    void concatenate(const std::vector<T>& t) {
        for (unsigned int i = 0; i != t.size(); ++i) {
            this->push_back(t[i]);
        }
    }
};
/** @brief Return the concatenation of the sequences. */
template<class T>
inline List<T> catenate(const List<T>& l1, const List<T>& l2) {
    List<T> r(l1);
    for (auto iter = l2.begin(); iter != l2.end(); ++iter) {
        r.append(*iter);
    }
    return r;
}
/** @brief Return the concatenation of the sequences. */
template<class T>
inline List<T> catenate(const List<T>& l1, const T& t2) {
    List<T> r(l1);
    r.append(t2);
    return r;
}
/** @brief Apply the supplied operation to the operands. */
template<class T, Invocable<T> F>
inline List<InvokeResult<F, T>> apply(F&& f, const List<T>& lst) {
    typedef InvokeResult<F, T> R;
    List<R> res;
    res.reserve(lst.size());
    for (auto itm: lst) {
        res.append(f(itm));
    }
    return res;
}

/** @brief Apply the supplied operation independently to corresponding elements. */
template<class F, class T>
List<ResultOf<F(T)>> elementwise(const F& f, const List<T>& l) {
    typedef ResultOf<F(T)> R;
    List<R> r;
    r.reserve(l.size());
    for (size_t i = 0; i != l.size(); ++i) {
        r.append(f(l[i]));
    }
    return r;
}

/** @brief Apply the supplied operation independently to corresponding elements. */
template<class F, class T1, class T2>
List<ResultOf<F(T1, T2)>> elementwise(const F& f, const List<T1>& l1, const List<T2>& l2) {
    typedef ResultOf<F(T1, T2)> R;
    assert(l1.size() == l2.size());
    List<R> r;
    r.reserve(l1.size());
    for (size_t i = 0; i != l1.size(); ++i) {
        r.append(f(l1[i], l2[i]));
    }
    return r;
}

/** @brief Write the value to the output stream. */
template<class T>
ostream& operator<<(ostream& os, const std::vector<T>& v) {
    bool first = true;
    for (auto x: v) {
        os << (first ? "[" : ",") << x;
        first = false;
    }
    if (first) {
        os << "[";
    }
    return os << "]";
}

/** @brief Sequence container extending the standard linked-list interface. */
template<class T>
class LinkedList
    : public std::list<T> {
  public:
    typedef typename std::list<T>::iterator Iterator;
    typedef typename std::list<T>::const_iterator ConstIterator;
    /** @brief Construct a \c LinkedList with the specified characteristics. */
    LinkedList() : std::list<T>() {}
    /** @brief Construct a \c LinkedList from n. */
    LinkedList(unsigned int n) : std::list<T>(n) {}
    /** @brief Construct a \c LinkedList from n, t. */
    LinkedList(unsigned int n, const T& t) : std::vector<T>(n, t) {}
    /** @brief Construct a \c LinkedList from l. */
    LinkedList(const std::list<T>& l) : std::list<T>(l) {}
    /** @brief Construct a \c LinkedList from l. */
    template<class X>
    LinkedList(const List<X>& l) : std::list<T>(l.begin(), l.end()) {}
    /** @brief Copy-construct the object from the supplied instance. */
    template<class X>
    LinkedList(const LinkedList<X>& l) : std::list<T>(l.begin(), l.end()) {}
    /** @brief Construct a \c LinkedList from b, e. */
    template<class I>
    LinkedList(const I& b, const I& e) : std::list<T>(b, e) {}
    // Conversion constructor from an value to a single-element list.
    /** @brief Append the supplied element or sequence. */
    void append(const T& t) { this->push_back(t); }
    /** @brief Append the supplied element or sequence. */
    void append(const LinkedList<T>& t) {
        for (unsigned int i = 0; i != t.size(); ++i) {
            this->push_back(t[i]);
        }
    }
    /** @brief Return the concatenation of the sequences. */
    void concatenate(const LinkedList<T>& t) {
        for (unsigned int i = 0; i != t.size(); ++i) {
            this->push_back(t[i]);
        }
    }
};
/** @brief Write the value to the output stream. */
template<class T>
inline ostream&
    operator<<(ostream& os, const std::list<T>& l) {
    return Ariadne::write_sequence(os, l.begin(), l.end());
}

/** @brief Ordered unique-element container with set operations. */
template<class T>
class Set : public std::set<T> {
  public:
    typedef typename std::set<T>::iterator Iterator;
    typedef typename std::set<T>::const_iterator ConstIterator;
    using std::set<T>::set;
    /** @brief Construct a \c Set with the specified characteristics. */
    Set() : std::set<T>() {}
    /** @brief Construct a \c Set from s. */
    template<ConvertibleTo<T> TT>
    Set(const InitializerList<TT>& s) : std::set<T>(s.begin(), s.end()) {}
    /** @brief Construct a \c Set from s. */
    template<ConvertibleTo<T> TT>
    Set(const std::vector<TT>& s) : std::set<T>(s.begin(), s.end()) {}
    /** @brief Construct a \c Set from s. */
    template<ConvertibleTo<T> TT>
    Set(const std::set<TT>& s) : std::set<T>(s.begin(), s.end()) {}
    /** @brief Construct a \c Set from s. */
    template<ExplicitlyConvertibleTo<T> TT>
    explicit Set(const std::set<TT>& s) {
        for (auto x: s) {
            this->insert(T(x));
        }
    }
    /** @brief Convert to list< t >. */
    explicit operator List<T>() const { return List<T>(this->begin(), this->end()); }
    /** @brief Test containment of the supplied value. */
    bool contains(const T& t) const {
        return this->find(t) != this->end();
    }
    /** @brief Test whether the first set is contained in the second. */
    bool subset(const std::set<T>& s) const {
        for (auto iter = s.begin(); iter != s.end(); ++iter) {
            if (!this->contains(*iter)) {
                return false;
            }
        }
        return true;
    }
    /** @brief Test whether the sets have no common elements. */
    bool disjoint(const std::set<T>& s) const {
        for (auto iter = s.begin(); iter != s.end(); ++iter) {
            if (this->contains(*iter)) {
                return false;
            }
        }
        return true;
    }
    /** @brief Test whether the sets have no common elements. */
    bool disjoint(const std::vector<T>& lst) const {
        for (auto iter = lst.begin(); iter != lst.end(); ++iter) {
            if (this->contains(*iter)) {
                return false;
            }
        }
        return true;
    }
    /** @brief Add the supplied value or component. */
    Set<T>& adjoin(const std::set<T>& s) {
        for (auto iter = s.begin(); iter != s.end(); ++iter) {
            this->insert(*iter);
        }
        return *this;
    }
    /** @brief Remove the supplied entry. */
    Set<T>& remove(const T& t) {
        auto iter = this->find(t);
        if (iter != this->end()) {
            this->erase(iter);
        }
        return *this;
    }
    /** @brief Remove the supplied entry. */
    Set<T>& remove(const std::set<T>& s) {
        auto iter = this->begin();
        while (iter != this->end()) {
            if (s.find(*iter) != s.end()) {
                this->erase(iter++);
            } else {
                ++iter;
            }
        }
        return *this;
    }
    /** @brief Restrict to the supplied subset or domain. */
    Set<T>& restrict(const std::set<T>& s) {
        auto iter = this->begin();
        while (iter != this->end()) {
            if (s.find(*iter) != s.end()) {
                this->erase(iter++);
            } else {
                ++iter;
            }
        }
        return *this;
    }
    /** @brief Remove the supplied entry. */
    template<class TT>
    Set<T>& remove(const std::vector<TT>& l) {
        for (auto iter = l.begin(); iter != l.end(); ++iter) {
            this->std::set<T>::erase(static_cast<const T&>(*iter));
        }
        return *this;
    }
};
/** @brief Apply the supplied operation to the operands. */
template<class T, Invocable<T> F>
inline Set<InvokeResult<F, T>> apply(F&& f, const Set<T>& set) {
    typedef InvokeResult<F, T> R;
    Set<R> res;
    for (auto itm: set) {
        res.adjoin(f(itm));
    }
    return res;
}
/** @brief Join the operands in their natural order. */
template<class T>
inline Set<T> join(Set<T> s1, const Set<T>& s2) {
    s1.adjoin(s2);
    return s1;
}
/** @brief Test containment of the supplied value. */
template<class T>
inline bool contains(const std::set<T>& s, const T& t) {
    return s.find(t) != s.end();
}
/** @brief Test whether the first set is contained in the second. */
template<class T>
inline bool subset(const std::set<T>& s1, const std::set<T>& s2) {
    for (auto iter = s1.begin(); iter != s1.end(); ++iter) {
        if (!contains(s2, *iter)) {
            return false;
        }
    }
    return true;
}
/** @brief Test whether the sets have no common elements. */
template<class T>
inline bool disjoint(const std::set<T>& s1, const std::set<T>& s2) {
    for (auto iter = s1.begin(); iter != s1.end(); ++iter) {
        if (contains(s2, *iter)) {
            return false;
        }
    }
    return true;
}
/** @brief Test whether the sets have no common elements. */
template<class T>
inline bool disjoint(const std::set<T>& s1, const std::vector<T>& l2) {
    for (auto iter = l2.begin(); iter != l2.end(); ++iter) {
        if (contains(s1, *iter)) {
            return false;
        }
    }
    return true;
}
/** @brief Return the intersection of the operands. */
template<class T>
inline Set<T> intersection(const std::set<T>& s1, const std::set<T>& s2) {
    Set<T> r(s1);
    restrict(r, s2);
    return r;
}
/** @brief Return the intersection of the operands. */
template<class T>
inline Set<T> intersection(const std::set<T>& s1, const std::vector<T>& l2) {
    Set<T> r;
    for (auto iter = l2.begin(); iter != l2.end(); ++iter) {
        if (contains(s1, *iter)) {
            r.insert(*iter);
        }
    }
    return r;
}
/** @brief Return elements belonging to the first set only. */
template<class T>
inline Set<T> difference(const std::set<T>& s1, const std::set<T>& s2) {
    Set<T> r(s1);
    remove(r, s2);
    return r;
}
/** @brief Remove the supplied entry. */
template<class T>
inline std::set<T>& remove(std::set<T>& r, const std::set<T>& s) {
    auto iter = r.begin();
    while (iter != r.end()) {
        if (contains(s, *iter)) {
            r.erase(iter++);
        } else {
            ++iter;
        }
    }
    return r;
}
/** @brief Restrict to the supplied subset or domain. */
template<class T>
inline std::set<T>& restrict(std::set<T>& r, const std::set<T>& s) {
    auto iter = r.begin();
    while (iter != r.end()) {
        if (!contains(s, *iter)) {
            r.erase(iter++);
        } else {
            ++iter;
        }
    }
    return r;
}

/** @brief Write the value to the output stream. */
template<class T>
ostream& operator<<(ostream& os, const std::set<T>& v) {
    bool first = true;
    for (auto x: v) {
        os << (first ? "{" : ",") << x;
        first = false;
    }
    if (first) {
        os << "{";
    }
    return os << "}";
}

/** @brief Ordered key-value container with lookup and key-set operations. */
template<class K, class T>
class Map : public std::map<K, T> {
  public:
    typedef typename std::map<K, T>::iterator Iterator;
    typedef typename std::map<K, T>::const_iterator ConstIterator;
    /** @brief Construct a \c Map from m. */
    template<ConvertibleTo<T> TT>
    Map(const std::map<K, TT>& m) : std::map<K, T>(m.begin(), m.end()) {}
    using std::map<K, T>::map;
    using std::map<K, T>::insert;
    /** @brief Return the selected element or component. */
    T& operator[](K k) { return this->std::map<K, T>::operator[](k); }
    /** @brief Return the selected element or component. */
    const T& operator[](K k) const {
        auto iter = this->find(k);
        ARIADNE_ASSERT(iter != this->end());
        return iter->second;
    }
    /** @brief Return the requested element or stored value. */
    const T& get(const K& k) const {
        auto i = this->find(k);
        ARIADNE_ASSERT(i != this->end());
        return i->second;
    }
    /** @brief Test whether the map contains the supplied key. */
    bool has_key(const K& k) const {
        return this->find(k) != this->end();
    }
    /** @brief Return the stored value. */
    T& value(const K& k) {
        auto iter = this->find(k);
        ARIADNE_ASSERT(iter != this->end());
        return iter->second;
    }
    /** @brief Return the stored value. */
    const T& value(const K& k) const {
        auto iter = this->find(k);
        ARIADNE_ASSERT(iter != this->end());
        return iter->second;
    }
    /** @brief Insert the supplied entry. */
    void insert(const std::pair<K, T>& kv) {
        this->std::map<K, T>::insert(kv);
    }
    /** @brief Insert the supplied entry. */
    void insert(const K& k, const T& v) {
        this->std::map<K, T>::insert(std::make_pair(k, v));
    }
    /** @brief Add the supplied value or component. */
    void adjoin(const std::map<K, T>& m) {
        for (auto i = m.begin(); i != m.end(); ++i) {
            this->insert(*i);
        }
    }
    /** @brief Remove the supplied keys. */
    void remove_keys(const Set<K>& s) {
        for (auto iter = s.begin(); iter != s.end(); ++iter) {
            this->erase(*iter);
        }
    }
    /** @brief Return the keys. */
    Set<K> keys() const {
        Set<K> res;
        for (auto iter = this->begin(); iter != this->end(); ++iter) {
            res.insert(iter->first);
        }
        return res;
    }
    /** @brief Return the stored values. */
    List<T> values() const {
        List<T> res;
        for (auto iter = this->begin(); iter != this->end(); ++iter) {
            res.append(iter->second);
        }
        return res;
    }
};
/** @brief Join the operands in their natural order. */
template<class K, class T>
inline Map<K, T> join(Map<K, T> m1, const Map<K, T>& m2) {
    m1.adjoin(m2);
    return m1;
}
/** @brief Insert the supplied entry. */
template<class I, class X, class J>
inline X& insert(Map<I, X>& m, const J& k, const X& v) {
    return m.std::template map<I, X>::insert(std::make_pair(k, v)).first->second;
}
/** @brief Retain only the supplied keys. */
template<class K, class T>
Map<K, T> restrict_keys(const std::map<K, T>& m, const std::set<K>& k) {
    Map<K, T> result;
    const Set<K>& keys = static_cast<const Set<K>&>(k);
    for (auto item_iter = m.begin(); item_iter != m.end(); ++item_iter) {
        if (keys.contains(item_iter->first)) {
            result.insert(*item_iter);
        }
    }
    return result;
}
/** @brief Write the value to the output stream. */
template<class K, class T>
ostream& operator<<(ostream& os, const std::map<K, T>& m) {
    bool first = true;
    for (auto x: m) {
        os << (first ? "{ " : ", ") << x.first << ":" << x.second;
        first = false;
    }
    if (first) {
        os << "{";
    }
    return os << " }";
}

/** @brief Return the elements occurring once. */
template<class T>
bool unique_elements(const std::vector<T>& lst) {
    Set<T> found;
    for (auto iter = lst.begin(); iter != lst.end(); ++iter) {
        if (found.contains(*iter)) {
            return false;
        } else {
            found.insert(*iter);
        }
    }
    return true;
}

/** @brief Return the elements occurring more than once. */
template<class T>
Set<T> duplicate_elements(const std::vector<T>& lst) {
    Set<T> result;
    Set<T> found;
    for (auto iter = lst.begin(); iter != lst.end(); ++iter) {
        if (found.contains(*iter)) {
            result.insert(*iter);
        } else {
            found.insert(*iter);
        }
    }
    return result;
}

/** @brief Construct an array from the supplied elements. */
template<class T>
inline Array<T> make_array(const T& t) { return Array<T>(1u, t); }
/** @brief Construct an array from the supplied elements. */
template<class T>
inline Array<T> make_array(const std::vector<T>& vec) { return Array<T>(vec.begin(), vec.end()); }
/** @brief Construct an array from the supplied elements. */
template<class T>
inline Array<T> make_array(const Array<T>& ary) { return Array<T>(ary); }

/** @brief Construct a list from the supplied elements. */
template<class T>
inline List<T> make_list(const T& t) { return List<T>(1u, t); }
/** @brief Construct a list from the supplied elements. */
template<class T>
inline List<T> make_list(const std::vector<T>& vec) { return List<T>(vec); }
/** @brief Construct a list from the supplied elements. */
template<class T>
inline List<T> make_list(const Array<T>& ary) { return List<T>(ary); }
/** @brief Construct a list from the supplied elements. */
template<class T>
inline List<T> make_list(const Set<T>& set) { return List<T>(set.begin(), set.end()); }

/** @brief Construct a set from the supplied elements. */
template<class T>
inline Set<T> make_set(const std::vector<T>& lst) { return Set<T>(lst); }

} // namespace Ariadne

#endif
