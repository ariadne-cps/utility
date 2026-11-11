/***************************************************************************
 *            utility/pointer.hpp
 *
 *  Copyright  2008-20  Pieter Collins
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

/** @file utility/pointer.hpp
 *  @brief Smart pointers in Euclidean space.
 */

#ifndef ARIADNE_UTILITY_POINTER_HPP
#define ARIADNE_UTILITY_POINTER_HPP

#include <cassert>
#include <iostream>
#include <memory>

namespace Ariadne {

template<class T>
using SharedPointer = std::shared_ptr<T>;
using std::ostream;

// A tag used as a function argument to denote that the value is a pointer, and not possibly the integer 0
/** @brief Tag selecting pointer. */
struct PointerTag {};

/** @brief Shared pointer that copies its value before mutable access. */
template<class T>
class copy_on_write_ptr {
  private:
    mutable int* _ref_count;
    mutable T* _ptr;

  public:
    /** @brief Destroy the object and release its owned resources. */
    ~copy_on_write_ptr() { this->_deallocate(); }
    /** @brief Construct a \c copy_on_write_ptr from pointer. */
    template<class S>
    copy_on_write_ptr(S* pointer)
        : _ref_count(new int(1)), _ptr(pointer) {}
    /** @brief Copy-construct the object from the supplied instance. */
    copy_on_write_ptr(const copy_on_write_ptr<T>& other)
        : _ref_count(other._ref_count), _ptr(other._ptr) { ++(*_ref_count); }
    /** @brief Assign the supplied value and return this object. */
    template<class S>
    copy_on_write_ptr<T>& operator=(S* pointer) {
        _deallocate();
        _allocate(pointer);
        return *this;
    }
    /** @brief Assign the supplied value and return this object. */
    copy_on_write_ptr<T>& operator=(const copy_on_write_ptr<T>& other) {
        if (_ptr != other._ptr) {
            _deallocate();
            _ptr = other._ptr;
            _ref_count = other._ref_count;
            ++(*_ref_count);
        }
        return *this;
    }
    /** @brief Return the value referenced by the iterator or pointer. */
    const T& operator*() const { return *_ptr; }
    /** @brief Return a pointer to the referenced object. */
    const T* operator->() const { return _ptr; }
    /** @brief Return the value referenced by the iterator or pointer. */
    T& operator*() {
        if (*_ref_count > 1) {
            _make_copy();
        }
        return *_ptr;
    }
    /** @brief Return a pointer to the referenced object. */
    T* operator->() {
        if (*_ref_count > 1) {
            _make_copy();
        }
        return _ptr;
    }

  public:
    /** @brief Return the number of shared owners. */
    int count() const { return *_ref_count; }
    /** @brief Return the address of the stored object. */
    const void* address() const { return _ptr; }
    /** @brief Return the stored value. */
    const T& value() const { return *_ptr; }

  private:
    /** @brief Copy the shared object before mutation. */
    void _make_copy() {
        --(*_ref_count);
        _ref_count = new int(1);
        _ptr = new T(*_ptr);
    }
    /** @brief Release the allocated storage. */
    void _deallocate() {
        if (*_ref_count == 1) {
            delete _ptr;
            _ptr = 0;
            delete _ref_count;
        } else {
            --(*_ref_count);
        }
    }
    /** @brief Allocate storage for the requested elements. */
    template<class S>
    void _allocate(S* pointer) {
        _ref_count = new int(1);
        _ptr = pointer;
    }
};

/** @brief Write the value to the output stream. */
template<class T>
std::ostream&
    operator<<(std::ostream& os, const copy_on_write_ptr<T>& ptr) {
    return os << "[" << ptr.count() << "]<" << ptr.address() << ">(" << ptr.value() << ")";
}

/** @brief Shared pointer that clones its polymorphic value before mutable access. */
template<class T>
class clone_on_write_ptr {
    template<class S>
    friend class clone_on_write_ptr;

  private:
    mutable int* _ref_count;
    mutable T* _ptr;

