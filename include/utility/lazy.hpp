/***************************************************************************
 *            utility/lazy.hpp
 *
 *  Copyright  2023  Luca Geretti
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

/** @file utility/lazy.hpp
 *  @brief Class for holding objects that are lazily retrieved.
 */

#ifndef ARIADNE_UTILITY_LAZY_HPP
#define ARIADNE_UTILITY_LAZY_HPP

#include <functional>
#include <memory>

namespace Ariadne {

using std::function;
using std::shared_ptr;

template<class O>
class Lazy {
  public:
    const O& operator()() const {
        if (_obj == nullptr)
            _obj.reset(_func());
        return *_obj;
    };
    Lazy(function<O*()> func) : _func(func) {}

  private:
    function<O*()> _func;
    mutable shared_ptr<O> _obj;
};

} // namespace Ariadne

#endif // ARIADNE_UTILITY_LAZY_HPP
