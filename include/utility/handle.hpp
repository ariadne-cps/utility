/***************************************************************************
 *            utility/handle.hpp
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

/** @file utility/handle.hpp
 *  @brief Reference-counted handles and wrappers for polymorphic objects.
 */

#ifndef ARIADNE_UTILITY_HANDLE_HPP
#define ARIADNE_UTILITY_HANDLE_HPP

#include <memory>

#include "utility/metaprogramming.hpp"
#include "utility/stdlib.hpp"
#include "utility/typedefs.hpp"
#include "utility/writable.hpp"

namespace Ariadne {

using std::shared_ptr;

/************ Handle *********************************************************/

/** @brief Base object stored behind a polymorphic handle. */
template<class T>
struct HandleObject {};

template<class I>
class Handle;
/** @brief Dynamically cast the object held by the handle. */
template<class T, class I>
const T& dynamic_handle_cast(const Handle<I>& h);
/** @brief Dynamically cast the object held by the handle. */
template<class T, class I>
T& dynamic_handle_cast(Handle<I>& h);

/** @brief Adapter exposing a concrete object through an interface. */
template<class T, class I>
class Wrapper
    : public virtual I,
      public T {
  protected:
    /** @brief Construct a \c Wrapper from t. */
    Wrapper(const T& t) : T(t) {}
    /** @brief Move-construct the object from the supplied instance. */
    Wrapper(T&& t) : T(std::move(t)) {}
};

/** @brief Shared owning handle to a polymorphic interface. */
template<class I>
class Handle {
  public:
    typedef I Interface;

  protected:
    mutable shared_ptr<I> _ptr;

  public:
    /** @brief Destroy the object and release its owned resources. */
    ~Handle() {}
    /** @brief Construct a \c Handle from p. */
    explicit Handle(I* p) : _ptr(p) {}
    /** @brief Construct a \c Handle from p. */
    Handle(shared_ptr<I> p) : _ptr(p) {}
    /** @brief Move-construct the object from the supplied instance. */
    Handle(I&& r) : _ptr(r._move()) {}
    /** @brief Construct a \c Handle from r. */
    Handle(const I& r) : _ptr(r._copy()) {}
    /** @brief Move-construct the object from the supplied instance. */
    template<DerivedFrom<I> T>
    Handle(T&& t) : _ptr(new T(std::move(t))) {}
    /** @brief Construct a \c Handle from t. */
    template<DerivedFrom<I> T>
        requires CopyConstructible<T>
    Handle(const T& t)
        : _ptr(new T(t)) {}
    // Handle(const Handle<I>& h) : _ptr(h._ptr) { }
    // Handle(Handle<I>&& h) : _ptr(h._ptr) { }
    /** @brief Copy-construct the object from the supplied instance. */
    Handle(const Handle<I>& h) = default;
    /** @brief Move-construct the object from the supplied instance. */
    Handle(Handle<I>&& h) = default;
    /** @brief Assign the supplied value and return this object. */
    Handle<I>& operator=(const Handle<I>& h) = default;
    /** @brief Assign the supplied value and return this object. */
    Handle<I>& operator=(Handle<I>&& h) = default;
    /** @brief Copy-construct the object from the supplied instance. */
    template<class II>
    explicit Handle(const Handle<II>& h) : _ptr(h.managed_pointer()) {}
    // explicit operator I const& () const { return *_ptr; }
    /** @brief Convert to const i &. */
    operator const I&() const { return *_ptr; }

  public:
    /** @brief Return a constant reference to the represented object. */
    const I& const_reference() const { return _ptr.operator*(); }
    /** @brief Return a reference to the represented object. */
    const I& reference() const { return _ptr.operator*(); }
    /** @brief Return a reference to the represented object. */
    I& reference() {
        make_unique();
        return _ptr.operator*();
    }

  public:
    /** @brief Return a constant pointer to the represented object. */
    const I* const_pointer() const { return _ptr.operator->(); }
    /** @brief Return a pointer to the represented object. */
    const I* pointer() const { return _ptr.operator->(); }
    /** @brief Return a pointer to the represented object. */
    I* pointer() {
        make_unique();
        return _ptr.operator->();
    }

  public:
    /** @brief Return a non-owning constant pointer to the represented object. */
    const I* raw_const_pointer() const { return _ptr.operator->(); }
    /** @brief Return a non-owning pointer to the represented object. */
    const I* raw_pointer() const { return _ptr.operator->(); }
    /** @brief Return a non-owning pointer to the represented object. */
    I* raw_pointer() {
        make_unique();
        return _ptr.operator->();
    }

  public:
    /** @brief Return a reference to the stored object. */
    const I& ref() const { return _ptr.operator*(); }
    /** @brief Return a reference to the stored object. */
    I& ref() {
        make_unique();
        return _ptr.operator*();
    }

  public:
    /** @brief Return a pointer to the stored object. */
    const I* ptr() const { return _ptr.operator->(); }
    /** @brief Return a pointer to the stored object. */
    I* ptr() {
        make_unique();
        return _ptr.operator->();
    }