  public:
    /** @brief Destroy the object and release its owned resources. */
    ~clone_on_write_ptr() { this->_deallocate(); }
    /** @brief Construct a \c clone_on_write_ptr from pointer. */
    template<class S>
    clone_on_write_ptr(S* pointer)
        : _ref_count(new int(1)), _ptr(pointer) {}
    /** @brief Copy-construct the object from the supplied instance. */
    clone_on_write_ptr(const clone_on_write_ptr<T>& other)
        : _ref_count(other._ref_count), _ptr(other._ptr) { ++(*_ref_count); }
    /** @brief Copy-construct the object from the supplied instance. */
    template<class S>
    clone_on_write_ptr(const clone_on_write_ptr<S>& other)
        : _ref_count(other._ref_count), _ptr(other._ptr) { ++(*_ref_count); }
    /** @brief Assign the supplied value and return this object. */
    template<class S>
    clone_on_write_ptr<T>& operator=(S* pointer) {
        _deallocate();
        _allocate(pointer);
        return *this;
    }
    /** @brief Assign the supplied value and return this object. */
    clone_on_write_ptr<T>& operator=(const clone_on_write_ptr<T>& other) {
        if (_ptr != other._ptr) {
            _deallocate();
            _ptr = other._ptr;
            _ref_count = other._ref_count;
            ++(*_ref_count);
        }
        return *this;
    }
    /** @brief Return the value referenced by the iterator or pointer. */
    const T& operator*() const { return *_ptr; }
    /** @brief Return a pointer to the referenced object. */
    const T* operator->() const { return _ptr; }
    /** @brief Return the value referenced by the iterator or pointer. */
    T& operator*() {
        if (*_ref_count > 1) {
            _makeclone();
        }
        return *_ptr;
    }
    /** @brief Return a pointer to the referenced object. */
    T* operator->() {
        if (*_ref_count > 1) {
            _makeclone();
        }
        return _ptr;
    }

  public:
    /** @brief Return the number of shared owners. */
    int count() const { return *_ref_count; }
    /** @brief Return the address of the stored object. */
    const void* address() const { return _ptr; }
    /** @brief Return the stored value. */
    const T& value() const { return *_ptr; }

  private:
    /** @brief Clone the shared object before mutation. */
    void _makeclone() {
        --(*_ref_count);
        _ref_count = new int(1);
        _ptr = _ptr->clone();
    }
    /** @brief Release the allocated storage. */
    void _deallocate() {
        if (*_ref_count == 1) {
            delete _ptr;
            _ptr = 0;
            delete _ref_count;
        } else {
            --(*_ref_count);
        }
    }
    /** @brief Allocate storage for the requested elements. */
    template<class S>
    void _allocate(S* pointer) {
        _ref_count = new int(1);
        _ptr = pointer;
    }
};

/** @brief Write the value to the output stream. */
template<class T>
std::ostream&
    operator<<(std::ostream& os, const clone_on_write_ptr<T>& ptr) {
    return os << "[" << ptr.count() << "]<" << ptr.address() << ">(" << ptr.value() << ")";
}

/** @brief Pointer that copies the pointed-to value when copied. */
template<class T>
class copy_on_copy_ptr {
  private:
    mutable T* _ptr;

  public:
    /** @brief Destroy the object and release its owned resources. */
    ~copy_on_copy_ptr() {
        delete _ptr;
        _ptr = 0;
    }
    /** @brief Construct a \c copy_on_copy_ptr with the specified characteristics. */
    copy_on_copy_ptr()
        : _ptr(0u) {}
    /** @brief Construct a \c copy_on_copy_ptr from pointer. */
    template<class S>
    copy_on_copy_ptr(S* pointer)
        : _ptr(pointer) {}
    /** @brief Copy-construct the object from the supplied instance. */
    copy_on_copy_ptr(const copy_on_copy_ptr<T>& other)
        : _ptr(other._ptr ? other._ptr->clone() : other._ptr) {}
    /** @brief Assign the supplied value and return this object. */
    template<class S>
    copy_on_copy_ptr<T>& operator=(S* pointer) {
        delete _ptr;
        _ptr = pointer;
        return *this;
    }
    /** @brief Assign the supplied value and return this object. */
    copy_on_copy_ptr<T>& operator=(const copy_on_copy_ptr<T>& other) {
        if (_ptr != other._ptr) {
            delete _ptr;
            _ptr = other._ptr ? other._ptr->clone() : other._ptr;
        }
        return *this;
    }
    /** @brief Return the value referenced by the iterator or pointer. */
    const T& operator*() const { return *_ptr; }
    /** @brief Return a pointer to the referenced object. */
    const T* operator->() const { return _ptr; }
    /** @brief Return the value referenced by the iterator or pointer. */
    T& operator*() { return *_ptr; }
    /** @brief Return a pointer to the referenced object. */
    T* operator->() { return _ptr; }

