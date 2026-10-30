/***************************************************************************
 *            lru_cache.hpp
 *
 *  Copyright  2007-21  Luca Geretti
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

/*! \file lru_cache.hpp
 *  \brief A Least Recently Used cache for holding a limited number of commonly-accessed objects
 */

#ifndef ARIADNE_UTILITY_LRU_CACHE_HPP
#define ARIADNE_UTILITY_LRU_CACHE_HPP

#include <map>
#include "utility/macros.hpp"

namespace Ariadne {

using std::size_t;

//! \brief A Least Recently Used cache for holding a limited number of commonly-accessed objects of type \a V indexed by a label \a L
template<class L, class V> class LRUCache {
  private:
    struct AgeValuePair {
        AgeValuePair(size_t a, V v) : age(a), value(v) { }
        size_t age;
        V value;
    };
  public:
    //! \brief Construct with a given \a maximum_size
    LRUCache(size_t maximum_size) : _maximum_size(maximum_size) { ARIADNE_PRECONDITION(maximum_size>0); }

    //! \brief Check whether the label is present
    bool has_label(L const& label) { return (_elements.find(label) != _elements.end()); }

    //! \brief Get the element identified with \a label
    //! \details Updates age accordingly
    V const& get(L const& label) {
        auto e = _elements.find(label);
        ARIADNE_ASSERT_MSG(e != _elements.end(), "Cache has no element for label " << label);
        for (auto& other : _elements) {
            if (other.second.age < e->second.age)
                other.second.age++;
        }
        e->second.age = 0;
        return e->second.value;
    }

    //! \brief The age of a given \a label in the cache
    size_t age(L const& label) {
        auto e = _elements.find(label);
        ARIADNE_ASSERT_MSG(e != _elements.end(), "Cache has no element for label " << label);
        return e->second.age;
    }

    //! \brief Insert the element
    //! \details The element must not already exist; updates ages accordingly
    void put(L const& label, V const& val) {
        ARIADNE_PRECONDITION(not has_label(label));
        if (_elements.size() < _maximum_size) {
            for (auto& other : _elements) other.second.age++;
        } else {
            auto to_remove = _elements.end();
            for (auto it = _elements.begin(); it != _elements.end(); ++it) {
                if (it->second.age == _maximum_size-1) to_remove = it;
                it->second.age++;
            }
            _elements.erase(to_remove);
        }
        _elements.insert({label,AgeValuePair(0,val)});
    }

    //! \brief The current size due to putting elements
    size_t current_size() const {
        return _elements.size();
    }

    //! \brief The maximum size as initially supplied
    size_t maximum_size() const {
        return _maximum_size;
    }

  private:
    size_t _maximum_size;
    std::map<L,AgeValuePair> _elements;
};


} // namespace Ariadne

#endif // ARIADNE_UTILITY_LRU_CACHE_HPP