  public:
    /** @brief Return shared ownership of the constant represented object. */
    const shared_ptr<I> managed_const_pointer() const { return _ptr; }
    /** @brief Return shared ownership of the represented object. */
    const shared_ptr<I> managed_pointer() const { return _ptr; }
    /** @brief Return shared ownership of the represented object. */
    shared_ptr<I> managed_pointer() {
        make_unique();
        return _ptr;
    }

  protected:
    /** @brief Ensure exclusive ownership before modifying the object. */
    template<class II>
    static void _make_unique(shared_ptr<II>& ptr) {
        ptr = shared_ptr<I>(ptr->clone());
    }
    /** @brief Ensure exclusive ownership before modifying the object. */
    template<class II>
    static void _make_unique(shared_ptr<const II>&) {}
    /** @brief Ensure exclusive ownership before modifying the object. */
    void make_unique() { _make_unique(_ptr); }

  private:
    /** @brief Extract the concrete object from the handle using a checked dynamic cast. */
    template<class T, class II>
    friend T& dynamic_handle_extract(Handle<II>& h);
    /** @brief Extract the concrete object from the handle using a checked dynamic cast. */
    template<class T, class II>
    friend const T& dynamic_handle_extract(const Handle<II>& h);
    /** @brief Dynamically cast the object held by the handle. */
    template<class D, class B>
    friend Handle<D> dynamic_handle_cast(Handle<B>& h);
    /** @brief Dynamically cast the object held by the handle. */
    template<class D, class B>
    friend const Handle<D> dynamic_handle_cast(const Handle<B>& h);
};

/** @brief Construct a managed handle for the supplied concrete object. */
template<class T, class... AS>
Handle<T> make_handle(AS&&... as) {
    return Handle<T>(std::make_shared<T>(as...));
}

/** @brief Write a diagnostic describing the failed handle conversion. */
inline void write_error(ostream& os, const char* f, const WritableInterface* w, const char* i, const char* c, const char* t) {
    os << "Error in " << f << ":" << std::flush;
    os << " cannot convert ";
    if (w) {
        w->_write(os);
    } else {
        os << "object";
    }
    os << std::flush;
    os << " of static type " << c << " and dynamic type " << i << " to type " << t << std::endl;
}

/** @brief Dynamically cast the object held by the handle. */
template<class D, class B>
D dynamic_handle_cast(const B& h) {
    typedef typename B::Interface BI;
    typedef typename D::Interface DI;
    if constexpr (Same<decltype(h.managed_pointer()), shared_ptr<BI>>) {
        shared_ptr<DI> p = std::dynamic_pointer_cast<DI>(h.managed_pointer());
        if (p) {
            return D(Handle<DI>(p));
        }
    } else {
        shared_ptr<const DI> p = std::dynamic_pointer_cast<const DI>(h.managed_pointer());
        if (p) {
            return D(Handle<const DI>(p));
        }
    }
    const BI* i = h.raw_pointer();
    const WritableInterface* w = dynamic_cast<const WritableInterface*>(i);
    write_error(std::cerr, "dynamic_handle_cast", w, typeid(*i).name(), typeid(i).name(), typeid(D).name());
    throw std::bad_cast();
}

/** @brief Extract the concrete object from the handle using a checked dynamic cast. */
template<class T, class I>
const T& dynamic_handle_extract(const Handle<const I>& h) {
    const I* i = h.raw_const_pointer();
    const T* p = dynamic_cast<const T*>(i);
    if (p) {
        return const_cast<T&>(*p);
    }
    p = dynamic_cast<const Wrapper<T, I>*>(i);
    if (p) {
        return *p;
    }
    const WritableInterface* w = dynamic_cast<const WritableInterface*>(i);
    write_error(std::cerr, "dynamic_handle_extract", w, typeid(*i).name(), typeid(i).name(), typeid(T).name());
    throw std::bad_cast();
}

/** @brief Extract the concrete object from the handle using a checked dynamic cast. */
template<class T, class I>
const T& dynamic_handle_extract(const Handle<I>& h) {
    return dynamic_handle_extract<T>(Handle<const I>(h));
}

/** @brief Extract the concrete object from the handle using a checked dynamic cast. */
template<class T, class I>
T& dynamic_handle_extract(Handle<I>& h) {
    const I* i = h.raw_const_pointer();
    const T* p = dynamic_cast<const T*>(i);
    if (p) {
        return const_cast<T&>(*p);
    }
    p = dynamic_cast<const Wrapper<T, I>*>(i);
    if (p) {
        return const_cast<T&>(*p);
    }
    const WritableInterface* w = dynamic_cast<const WritableInterface*>(i);
    write_error(std::cerr, "dynamic_handle_extract", w, typeid(*i).name(), typeid(i).name(), typeid(T).name());
    throw std::bad_cast();
}

/** @brief Extract the requested concrete object from the pointer. */
template<class T, class I>
shared_ptr<const T> dynamic_pointer_extract(const shared_ptr<const I>& ip) {
    shared_ptr<const T> tp = dynamic_pointer_cast<const T>(ip);
    if (tp) {
        return tp;
    }
    tp = static_pointer_cast<const T>(dynamic_pointer_cast<const Wrapper<T, I>>(ip));
    if (tp) {
        return tp;
    }
    shared_ptr<const WritableInterface> wp = dynamic_pointer_cast<const WritableInterface>(ip);
    return tp;
}

} // namespace Ariadne

#endif
