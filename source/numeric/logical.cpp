/***************************************************************************
 *            logical.cpp
 *
 *  Copyright 2013--17  Pieter Collins
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*! \file logical.cpp
 *  \brief
 */

#include "utility/stdlib.hpp"
#include "utility/string.hpp"
#include "expression/templates.hpp"

#include "logical.hpp"

namespace Ariadne {

inline LogicalValue operator!(LogicalValue lv) { return negation(lv); }
inline LogicalValue operator&&(LogicalValue lv1, LogicalValue lv2) { return conjunction(lv1,lv2); }
inline LogicalValue operator||(LogicalValue lv1, LogicalValue lv2) { return disjunction(lv1,lv2); }
inline LogicalValue operator^(LogicalValue lv1, LogicalValue lv2) { return exclusive(lv1,lv2); }

Logical<ExactTag> operator||(Bool b1, Logical<ExactTag> l2) { return Logical<ExactTag>(b1) || l2; }
Logical<ExactTag> operator||(Logical<ExactTag> l1, Bool b2) { return l1 || Logical<ExactTag>(b2); }
Logical<ExactTag> operator&&(Bool b1, Logical<ExactTag> l2) { return Logical<ExactTag>(b1) && l2; }
Logical<ExactTag> operator&&(Logical<ExactTag> l1, Bool b2) { return l1 && Logical<ExactTag>(b2); }

class LogicalConstant : public LogicalInterface {
    LogicalValue _v;
  public:
    LogicalConstant(LogicalValue v) : _v(v) { };
    virtual LogicalValue _check(Effort e) const { return _v; }
    virtual OutputStream& _write(OutputStream& os) const { return os << this->_v; }
};

template<class OP, class... ARGS> struct LogicalExpression;

template<class OP, class ARG> struct LogicalExpression<OP,ARG>
    : virtual LogicalInterface, ExpressionTemplate<OP,ARG>
{
    using ExpressionTemplate<OP,ARG>::ExpressionTemplate;
    virtual LogicalValue _check(Effort e) const { return this->_op(check(this->_arg,e)); }
    virtual OutputStream& _write(OutputStream& os) const {
        return os << static_cast<ExpressionTemplate<OP,ARG>const&>(*this); }
};

template<class OP, class ARG1, class ARG2> struct LogicalExpression<OP,ARG1,ARG2>
    : virtual LogicalInterface, ExpressionTemplate<OP,ARG1,ARG2>
{

    using ExpressionTemplate<OP,ARG1,ARG2>::ExpressionTemplate;
    virtual LogicalValue _check(Effort e) const { return this->_op(check(this->_arg1,e),check(this->_arg2,e)); }
    virtual OutputStream& _write(OutputStream& os) const {
        return os << static_cast<ExpressionTemplate<OP,ARG1,ARG2>const&>(*this); }
};

//FIXME: Should not need special treatment
template<class ARG1, class ARG2> struct LogicalExpression<Equal,ARG1,ARG2>
    : virtual LogicalInterface, ExpressionTemplate<Equal,ARG1,ARG2>
{
    using ExpressionTemplate<Equal,ARG1,ARG2>::ExpressionTemplate;
    virtual LogicalValue _check(Effort e) const { return equality(check(this->_arg1,e),check(this->_arg2,e)); }
    virtual OutputStream& _write(OutputStream& os) const {
        return os << static_cast<ExpressionTemplate<Equal,ARG1,ARG2>const&>(*this); }
};

LogicalHandle::LogicalHandle(LogicalValue l)
    : _ptr(std::make_shared<LogicalConstant>(l)) {
}

LogicalHandle conjunction(LogicalHandle l1, LogicalHandle l2) {
    return LogicalHandle(std::make_shared<LogicalExpression<AndOp,LogicalHandle,LogicalHandle>>(AndOp(),l1,l2));
};

LogicalHandle disjunction(LogicalHandle l1, LogicalHandle l2) {
    return LogicalHandle(std::make_shared<LogicalExpression<OrOp,LogicalHandle,LogicalHandle>>(OrOp(),l1,l2));
};

LogicalHandle negation(LogicalHandle l) {
    return LogicalHandle(std::make_shared<LogicalExpression<NotOp,LogicalHandle>>(NotOp(),l));
};

LogicalHandle equality(LogicalHandle l1, LogicalHandle l2) {
    return LogicalHandle(std::make_shared<LogicalExpression<Equal,LogicalHandle,LogicalHandle>>(Equal(),l1,l2));
};

LogicalHandle exclusive(LogicalHandle l1, LogicalHandle l2) {
    return LogicalHandle(std::make_shared<LogicalExpression<XOrOp,LogicalHandle,LogicalHandle>>(XOrOp(),l1,l2));
};

LogicalValue equality(LogicalValue l1, LogicalValue l2) {
    switch (l1) {
        case LogicalValue::TRUE:
            return l2;
        case LogicalValue::LIKELY:
            switch (l2) { case LogicalValue::TRUE: return LogicalValue::LIKELY; case LogicalValue::FALSE: return LogicalValue::UNLIKELY; default: return l2; }
        case LogicalValue::INDETERMINATE:
            return LogicalValue::INDETERMINATE;
        case LogicalValue::UNLIKELY:
            switch (l2) { case LogicalValue::TRUE: return LogicalValue::UNLIKELY; case LogicalValue::FALSE: return LogicalValue::LIKELY; default: return negation(l2); }
        case LogicalValue::FALSE:
            return negation(l2);
        default:
            return LogicalValue::INDETERMINATE;
    }
}


OutputStream& operator<<(OutputStream& os, LogicalValue l) {
    switch(l) {
        case LogicalValue::TRUE: os << "true"; break;
        case LogicalValue::LIKELY: os << "likely";  break;
        case LogicalValue::INDETERMINATE: os << "indeterminate";  break;
        case LogicalValue::UNLIKELY: os << "unlikely"; break;
        case LogicalValue::FALSE: os << "false"; break;
    }
    return os;
}

const Logical<EffectiveTag> indeterminate = Logical<EffectiveTag>(LogicalValue::INDETERMINATE);

template<> String class_name<ExactTag>() { return "Exact"; }
template<> String class_name<EffectiveTag>() { return "Effective"; }
template<> String class_name<ValidatedTag>() { return "Validated"; }
template<> String class_name<BoundedTag>() { return "Bounded"; }
template<> String class_name<UpperTag>() { return "Upper"; }
template<> String class_name<LowerTag>() { return "Lower"; }
template<> String class_name<ApproximateTag>() { return "Approximate"; }

template<> String class_name<Bool>() { return "Bool"; }
template<> String class_name<Boolean>() { return "Boolean"; }
template<> String class_name<Kleenean>() { return "Kleenean"; }
template<> String class_name<Sierpinskian>() { return "Sierpinskian"; }
template<> String class_name<NegatedSierpinskian>() { return "NegatedSierpinskian"; }
template<> String class_name<ValidatedKleenean>() { return "ValidatedKleenean"; }
template<> String class_name<ValidatedSierpinskian>() { return "ValidatedSierpinskian"; }
template<> String class_name<ValidatedNegatedSierpinskian>() { return "ValidatedNegatedSierpinskian"; }
template<> String class_name<Fuzzy>() { return "Fuzzy"; }

} // namespace Ariadne
