/***************************************************************************
 *            utility/dispatching.hpp
 *
 *  Copyright  2013-22  Pieter Collins
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

/** @file utility/dispatching.hpp
 *  @brief Utility classes supporting double dispatching
 */

#ifndef ARIADNE_UTILITY_DISPATCHING_HPP
#define ARIADNE_UTILITY_DISPATCHING_HPP

namespace Ariadne {

/** @brief Type list identifying concrete types available for runtime dispatch. */
template<class... YS>
struct Aware;

/** @brief Tag indicating managed ownership in dispatch conversions. */
template<class DI>
struct Managed;
template<class DI>
using ManagedTypes = typename Managed<DI>::Types;

/** @brief Dispatch er traits for the supported operand types. */
template<class T>
struct DispatcherTraits;
template<class T>
using DispatcherInterface = typename DispatcherTraits<T>::Interface;
template<class T>
using Dispatcher = typename DispatcherTraits<T>::Interface;

/** @brief Compile-time traits for interface. */
template<class I>
struct InterfaceTraits;
template<class X, class I>
using MixinType = typename InterfaceTraits<I>::template MixinType<X>;
template<class X, class I>
using WrapperType = typename InterfaceTraits<I>::template WrapperType<X>;

/** @brief Polymorphic interface for right operable. */
template<class R, class OP, class YS, class BYS = YS>
struct RightOperableInterface;
/** @brief Polymorphic interface for right operable. */
template<class R, class OP, class BYS, class Y>
struct RightOperableInterface<R, OP, Aware<Y>, BYS> {
    /** @brief Dispatch the binary operation with the concrete right operand. */
    virtual R _concrete_apply_right(OP op, const Y& y) const = 0;
};
/** @brief Polymorphic interface for right operable. */
template<class R, class OP, class BYS, class Y, class... YS>
struct RightOperableInterface<R, OP, Aware<Y, YS...>, BYS>
    : public virtual RightOperableInterface<R, OP, Aware<YS...>, BYS> {
    using RightOperableInterface<R, OP, Aware<YS...>, BYS>::_concrete_apply_right;
    /** @brief Dispatch the binary operation with the concrete right operand. */
    virtual R _concrete_apply_right(OP op, const Y& y) const = 0;
};

/** @brief Polymorphic interface for left operable. */
template<class R, class OP, class YS, class RYS = YS>
struct LeftOperableInterface;
/** @brief Polymorphic interface for left operable. */
template<class R, class OP, class RYS, class Y>
struct LeftOperableInterface<R, OP, Aware<Y>, RYS>
    : public virtual RightOperableInterface<R, OP, RYS> {
    /** @brief Dispatch the binary operation with the concrete left operand. */
    virtual R _concrete_apply_left(OP op, const Y& y) const = 0;
};
/** @brief Polymorphic interface for left operable. */
template<class R, class OP, class RYS, class Y, class... YS>
struct LeftOperableInterface<R, OP, Aware<Y, YS...>, RYS>
    : public virtual LeftOperableInterface<R, OP, Aware<YS...>, RYS> {
    using LeftOperableInterface<R, OP, Aware<YS...>, RYS>::_concrete_apply_left;
    /** @brief Dispatch the binary operation with the concrete left operand. */
    virtual R _concrete_apply_left(OP op, const Y& y) const = 0;
};

/** @brief Polymorphic interface for self operable. */
template<class R, class OP, class YS>
struct SelfOperableInterface
    : public RightOperableInterface<R, OP, YS, YS> {};

/** @brief Polymorphic interface for operable. */
template<class R, class OP, class YS>
struct OperableInterface
    : public LeftOperableInterface<R, OP, YS, YS> {};

/*
template<class R, class OP, class X> inline R _concrete_apply(OP op, X const& x);
template<class R, class OP, class X1, class X2> inline R _concrete_apply(OP op, X1 const& x1, X2 const& x2);
*/

/** @brief Reusable implementation of right operable. */
template<class X, class I, class R, class OP, class YS, class BYS = YS>
struct RightOperableMixin;
/** @brief Reusable implementation of right operable. */
template<class X, class I, class R, class OP, class BYS>
struct RightOperableMixin<X, I, R, OP, Aware<>, BYS>
    : virtual RightOperableInterface<R, OP, BYS> {
    /** @brief Return the concrete object used for dispatch. */
    const X& self() const { return static_cast<const WrapperType<X, I>&>(*this); }
};
/** @brief Reusable implementation of right operable. */
template<class X, class I, class R, class OP, class BYS, class Y, class... YS>
struct RightOperableMixin<X, I, R, OP, Aware<Y, YS...>, BYS>
    : RightOperableMixin<X, I, R, OP, Aware<YS...>, BYS> {
    using RightOperableMixin<X, I, R, OP, Aware<YS...>, BYS>::_concrete_apply_right;
    /** @brief Dispatch the binary operation with the concrete right operand. */
    virtual R _concrete_apply_right(OP op, const Y& y) const override { return _concrete_apply<R>(op, y, this->self()); }
};

/** @brief Reusable implementation of left operable. */
template<class X, class I, class R, class OP, class YS, class BYS = YS>
struct LeftOperableMixin;
/** @brief Reusable implementation of left operable. */
template<class X, class I, class R, class OP, class BYS>
struct LeftOperableMixin<X, I, R, OP, Aware<>, BYS>
    : virtual LeftOperableInterface<R, OP, BYS> {
    /** @brief Return the concrete object used for dispatch. */
    const X& self() const { return static_cast<const WrapperType<X, I>&>(*this); }
};
/** @brief Reusable implementation of left operable. */
template<class X, class I, class R, class OP, class BYS, class Y, class... YS>
struct LeftOperableMixin<X, I, R, OP, Aware<Y, YS...>, BYS>
    : LeftOperableMixin<X, I, R, OP, Aware<YS...>, BYS> {
    using LeftOperableMixin<X, I, R, OP, Aware<YS...>, BYS>::_concrete_apply_left;
    using LeftOperableMixin<X, I, R, OP, Aware<YS...>, BYS>::_concrete_apply_right;
    /** @brief Dispatch the binary operation with the concrete left operand. */
    virtual R _concrete_apply_left(OP op, const Y& y) const override { return _concrete_apply<R>(op, this->self(), y); }
    /** @brief Dispatch the binary operation with the concrete right operand. */
    virtual R _concrete_apply_right(OP op, const Y& y) const override { return _concrete_apply<R>(op, y, this->self()); }
};

/** @brief Reusable implementation of self operable. */
template<class X, class I, class R, class OP, class YS>
struct SelfOperableMixin
    : public RightOperableMixin<X, I, R, OP, YS, YS> {};
/** @brief Reusable implementation of operable. */
template<class X, class I, class R, class OP, class YS>
struct OperableMixin
    : public LeftOperableMixin<X, I, R, OP, YS, YS> {};

/** @brief Apply the supplied operation to the operands. */
template<class R, class X, class OP, class I>
inline R _apply(const X& self, OP op, const I* self_ptr, const I* other_ptr) {
    using AW = ManagedTypes<Dispatcher<X>>;
    auto aware_other_ptr = dynamic_cast<const RightOperableInterface<R, OP, AW>*>(other_ptr);
    if (aware_other_ptr) {
        return aware_other_ptr->_concrete_apply_right(op, self);
    } else {
        return other_ptr->_rapply(op, self_ptr);
    }
}
/** @brief Apply the operation with reversed operand order. */
template<class R, class X, class OP, class I>
inline R _rapply(const X& self, OP op, const I* self_ptr, const I* other_ptr) {
    using AW = ManagedTypes<Dispatcher<X>>;
    auto aware_other_ptr = dynamic_cast<const LeftOperableInterface<R, OP, AW>*>(other_ptr);
    if (aware_other_ptr) {
        return aware_other_ptr->_concrete_apply_left(op, self);
    } else {
        return make_symbolic(op, other_ptr, self_ptr);
    }
}

/** @brief Reusable implementation of unary operation. */
template<class X, class RI, class OP, class I>
struct UnaryOperationMixin : public virtual I {
    /** @brief Convert to the concrete representation required by dispatch. */
    static const X& _cast(const UnaryOperationMixin<X, RI, OP, I>& self) { return static_cast<const MixinType<X, I>&>(self); }
    /** @brief Wrap the concrete object in the requested interface. */
    template<class R>
    static RI* _make_wrapper(R&& r) { return new WrapperType<R, I>(r); }
    /** @brief Apply the supplied operation to the operands. */
    virtual RI* _apply(OP op) const final { return _concrete_apply<RI*>(op, _cast(*this)); }
};

/*
template<class X, class RI, class OP, class I> struct UnaryOperationMixin : public virtual I {
    static X const& _cast(UnaryOperationMixin<X,RI,OP,I> const& self) { return static_cast<MixinType<X,I> const&>(self); }
    template<class R> static RI* _make_wrapper(R&& r) { return new WrapperType<R,I>(r); }
    virtual RI* _apply(OP op) const final { return _make_wrapper(op(_cast(*this))); }
    //virtual RI* _apply(OP op) const final { return _concrete_unary_apply<RI*,OP,X>(op,_cast(*this)); }
    //virtual RI* _apply(OP op) const final { X const& x=_cast(*this); return op.accept([&x](auto _op){return _make_wrapper(_op(x));}); }
    //virtual RI* _apply(OP op) const final { X const& x=_cast(*this); return op.accept([&x](auto _op){return _make_wrapper(_op(x));}); }
};
*/

/** @brief Reusable implementation of binary operation. */
template<class X, class RI, class OP, class I>
struct BinaryOperationMixin : public virtual I {
    /** @brief Convert to the concrete representation required by dispatch. */
    static const X& _cast(const BinaryOperationMixin<X, RI, OP, I>& self) { return static_cast<const MixinType<X, I>&>(self); }
    /** @brief Apply the supplied operation to the operands. */
    virtual RI* _apply(OP op, const I* other) const final { return Ariadne::_apply<RI*, X>(_cast(*this), op, static_cast<const I*>(this), other); }
    /** @brief Apply the operation with reversed operand order. */
    virtual RI* _rapply(OP op, const I* other) const final { return Ariadne::_rapply<RI*, X>(_cast(*this), op, static_cast<const I*>(this), other); }
};

/** @brief Reusable implementation of graded operation. */
template<class X, class RI, class OP, class I, class N>
struct GradedOperationMixin : public virtual I {
    /** @brief Convert to the concrete representation required by dispatch. */
    static const X& _cast(const GradedOperationMixin<X, RI, OP, I, N>& self) { return static_cast<const MixinType<X, I>&>(self); }
    /** @brief Wrap the concrete object in the requested interface. */
    template<class R>
    static RI* _make_wrapper(R&& r) { return new WrapperType<R, I>(r); }
    /** @brief Apply the supplied operation to the operands. */
    virtual RI* _apply(OP op, N n) const final { return _make_wrapper(op(_cast(*this), n)); }
};

} // namespace Ariadne

#endif /* ARIADNE_DISPATCHING_HPP */