  public:
    /** @brief Return the address of the stored object. */
    const void* address() const { return _ptr; }
    /** @brief Return the stored value. */
    const T& value() const { return *_ptr; }
};

/** @brief Write the value to the output stream. */
template<class T>
std::ostream&
    operator<<(std::ostream& os, const copy_on_copy_ptr<T>& ptr) {
    return os << "<" << ptr.address() << ">";
}

/** @brief Pointer that clones the pointed-to polymorphic value when copied. */
template<class T>
class clone_on_copy_ptr {
  private:
    mutable T* _ptr;

  public:
    /** @brief Destroy the object and release its owned resources. */
    ~clone_on_copy_ptr() {
        delete _ptr;
        _ptr = 0;
    }
    /** @brief Construct a \c clone_on_copy_ptr with the specified characteristics. */
    clone_on_copy_ptr() : _ptr(0) {}
    /** @brief Construct a \c clone_on_copy_ptr from pointer. */
    template<class S>
    clone_on_copy_ptr(S* pointer)
        : _ptr(pointer) {}
    /** @brief Copy-construct the object from the supplied instance. */
    clone_on_copy_ptr(const clone_on_copy_ptr<T>& other)
        : _ptr(other._ptr ? other._ptr->_clone() : other._ptr) {}
    /** @brief Assign the supplied value and return this object. */
    template<class S>
    clone_on_copy_ptr<T>& operator=(S* pointer) {
        delete _ptr;
        _ptr = pointer;
        return *this;
    }
    /** @brief Assign the supplied value and return this object. */
    clone_on_copy_ptr<T>& operator=(const clone_on_copy_ptr<T>& other) {
        if (_ptr != other._ptr) {
            delete _ptr;
            _ptr = other._ptr ? other._ptr->_clone() : other._ptr;
        }
        return *this;
    }
    /** @brief Return the value referenced by the iterator or pointer. */
    const T& operator*() const { return *_ptr; }
    /** @brief Return a pointer to the referenced object. */
    const T* operator->() const { return _ptr; }
    /** @brief Return the value referenced by the iterator or pointer. */
    T& operator*() { return *_ptr; }
    /** @brief Return a pointer to the referenced object. */
    T* operator->() { return _ptr; }

  public:
    /** @brief Return the address of the stored object. */
    const void* address() const { return _ptr; }
    /** @brief Return the stored value. */
    const T& value() const { return *_ptr; }
};

/** @brief Write the value to the output stream. */
template<class T>
std::ostream&
    operator<<(std::ostream& os, const clone_on_copy_ptr<T>& ptr) {
    return os << "<" << ptr.address() << ">";
}

/** @brief Reference-counted pointer managing the lifetime of its pointee. */
template<class T>
class counted_pointer;

/** @brief Reference-counted pointer managing the lifetime of its pointee. */
template<class T>
class counted_pointer<const T> {
    const T* _ptr;

  public:
    /** @brief Construct a \c counted_pointer from p. */
    counted_pointer(const T* p) : _ptr(p) {
        assert(p != 0);
        ++_ptr->count;
    }
    /** @brief Copy-construct the object from the supplied instance. */
    counted_pointer(const counted_pointer<const T>& p) : _ptr(p._ptr) { ++_ptr->count; }
    /** @brief Assign the supplied value and return this object. */
    counted_pointer<const T>& operator=(const counted_pointer<const T>& p) {
        if (_ptr != p._ptr) {
            --_ptr->count;
            if (_ptr->count == 0) {
                delete _ptr;
            }
            _ptr = p._ptr;
            ++_ptr->count;
        }
        return *this;
    }
    /** @brief Destroy the object and release its owned resources. */
    ~counted_pointer() {
        --_ptr->count;
        if (_ptr->count == 0) {
            delete _ptr;
        }
    }
    /** @brief Return a pointer to the referenced object. */
    const T* operator->() const { return _ptr; }
};

} // namespace Ariadne

#endif // ARIADNE_POINTER_HPP
