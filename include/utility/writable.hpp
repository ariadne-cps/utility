/***************************************************************************
 *            utility/writable.hpp
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

/** @file utility/writable.hpp
 *  @brief Interfaces and adapters for writing objects to output streams.
 */

#ifndef ARIADNE_UTILITY_WRITABLE_HPP
#define ARIADNE_UTILITY_WRITABLE_HPP

#include "utility/metaprogramming.hpp"
#include "utility/typedefs.hpp"

namespace Ariadne {

using std::ostream;

/************ WritableInterface **********************************************/

class WritableInterface {
  public:
    virtual ~WritableInterface() = default;
    friend ostream& operator<<(ostream& os, const WritableInterface& w);

  public:
    inline ostream& write(ostream& os) const { return this->_write(os); }

  protected:

  public:
    virtual ostream& _write(ostream&) const = 0;
};
inline ostream& operator<<(ostream& os, const WritableInterface& w) {
    w._write(os);
    return os;
}

template<class T, class = decltype(declval<T>()._write(declval<ostream&>()))>
True has_write(int);
template<class T>
False has_write(...);
template<class T, class = Fallback>
struct IsWritable : decltype(has_write<T>(1)) {};

template<class T>
    requires IsWritable<T>::value
ostream& operator<<(ostream& os, const T& t) {
    return t._write(os);
}

template<class T>
class Writer;
template<class T>
class WriterInterface;
template<class T, class W = WriterInterface<T>>
class WritableTemporary;

template<class T>
class WriterInterface {
  public:
    virtual ~WriterInterface() = default;
    inline WritableTemporary<T> operator()(const T& t) const;

  private:
    virtual ostream& _write(ostream& os, const T& t) const = 0;
    friend ostream& operator<<(ostream& os, const WritableTemporary<T>& t);
};

template<class T>
class Handle;

template<class W, class T>
concept AWriter = requires(const W& w, ostream& os, const T& t) {
    w._write(os, t);
};

template<class T, AWriter<T> W>
class WriterMixin : public WriterInterface<T> {
    W _w;

  public:
    WriterMixin(W w) : _w(w) {}
    virtual ostream& _write(ostream& os, const T& t) const override { return this->_w._write(os, t); }
};

template<class T>
class Writer : public Handle<WriterInterface<T>> {
  public:
    using Handle<WriterInterface<T>>::Handle;
    Writer(Handle<WriterInterface<T>> wh) : Handle<WriterInterface<T>>(wh) {}
    template<AWriter<T> W>
    Writer(W w) : Writer(new WriterMixin<T, W>(w)) {}
    inline WritableTemporary<T> operator()(const T& t) const;
};

template<class T, class W>
class WritableTemporary {
  private:
    const W& _w;
    const T& _t;
    WritableTemporary(const W& w, const T& t) : _w(w), _t(t) {}
    friend W;
    friend class Writer<T>;

  public:
    friend ostream& operator<<(ostream& os, const WritableTemporary<T, W>& wt) { return wt._w._write(os, wt._t); }
};

template<class T>
WritableTemporary<T> WriterInterface<T>::operator()(const T& t) const {
    return WritableTemporary<T>(*this, t);
}
template<class T>
WritableTemporary<T> Writer<T>::operator()(const T& t) const {
    return WritableTemporary<T>(*this->_ptr, t);
}

template<class T>
class RepresentationWriter;

} // namespace Ariadne

#endif
