/***************************************************************************
 *            expression.cc
 *
 *  Copyright 2009  Pieter Collins
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

#include "expression.h"
#include "assignment.h"
#include "space.h"
#include "valuation.h"

#include "numeric.h"
#include "formula.h"

namespace Ariadne {

inline Real operator*(const Real& x1, int x2) { return x1*Real(x2); }
inline Real operator/(int x1, const Real& x2) { return Real(x1)/x2; }


template<class T> inline
std::ostream& operator<<(std::ostream& os, const ExpressionInterface<T>& e) { return e.write(os); }


//! A constant, viewed as a function \f$c:\R^n\rightarrow\R\f$.
template<class R>
class ConstantExpression
    : public ExpressionInterface<R>
{
  public:
    ConstantExpression(const std::string& nm, const R& c) : _name(nm), _c(c) { }
    ConstantExpression(const R& c) : _name(to_string(c)), _c(c) { }
    operator R () const { return _c; }
    R value() const { return _c; }
    virtual String name() const { return this->_name; }
    virtual Operator op() const { return CNST; }
    virtual Set<UntypedVariable> arguments() const { return Set<UntypedVariable>(); }
    virtual ConstantExpression<R>* clone() const { return new ConstantExpression<R>(*this); }
    virtual std::ostream& write(std::ostream& os) const { return os << _name; }
  protected:
    virtual ExpressionInterface<R>* simplify() const { return this->clone(); }
  private:
    String _name;
    R _c;
};

//! A constant, viewed as a function \f$c:\R^n\rightarrow\R\f$.
template<>
class ConstantExpression<Real>
    : public ExpressionInterface<Real>
{
  public:
    ConstantExpression(const std::string& nm, const Real& c) : _name(nm), _c(c) { }
    ConstantExpression(double c) : _name(to_string(c)), _c(c) { }
    ConstantExpression(const Real& c) : _name(to_string(c)), _c(c) { }
    operator Real() const { return _c; }
    Real value() const { return _c; }
    virtual String name() const { return this->_name; }
    virtual String operator_name() const { return "const"; }
    virtual Operator op() const { return CNST; }
    virtual Set<UntypedVariable> arguments() const { return Set<UntypedVariable>(); }
    virtual ConstantExpression<Real>* clone() const { return new ConstantExpression<Real>(*this); }
    virtual std::ostream& write(std::ostream& os) const { return os << _name; }
  protected:
    virtual ExpressionInterface<Real>* simplify() const { return this->clone(); }
  private:
    String _name;
    Real _c;
};


//! A projection onto a named variable.
template<class R>
class VariableExpression
    : public ExpressionInterface<R>
{
  public:
    explicit VariableExpression(const String& s) : _var(s) { }
    VariableExpression(const Variable<R>& v) : _var(v) { }
    const Variable<R>& variable() const { return this->_var; }
    String name() const { return this->_var.name(); }
    virtual String operator_name() const { return "var"; }
    virtual Operator op() const { return VAR; }
    virtual VariableExpression<R>* clone() const { return new VariableExpression<R>(*this); }
    virtual Set<UntypedVariable> arguments() const { Set<UntypedVariable> r; r.insert(this->_var); return r; }
    virtual std::ostream& write(std::ostream& os) const { return os << this->name(); }
  protected:
    virtual ExpressionInterface<R>* simplify() const {
        return this->clone(); }
  private:
    Variable<R> _var;
};

template<class T> class CoordinateExpression;

//! A coordinate projection \f$\pi:\R^n\rightarrow\R\f$ given by \f$\pi(x)=x_j\f$.
template<>
class CoordinateExpression<Real>
    : public ExpressionInterface<Real>
{
    typedef unsigned int SizeType;
  public:
    explicit CoordinateExpression() : _as(0), _j(0) { }
    explicit CoordinateExpression(SizeType j) : _as(0), _j(j) { }
    explicit CoordinateExpression(unsigned int as, unsigned int j) : _as(as), _j(j) { }
    SizeType argument_size() const { return _as; }
    SizeType index() const { return _j; }
    SizeType coordinate() const { return _j; }
    String name() const { String s("x0"); s[1]+=_j; return s; }
    virtual String operator_name() const { return "coord"; }
    virtual Operator op() const { return IND; }
    virtual CoordinateExpression<Real>* clone() const { return new CoordinateExpression<Real>(*this); }
    virtual Set<UntypedVariable> arguments() const { ARIADNE_FAIL_MSG("Cannot compute arguments of a CoordinateExpression"); }
    virtual std::ostream& write(std::ostream& os) const { return os << this->name(); }
  protected:
    virtual ExpressionInterface<Real>* simplify() const { return this->clone(); }
  private:
    SizeType _as;
    SizeType _j;
};



//! A composed scalar function, based on a standard operator.
template<class R, class Op=Operator, class A=R> class UnaryExpression
    : public ExpressionInterface<R>
{
  public:
    UnaryExpression(const Op& op, const ExpressionInterface<A>& expr)
        : _op(op), _arg(expr.clone()) { }
    UnaryExpression(const Op& op, const ExpressionInterface<A>* expr)
        : _op(op), _arg(expr) { }
    UnaryExpression(const Op& op, shared_ptr< const ExpressionInterface<A> > expr)
        : _op(op), _arg(expr) { }
    virtual String operator_name() const { return name(_op); }
    virtual Operator op() const { return static_cast<Operator>(_op); }
    virtual UnaryExpression<R,Op,A>* clone() const { return new UnaryExpression<R,Op,A>(_op,_arg._ptr); }
    virtual Set<UntypedVariable> arguments() const { return this->_arg.arguments(); }
    virtual std::ostream& write(std::ostream& os) const;
  protected:
    virtual ExpressionInterface<R>* simplify() const { return this->clone(); }
  public:
    Op _op;
    Expression<A> _arg;
};

//! A composed scalar function, based on a standard operator.
template<class R> class UnaryExpression<R,Operator,R>
    : public ExpressionInterface<R>
{
    typedef Operator Op; typedef R A;
  public:
    UnaryExpression(const Op& op, const ExpressionInterface<A>& expr)
        : _op(op), _arg(expr.clone()) { }
    UnaryExpression(const Op& op, const ExpressionInterface<A>* expr)
        : _op(op), _arg(expr) { }
    UnaryExpression(const Op& op, shared_ptr< const ExpressionInterface<A> > expr)
        : _op(op), _arg(expr) { }
    virtual String operator_name() const { return name(_op); }
    virtual Operator op() const { return static_cast<Operator>(_op); }
    virtual UnaryExpression<R,Op,A>* clone() const { return new UnaryExpression<R,Op,A>(_op,_arg._ptr); }
    virtual Set<UntypedVariable> arguments() const { return this->_arg.arguments(); }
    virtual std::ostream& write(std::ostream& os) const;
  protected:
    virtual ExpressionInterface<R>* simplify() const { return this->clone(); }
  public:
    Op _op;
    Expression<A> _arg;
};

template<class R, class Op, class A> inline std::ostream& UnaryExpression<R,Op,A>::write(std::ostream& os) const {
    switch(_op) {
        case NEG: return os << '-' << _arg;
        case NOT: return os << '!' << _arg;
        default: return os << _op << "(" << _arg << ")";
    }
}

template<class R> inline std::ostream& UnaryExpression<R,Operator,R>::write(std::ostream& os) const {
    switch(_op) {
        case NEG: return os << '-' << _arg;
        case NOT: return os << '!' << _arg;
        default: return os << _op << "(" << _arg << ")";
    }
}


//! A composed scalar function, based on an arthmetic operator.
template<class R, class Op=Operator, class A1=R, class A2=A1> class BinaryExpression
    : public ExpressionInterface<R>
{
  public:
    BinaryExpression(Op op, const ExpressionInterface<A1>& expr1, const ExpressionInterface<A2>& expr2)
        : _op(op), _arg1(expr1.clone()), _arg2(expr2.clone()) { }
    BinaryExpression(Op op, const ExpressionInterface<A1>* expr1, const ExpressionInterface<A2>* expr2)
        : _op(op), _arg1(expr1), _arg2(expr2) { }
    BinaryExpression(Op op, shared_ptr< const ExpressionInterface<A1> > expr1, shared_ptr< const ExpressionInterface<A2> > expr2)
        : _op(op), _arg1(expr1), _arg2(expr2)  { }
    virtual String operator_name() const { return name(_op); }
    virtual Operator op() const { return static_cast<Operator>(_op); }
    virtual BinaryExpression<R,Op,A1,A2>* clone() const { return new BinaryExpression<R,Op,A1,A2>(_op,_arg1._ptr,_arg2._ptr); }
    virtual Set<UntypedVariable> arguments() const { return join(this->_arg1.arguments(),this->_arg2.arguments()); }
    virtual std::ostream& write(std::ostream& os) const;
  protected:
    virtual ExpressionInterface<R>* simplify() const { return this->clone(); }
  public:
    Op _op;
    Expression<A1> _arg1;
    Expression<A2> _arg2;
};


template<class R, class Op, class A1, class A2> inline
std::ostream& BinaryExpression<R,Op,A1,A2>::write(std::ostream& os) const {
    switch(_op) {
        case AND: case OR:
        case ADD: case SUB: case MUL: case DIV:
        case EQ: case NEQ: case LT: case GT: case LEQ: case GEQ:
            return os << "(" << _arg1 << symbol(_op) << _arg2 << ")";
        default:
            return os << name(_op) << "(" << _arg1 << "," << _arg2 << ")";
    }
}




//! An expression in multiple variables of the same type
template<class R, class Op=Operator, class A=R> class MultiaryExpression
    : public ExpressionInterface<R>
{
  public:
    MultiaryExpression(Op op, const ExpressionInterface<A>& expr1, const ExpressionInterface<A>& expr2)
        : _op(op), _args() { _args.append(Expression<A>(expr1.clone())); _args.append(Expression<A>(expr2.clone()));  }
    MultiaryExpression(Op op, const ExpressionInterface<A>* expr1, const ExpressionInterface<A>* expr2)
        : _op(op), _args() { _args.append(Expression<A>(expr1)); _args.append(Expression<A>(expr2));  }
    MultiaryExpression(Op op, shared_ptr< const ExpressionInterface<A> > expr1, shared_ptr< const ExpressionInterface<A> > expr2)
        : _op(op), _args() { _args.append(expr1); _args.append(expr2);  }
    virtual unsigned int number_of_arguments() const { return _args.size(); }
    virtual String operator_name() const { return name(_op); }
    virtual Operator op() const { return static_cast<Operator>(_op); }
    virtual MultiaryExpression<R,Op,A>* clone() const { return new MultiaryExpression<R,Op,A>(_op,_args); }
    virtual Set<UntypedVariable> arguments() const {
        Set<UntypedVariable> res; for(uint i=0; i!=_args.size(); ++i) { res.adjoin(_args[i].arguments()); } return res; }
    virtual std::ostream& write(std::ostream& os) const {
        return os << _op << _args; }
/*
  private:
    template<class R, class A> inline
    void compute(R& r, const A& a) { r=Op()(_arg1->evaluate(a),_arg2->evaluate(a)); }
*/
  public:
    Op _op;
    List< Expression<A> > _args;
};



bool operator==(const Expression<Tribool>& e, bool v) {
    const ConstantExpression<Tribool>* expr = dynamic_cast<const ConstantExpression<Tribool>*>(e._raw_pointer());
    // No shortcircuit of return since right-hand side is a tribool
    return expr && bool(expr->value()==v);
}


template<class R> Expression<R>::Expression(const R& c)
    : _ptr(new ConstantExpression<R>(c))
{
}

template<class R> Expression<R>::Expression(const Constant<R>& c)
    : _ptr(new ConstantExpression<R>(c.name(),c.value()))
{
}

template<class R> Expression<R>::Expression(const Variable<R>& v)
    : _ptr(new VariableExpression<R>(v))
{
}

Expression<Real>::Expression(const double& c) : _ptr(new ConstantExpression<Real>(c)) { }
Expression<Real>::Expression(const Real& c) : _ptr(new ConstantExpression<Real>(c)) { }
Expression<Real>::Expression(const Constant<Real>& c) : _ptr(new ConstantExpression<Real>(c.name(),c.value())) { }
Expression<Real>::Expression(const Variable<Real>& v) : _ptr(new VariableExpression<Real>(v)) { }


template<class R> List< Expression<R> > Expression<R>::subexpressions() const {
    // Return the basic subexpressions used in forming the expression
    // Ideally, this code should be part of ExpressionInterface to avoid the
    // switching logic
    List< Expression<R> > res;
    const UnaryExpression<R>* uptr;
    const BinaryExpression<R>* bptr;
    uptr=dynamic_cast<const UnaryExpression<R>*>(this->_ptr.operator->());
    if(uptr) {
        res.append(uptr->_arg);
    } else {
        bptr=dynamic_cast<const BinaryExpression<R>*>(this->_ptr.operator->());
        if(bptr) {
            res.append(bptr->_arg1);
            res.append(bptr->_arg2);
        }
        else {
            ARIADNE_WARN("subexpressions of "<<*this<<"\n");
        }
    }
    return res;
}

List< Expression<Real> > Expression<Real>::subexpressions() const {
    // Return the basic subexpressions used in forming the expression
    // Ideally, this code should be part of ExpressionInterface to avoid the
    // switching logic
    List< Expression<R> > res;
    const UnaryExpression<R>* uptr;
    const BinaryExpression<R>* bptr;
    uptr=dynamic_cast<const UnaryExpression<R>*>(this->_ptr.operator->());
    if(uptr) {
        res.append(uptr->_arg);
    } else {
        bptr=dynamic_cast<const BinaryExpression<R>*>(this->_ptr.operator->());
        if(bptr) {
            res.append(bptr->_arg1);
            res.append(bptr->_arg2);
        }
    }
    return res;
}


bool identical(const Expression<Real>& e1, const Expression<Real>& e2) {
   if(e1._ptr==e2._ptr) { return true; }
    if(e1.op()!=e2.op()) { return false; }
    const VariableExpression<Real> *vptr1, *vptr2;
    const ConstantExpression<Real> *cptr1, *cptr2;
    switch(e1.op()) {
        case VAR:
            vptr1 = dynamic_cast<const VariableExpression<Real>*>(e1._raw_pointer());
            vptr2 = dynamic_cast<const VariableExpression<Real>*>(e2._raw_pointer());
            assert(vptr1); assert(vptr2);
            return (vptr1->variable()==vptr2->variable());
        case CNST:
            cptr1 = dynamic_cast<const ConstantExpression<Real>*>(e1._raw_pointer());
            cptr2 = dynamic_cast<const ConstantExpression<Real>*>(e2._raw_pointer());
            assert(cptr1); assert(cptr2);
            return (cptr1->value()==cptr2->value());
        default:
            List< Expression<Real> > sub1=e1.subexpressions();
            List< Expression<Real> > sub2=e2.subexpressions();
            bool result=true;
            for(uint i=0; i!=sub1.size(); ++i) {
                result = result && identical(sub1[i],sub2[i]);
            }
            return result;
    }
}



template class Expression<Boolean>;
template class Expression<Tribool>;
template class Expression<String>;
template class Expression<Integer>;
template class Expression<Real>;





template<class R, class Op, class A> inline
Expression<R> make_expression(Op op, Expression<A> e) {
    return Expression<R>(new UnaryExpression<R,Op,A>(op,e._ptr)); }
template<class R, class Op, class A1, class A2> inline
Expression<R> make_expression(Op op, Expression<A1> e1, Expression<A2> e2) {
    return Expression<R>(new BinaryExpression<R,Op,A1,A2>(op,e1._ptr,e2._ptr)); }

template<class R, class Op, class A> inline
Expression<R> make_expression(Op op, ExpressionInterface<A>* eptr) {
    return Expression<R>(new UnaryExpression<R,Op,A>(op,eptr)); }
template<class R, class Op, class A1, class A2> inline
Expression<R> make_expression(Op op, ExpressionInterface<A1>* e1ptr, ExpressionInterface<A2>* e2ptr) {
    return Expression<R>(new BinaryExpression<R,Op,A1,A2>(op,e1ptr,e2ptr)); }


Expression<Boolean> operator&&(Expression<Boolean> e1, Expression<Boolean> e2) {
    return make_expression<Boolean>(AND,e1,e2); }
Expression<Boolean> operator||(Expression<Boolean> e1, Expression<Boolean> e2) {
    return make_expression<Boolean>(OR,e1,e2); }
Expression<Boolean> operator!(Expression<Boolean> e) {
    return make_expression<Boolean>(NOT,e); }


Expression<tribool> operator&&(Expression<tribool> e1, Expression<tribool> e2) {
    return make_expression<tribool>(AND,e1,e2); }
Expression<tribool> operator||(Expression<tribool> e1, Expression<tribool> e2) {
    return make_expression<tribool>(OR,e1,e2); }
Expression<tribool> operator!(Expression<tribool> e) {
    return make_expression<tribool>(NOT,e); }


Expression<Boolean> operator==(Variable<String> v1, const String& s2) {
    return make_expression<Boolean>(EQ,Expression<String>(v1),Expression<String>(s2)); }
Expression<Boolean> operator!=(Variable<String> v1, const String& s2) {
    return make_expression<Boolean>(NEQ,Expression<String>(v1),Expression<String>(s2)); }


Expression<Boolean> operator==(Expression<Integer> e1, Expression<Integer> e2) {
    return make_expression<Boolean>(EQ,e1,e2); }
Expression<Boolean> operator!=(Expression<Integer> e1, Expression<Integer> e2) {
    return make_expression<Boolean>(NEQ,e1,e2); }
Expression<Boolean> operator>=(Expression<Integer> e1, Expression<Integer> e2) {
    return make_expression<Boolean>(GEQ,e1,e2); }
Expression<Boolean> operator<=(Expression<Integer> e1, Expression<Integer> e2) {
    return make_expression<Boolean>(LEQ,e1,e2); }
Expression<Boolean> operator>(Expression<Integer> e1, Expression<Integer> e2) {
    return make_expression<Boolean>(GT,e1,e2); }
Expression<Boolean> operator<(Expression<Integer> e1, Expression<Integer> e2) {
    return make_expression<Boolean>(LT,e1,e2); }



Expression<Integer> operator+(Expression<Integer> e) {
    return make_expression<Integer>(POS,e); }
Expression<Integer> operator-(Expression<Integer> e) {
    return make_expression<Integer>(NEG,e); }
Expression<Integer> operator+(Expression<Integer> e1, Expression<Integer> e2) {
    return make_expression<Integer>(ADD,e1,e2); }
Expression<Integer> operator-(Expression<Integer> e1, Expression<Integer> e2) {
    return make_expression<Integer>(SUB,e1,e2); }
Expression<Integer> operator*(Expression<Integer> e1, Expression<Integer> e2) {
    return make_expression<Integer>(MUL,e1,e2); }



Expression<Tribool> sgn(Expression<Real> e) {
    return make_expression<Tribool>(SGN,e); }

Expression<Tribool> operator==(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Tribool>(EQ,e1,e2); }
Expression<Tribool> operator!=(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Tribool>(NEQ,e1,e2); }
Expression<Tribool> operator>=(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Tribool>(GEQ,e1,e2); }
Expression<Tribool> operator<=(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Tribool>(LEQ,e1,e2); }
Expression<Tribool> operator>(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Tribool>(GT,e1,e2); }
Expression<Tribool> operator<(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Tribool>(LT,e1,e2); }


Expression<Real> operator+(Expression<Real> e) {
    return make_expression<Real>(POS,e); }
Expression<Real> operator-(Expression<Real> e) {
    return make_expression<Real>(NEG,e); }
Expression<Real> operator+(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Real>(ADD,e1,e2); }
Expression<Real> operator-(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Real>(SUB,e1,e2); }
Expression<Real> operator*(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Real>(MUL,e1,e2); }
Expression<Real> operator/(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Real>(DIV,e1,e2); }

Expression<Real> pow(Expression<Real> e, int n) {
    ARIADNE_NOT_IMPLEMENTED;
    //return make_expression(POW,e,n);
}

Expression<Real> neg(Expression<Real> e) {
    return make_expression<Real>(NEG,e); }
Expression<Real> rec(Expression<Real> e) {
    return make_expression<Real>(REC,e); }
Expression<Real> sqr(Expression<Real> e) {
    return make_expression<Real>(SQR,e); }
Expression<Real> sqrt(Expression<Real> e) {
    return make_expression<Real>(SQRT,e); }
Expression<Real> exp(Expression<Real> e) {
    return make_expression<Real>(EXP,e); }
Expression<Real> log(Expression<Real> e) {
    return make_expression<Real>(LOG,e); }
Expression<Real> sin(Expression<Real> e) {
    return make_expression<Real>(SIN,e); }
Expression<Real> cos(Expression<Real> e) {
    return make_expression<Real>(COS,e); }
Expression<Real> tan(Expression<Real> e) {
    return make_expression<Real>(TAN,e); }

Expression<Real> max(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Real>(MAX,e1,e2); }
Expression<Real> min(Expression<Real> e1, Expression<Real> e2) {
    return make_expression<Real>(MIN,e1,e2); }
Expression<Real> abs(Expression<Real> e) {
    return make_expression<Real>(ABS,e); }



inline void _set_constant(Real& r, const Real& c) { r=c; }
inline void _set_constant(Formula<Real>& r, const Real& c) { r=c; }

Boolean _compare(Operator cmp, const String& s1, const String& s2) {
    switch(cmp) {
        case EQ:  return s1==s2;
        case NEQ: return s1!=s2;
        default: ARIADNE_FAIL_MSG("Cannot evaluate comparison "<<cmp<<" on string arguments.");
    }
}

Boolean _compare(Operator cmp, const Integer& z1, const Integer& z2) {
    switch(cmp) {
        case EQ:  return z1==z2;
        case NEQ: return z1!=z2;
        case LEQ: return z1<=z2;
        case GEQ: return z1>=z2;
        case LT:  return z1< z2;
        case GT:  return z1> z2;
        default: ARIADNE_FAIL_MSG("Cannot evaluate comparison "<<cmp<<" on integer arguments.");
    }
}

template<class X> Tribool _compare(Operator cmp, const X& x1, const X& x2) {
    switch(cmp) {
        case GT: case GEQ: return x1>x2;
        case LT: case LEQ: return x1<x2;
        default: ARIADNE_FAIL_MSG("Cannot evaluate comparison "<<cmp<<" on real arguments.");
    }
}

Boolean _compute(Operator op, const Boolean& b) {
    switch(op) {
        case NOT: return !b;
        default: ARIADNE_FAIL_MSG("Cannot evaluate operator "<<op<<" on a boolean arguments.");
    }
}

Boolean _compute(Operator op, const Boolean& b1, const Boolean& b2) {
    switch(op) {
        case AND: return b1 && b2;
        case OR: return b1 || b2;
        case XOR: return b1 ^ b2;
        case IMPL: return !b1 || b2;
        default: ARIADNE_FAIL_MSG("Cannot evaluate operator "<<op<<" on a boolean arguments.");
    }
}

Tribool _compute(Operator op, const Tribool& b) {
    switch(op) {
        case NOT: return !b;
        default: ARIADNE_FAIL_MSG("Cannot evaluate operator "<<op<<" on a boolean arguments.");
    }
}

Tribool _compute(Operator op, const Tribool& b1, const Tribool& b2) {
    switch(op) {
        case AND: return b1 && b2;
        case OR: return b1 || b2;
        case XOR: return b1 ^ b2;
        case IMPL: return !b1 || b2;
        default: ARIADNE_FAIL_MSG("Cannot evaluate operator "<<op<<" on a boolean arguments.");
    }
}

Integer _compute(Operator op, const Integer& x1, const Integer& x2) {
    switch(op) {
        case ADD: return x1+x2;
        case SUB: return x1-x2;
        case MUL: return x1*x2;
        default: ARIADNE_FAIL_MSG("Cannot evaluate operator "<<op<<" on two integer arguments.");
    }
}

template<class X> X _compute(Operator op, const X& x1, const X& x2) {
    switch(op) {
        case ADD: return x1+x2;
        case SUB: return x1-x2;
        case MUL: return x1*x2;
        case DIV: return x1/x2;
        default: ARIADNE_FAIL_MSG("Cannot evaluate operator "<<op<<" on two real arguments.");
    }
}

Integer _compute(Operator op, const Integer& z) {
    switch(op) {
        case POS: return +z;
        case NEG: return -z;
        default: ARIADNE_FAIL_MSG("Cannot evaluate operator "<<op<<" on one integer argument.");
    }
}

template<class X>
X _compute(Operator op, const X& x) {
    switch(op) {
        case NEG: return -x;
        case REC: return 1/x;
        case EXP: return exp(x);
        case LOG: return log(x);
        case SIN: return sin(x);
        case COS: return cos(x);
        case TAN: return cos(x);
        default: ARIADNE_FAIL_MSG("Cannot evaluate operator "<<op<<" on one real argument.");
    }
}



String evaluate(const Expression<String>& e, const StringValuation& x) {
    const ExpressionInterface<String>* eptr=e._raw_pointer();
    const ConstantExpression<String>* cptr=dynamic_cast<const ConstantExpression<String>*>(eptr);
    if(cptr) { return cptr->value(); }
    const VariableExpression<String>* vptr=dynamic_cast<const VariableExpression<String>*>(eptr);
    if(vptr) { return x[vptr->variable()]; }
    ARIADNE_FAIL_MSG("Cannot evaluate expression "<<e<<" to a String using variables "<<x);
}

Integer evaluate(const Expression<Integer>& e, const IntegerValuation& x) {
    const ExpressionInterface<Integer>* eptr=e._raw_pointer();
    const BinaryExpression<Integer>* bptr=dynamic_cast<const BinaryExpression<Integer>*>(eptr);
    if(bptr) { return _compute(bptr->_op,evaluate(bptr->_arg1,x),evaluate(bptr->_arg2,x)); }
    const UnaryExpression<Integer>* uptr=dynamic_cast<const UnaryExpression<Integer>*>(eptr);
    if(uptr) { return _compute(uptr->_op,evaluate(uptr->_arg,x)); }
    const ConstantExpression<Integer>* cptr=dynamic_cast<const ConstantExpression<Integer>*>(eptr);
    if(cptr) { return cptr->value(); }
    const VariableExpression<Integer>* vptr=dynamic_cast<const VariableExpression<Integer>*>(eptr);
    if(vptr) { return x[vptr->variable()]; }
    ARIADNE_FAIL_MSG("Cannot evaluate expression "<<e<<" to an Integer using variables "<<x);
}

Boolean evaluate(const Expression<Boolean>& e, const StringValuation& x) {
    const ExpressionInterface<Boolean>* eptr=e._raw_pointer();
    const BinaryExpression<Boolean>* bptr=dynamic_cast<const BinaryExpression<Boolean,Operator>*>(eptr);
    if(bptr) { return _compute(bptr->_op,evaluate(bptr->_arg1,x),evaluate(bptr->_arg2,x)); }
    const UnaryExpression<Boolean>* uptr=dynamic_cast<const UnaryExpression<Boolean>*>(eptr);
    if(uptr) { return _compute(uptr->_op,evaluate(uptr->_arg,x)); }
    const ConstantExpression<Boolean>* cptr=dynamic_cast<const ConstantExpression<Boolean>*>(eptr);
    if(cptr) { return cptr->value(); }
    const BinaryExpression<Boolean,Operator,String>* bsptr=dynamic_cast<const BinaryExpression<Boolean,Operator,String>*>(eptr);
    if(bsptr) { return _compare(bsptr->_op,evaluate(bsptr->_arg1,x),evaluate(bsptr->_arg2,x)); }
    ARIADNE_FAIL_MSG("Cannot evaluate expression "<<e<<" to a Boolean using variables "<<x);
}




Boolean evaluate(const Expression<Boolean>& e, const DiscreteValuation& x) {
    const ExpressionInterface<Boolean>* eptr=e._raw_pointer();
    const BinaryExpression<Boolean>* bptr=dynamic_cast<const BinaryExpression<Boolean,Operator>*>(eptr);
    if(bptr) { return _compute(bptr->_op,evaluate(bptr->_arg1,x),evaluate(bptr->_arg2,x)); }
    const UnaryExpression<Boolean>* uptr=dynamic_cast<const UnaryExpression<Boolean>*>(eptr);
    if(uptr) { return _compute(uptr->_op,evaluate(uptr->_arg,x)); }
    const ConstantExpression<Boolean>* cptr=dynamic_cast<const ConstantExpression<Boolean>*>(eptr);
    if(cptr) { return cptr->value(); }
    const BinaryExpression<Boolean,Operator,String>* bsptr=dynamic_cast<const BinaryExpression<Boolean,Operator,String>*>(eptr);
    if(bsptr) { return _compare(bsptr->_op,evaluate(bsptr->_arg1,x),evaluate(bsptr->_arg2,x)); }
    const BinaryExpression<Boolean,Operator,Integer>* bzptr=dynamic_cast<const BinaryExpression<Boolean,Operator,Integer>*>(eptr);
    if(bzptr) { return _compare(bsptr->_op,evaluate(bzptr->_arg1,x),evaluate(bzptr->_arg2,x)); }
    ARIADNE_FAIL_MSG("Cannot evaluate expression "<<e<<" to a Boolean using variables "<<x);
}

template<class X> Tribool evaluate(const Expression<Tribool>& e, const ContinuousValuation<X>& x) {
    const ExpressionInterface<Tribool>* eptr=e._raw_pointer();
    const BinaryExpression<Tribool>* bptr=dynamic_cast<const BinaryExpression<Tribool>*>(eptr);
    if(bptr) { return _compute(bptr->_op,evaluate(bptr->_arg1,x),evaluate(bptr->_arg2,x)); }
    const BinaryExpression<Tribool,Operator,Real,Real>* brptr=dynamic_cast<const BinaryExpression<Tribool,Operator,Real,Real>*>(eptr);
    if(brptr) { return _compare(bptr->_op,evaluate(brptr->_arg1,x),evaluate(brptr->_arg2,x)); }
    ARIADNE_FAIL_MSG("");
}

template<class X> X evaluate(const Expression<Real>& e, const ContinuousValuation<X>& x) {
    const ExpressionInterface<Real>* eptr=e._raw_pointer();
    const BinaryExpression<Real>* bptr=dynamic_cast<const BinaryExpression<Real>*>(eptr);
    if(bptr) { return _compute(bptr->_op,evaluate(bptr->_arg1,x),evaluate(bptr->_arg2,x)); }
    const UnaryExpression<Real>* uptr=dynamic_cast<const UnaryExpression<Real>*>(eptr);
    if(uptr) { return _compute(uptr->_op,evaluate(uptr->_arg,x)); }
    const ConstantExpression<Real>* cptr=dynamic_cast<const ConstantExpression<Real>*>(eptr);
    if(cptr) { X r; _set_constant(r,cptr->value()); return r; }
    const VariableExpression<Real>* vptr=dynamic_cast<const VariableExpression<Real>*>(eptr);
    if(vptr) { return x[vptr->variable()]; }
    ARIADNE_FAIL_MSG("");
}

template<class X> X evaluate(const Expression<Real>& e, const Map<ExtendedRealVariable,X>& x) {
    const ExpressionInterface<Real>* eptr=e._raw_pointer();
    const BinaryExpression<Real>* bptr=dynamic_cast<const BinaryExpression<Real>*>(eptr);
    if(bptr) { return _compute(bptr->_op,evaluate(bptr->_arg1,x),evaluate(bptr->_arg2,x)); }
    const UnaryExpression<Real>* uptr=dynamic_cast<const UnaryExpression<Real>*>(eptr);
    if(uptr) { return _compute(uptr->_op,evaluate(uptr->_arg,x)); }
    const ConstantExpression<Real>* cptr=dynamic_cast<const ConstantExpression<Real>*>(eptr);
    if(cptr) { X r=x.begin()->second*0; _set_constant(r,cptr->value()); return r; }
    const VariableExpression<Real>* vptr=dynamic_cast<const VariableExpression<Real>*>(eptr);
    if(vptr) { ARIADNE_ASSERT_MSG(x.has_key(vptr->variable()),"Valuation "<<x<<" does not contain variable "<<vptr->variable()<<" used in expression "<<e); return x[vptr->variable()]; }
    ARIADNE_FAIL_MSG("");
}


template Tribool evaluate(const Expression<Tribool>& e, const ContinuousValuation<Real>& x);
template Real evaluate(const Expression<Real>& e, const ContinuousValuation<Real>& x);


template Real evaluate(const Expression<Real>& e, const Map<ExtendedRealVariable,Real>& x);
template Formula<Real> evaluate(const Expression<Real>& e, const Map< ExtendedRealVariable, Formula<Real> >& x);


template<class X, class Y> Expression<X> substitute_variable(const VariableExpression<X>& e, const Variable<Y>& v, const Y& c) {
    return Expression<X>(e.clone()); }
template<class X> Expression<X> substitute_variable(const VariableExpression<X>& e, const Variable<X>& v, const X& c) {
    if(e.variable()==v) { return Expression<X>(c); } else { return Expression<X>(e.clone()); } }
template<class X, class Y> Expression<X> substitute_variable(const VariableExpression<X>& e, const Variable<Y>& v, const Expression<Y>& c) {
    return Expression<X>(e.clone()); }
template<class X> Expression<X> substitute_variable(const VariableExpression<X>& e, const Variable<X>& v, const Expression<X>& c) {
    if(e.variable()==v) { return Expression<X>(c); } else { return Expression<X>(e.clone()); } }


template<class X, class Y> Expression<X> substitute(const Expression<X>& e, const Variable<Y>& v, const Y& c) {
    const ExpressionInterface<X>* eptr=e._raw_pointer();
    const BinaryExpression<X,Operator,Y,Y>* aptr=dynamic_cast<const BinaryExpression<X,Operator,Y,Y>*>(eptr);
    if(aptr) { return make_expression<X>(aptr->_op,substitute(aptr->_arg1,v,c),substitute(aptr->_arg2,v,c)); }
    const BinaryExpression<X>* bptr=dynamic_cast<const BinaryExpression<X>*>(eptr);
    if(bptr) { return make_expression<X>(bptr->_op,substitute(bptr->_arg1,v,c),substitute(bptr->_arg2,v,c)); }
    const UnaryExpression<X>* uptr=dynamic_cast<const UnaryExpression<X>*>(eptr);
    if(uptr) { return make_expression<X>(uptr->_op,substitute(uptr->_arg,v,c)); }
    const ConstantExpression<X>* cptr=dynamic_cast<const ConstantExpression<X>*>(eptr);
    if(cptr) { return e; }
    const VariableExpression<X>* vptr=dynamic_cast<const VariableExpression<X>*>(eptr);
    if(vptr) { return substitute_variable(*vptr,v,c); }
    ARIADNE_FAIL_MSG("Cannot substitute for a named variable in an unknown expression.");
}

template<class X, class Y> Expression<X> substitute(const Expression<X>& e, const Variable<Y>& v, const Expression<Y>& c) {
    const ExpressionInterface<X>* eptr=e._raw_pointer();
    const BinaryExpression<X,Operator,Y,Y>* aptr=dynamic_cast<const BinaryExpression<X,Operator,Y,Y>*>(eptr);
    if(aptr) { return make_expression<X>(aptr->_op,substitute(aptr->_arg1,v,c),substitute(aptr->_arg2,v,c)); }
    const BinaryExpression<X>* bptr=dynamic_cast<const BinaryExpression<X>*>(eptr);
    if(bptr) { return make_expression<X>(bptr->_op,substitute(bptr->_arg1,v,c),substitute(bptr->_arg2,v,c)); }
    const UnaryExpression<X>* uptr=dynamic_cast<const UnaryExpression<X>*>(eptr);
    if(uptr) { return make_expression<X>(uptr->_op,substitute(uptr->_arg,v,c)); }
    const ConstantExpression<X>* cptr=dynamic_cast<const ConstantExpression<X>*>(eptr);
    if(cptr) { return e; }
    const VariableExpression<X>* vptr=dynamic_cast<const VariableExpression<X>*>(eptr);
    if(vptr) { return substitute_variable(*vptr,v,c); }
    ARIADNE_FAIL_MSG("Cannot substitute "<<c<<" for a named variable "<<v<<" in an unknown expression "<<e<<"\n");
}

template<class X, class Y> Expression<X> substitute(const Expression<X>& e, const List< Assignment< Variable<Y>, Expression<Y> > >& a) {
    Expression<X> r=e;
    for(uint i=0; i!=a.size(); ++i) {
        r=substitute(r,a[i].lhs,a[i].rhs);
    }
    return r;
}

template Expression<Tribool> substitute(const Expression<Tribool>& e, const Variable<Tribool>& v, const Tribool& c);
template Expression<Tribool> substitute(const Expression<Tribool>& e, const Variable<Real>& v, const Real& c);
template Expression<Real> substitute(const Expression<Real>& e, const Variable<Real>& v, const Real& c);
template Expression<Real> substitute(const Expression<Real>& e, const Variable<Real>& v, const Expression<Real>& c);
template Expression<Real> substitute(const Expression<Real>& e, const List< Assignment< Variable<Real>, Expression<Real> > >& c);
template Expression<Tribool> substitute(const Expression<Tribool>& e, const List< Assignment< Variable<Real>, Expression<Real> > >& c);

bool is_constant(const Expression<Tribool>& e, const Boolean& c) {
    const ExpressionInterface<Tribool>* eptr=e._raw_pointer();
    const ConstantExpression<Tribool>* cptr=dynamic_cast<const ConstantExpression<Tribool>*>(eptr);
    if(cptr==0) { return false; }
    return (cptr->value()==c);
}

bool is_variable(const Expression<Real>& e, const Identifier& v) {
    const ExpressionInterface<Real>* eptr=e._raw_pointer();
    const VariableExpression<Real>* cptr=dynamic_cast<const VariableExpression<Real>*>(eptr);
    if(cptr==0) { return false; }
    return (cptr->name()==v);
}

namespace {

template<class X> inline Expression<X> _simplify(const Expression<X>& e) {
    return e;
}

template<> inline Expression<Real> _simplify(const Expression<Real>& e) {
    //return e;
    const ExpressionInterface<Real>* eptr=e._raw_pointer();
    //const UnaryExpression<Real>* uptr=static_cast<const UnaryExpression<Real>*>(eptr);
    const BinaryExpression<Real>* bptr=dynamic_cast<const BinaryExpression<Real>*>(eptr);
    if(!bptr) { return e; }
    Expression<Real> sarg1=simplify(bptr->_arg1);
    Expression<Real> sarg2=simplify(bptr->_arg2);
    Expression<Real> zero(0.0);
    Expression<Real> one(1.0);
    switch(eptr->op()) {
        case ADD:
            if(identical(sarg2,zero)) { return sarg1; }
            if(identical(sarg1,zero)) { return sarg2; }
            break;
        case SUB:
            if(identical(sarg2,zero)) { return sarg1; }
            if(identical(sarg1,zero)) { return -sarg2; }
            break;
        case MUL:
            if(identical(sarg1,zero)) { return sarg1; }
            if(identical(sarg2,zero)) { return sarg2; }
            if(identical(sarg1,one)) { return sarg2; }
            if(identical(sarg2,one)) { return sarg1; }
            break;
        case DIV:
            if(identical(sarg1,zero)) { return sarg1; }
            if(identical(sarg1,one)) { return rec(sarg2); }
            if(identical(sarg2,one)) { return sarg1; }
        default:
            break;
    }
    return e;

}


template<> inline Expression<Tribool> _simplify(const Expression<Tribool>& e) {
    const ExpressionInterface<Tribool>* eptr=e._ptr.operator->();
    const UnaryExpression<Tribool>* uptr=dynamic_cast<const UnaryExpression<Tribool>*>(eptr);
    if(uptr) {
        Expression<Tribool> sarg=simplify(uptr->_arg);
        if(uptr->_op==NOT) {
            const UnaryExpression<Tribool>* nuptr=dynamic_cast<const UnaryExpression<Tribool>*>(sarg._ptr.operator->());
            if(nuptr && nuptr->_op==NOT) {
                return nuptr->_arg;
            }
            const ConstantExpression<Tribool>* ncptr=dynamic_cast<const ConstantExpression<Tribool>*>(sarg._ptr.operator->());
            if(ncptr) {
                return Expression<Tribool>(!ncptr->value());
            }
        }
        return make_expression<Tribool>(uptr->_op,sarg);
    }
    const BinaryExpression<Tribool>* bptr=dynamic_cast<const BinaryExpression<Tribool>*>(eptr);
    if(bptr) {
        Expression<Tribool> sarg1=simplify(bptr->_arg1);
        Expression<Tribool> sarg2=simplify(bptr->_arg2);
        const ConstantExpression<Tribool>* carg1ptr=dynamic_cast<const ConstantExpression<Tribool>*>(sarg1._ptr.operator->());
        const ConstantExpression<Tribool>* carg2ptr=dynamic_cast<const ConstantExpression<Tribool>*>(sarg2._ptr.operator->());
        if(carg1ptr && carg2ptr) {
            if(bptr->_op==AND) { return Expression<Tribool>(carg1ptr->value() && carg2ptr->value()); }
            if(bptr->_op==OR) { return Expression<Tribool>(carg1ptr->value() || carg2ptr->value()); }
        } else if(carg1ptr) {
            if(bptr->_op==AND && carg1ptr->value()==true) { return sarg2; }
            if(bptr->_op==AND && carg1ptr->value()==false) { return sarg1; }
            if(bptr->_op==OR && carg1ptr->value()==true) { return sarg1; }
            if(bptr->_op==OR && carg1ptr->value()==false) { return sarg2; }
        } else if(carg2ptr) {
            if(bptr->_op==AND && carg2ptr->value()==true) { return sarg1; }
            if(bptr->_op==AND && carg2ptr->value()==false) { return sarg2; }
            if(bptr->_op==OR && carg2ptr->value()==true) { return sarg2; }
            if(bptr->_op==OR && carg2ptr->value()==false) { return sarg1; }
        } else {
            return make_expression<Tribool>(bptr->_op,sarg1,sarg2);
        }
    }
    return e;
}

} // namespace

template<class X> Expression<X> simplify(const Expression<X>& e) {
    return Ariadne::_simplify(e);
}

template Expression<Real> simplify(const Expression<Real>& e);
template Expression<Tribool> simplify(const Expression<Tribool>& e);


Expression<Real> derivative(const Expression<Real>& e, const Variable<Real>& v)
{
    const ExpressionInterface<Real>* eptr=e._raw_pointer();
    const VariableExpression<Real>* vptr=static_cast<const VariableExpression<Real>*>(eptr);
    const UnaryExpression<Real>* uptr=static_cast<const UnaryExpression<Real>*>(eptr);
    const BinaryExpression<Real>* bptr=static_cast<const BinaryExpression<Real>*>(eptr);
    switch(eptr->op()) {
        case CNST:
            return Expression<Real>(0.0);
        case VAR:
            if(vptr->variable()==v) { return Expression<Real>(1.0); }
            else { return Expression<Real>(0.0); }
        case ADD:
            return simplify( derivative(bptr->_arg1,v)+derivative(bptr->_arg2,v) );
        case SUB:
            return simplify( derivative(bptr->_arg1,v)-derivative(bptr->_arg2,v) );
        case MUL:
            return simplify( bptr->_arg1*derivative(bptr->_arg2,v)+derivative(bptr->_arg1,v)*bptr->_arg2 );
        case DIV:
            return simplify( derivative(bptr->_arg1 * rec(bptr->_arg2),v) );
        case NEG:
            return simplify( - derivative(uptr->_arg,v) );
        case REC:
            return simplify( - derivative(uptr->_arg,v) * rec(sqr(uptr->_arg)) );
        case SQR:
            return simplify( 2 * derivative(uptr->_arg,v) * uptr->_arg );
        case EXP:
            return derivative(uptr->_arg,v) * uptr->_arg;
        case LOG:
            return derivative(uptr->_arg,v) * rec(uptr->_arg);
        case SIN:
            return derivative(uptr->_arg,v) * cos(uptr->_arg);
        case COS:
            return -derivative(uptr->_arg,v) * sin(uptr->_arg);
        case TAN:
            return derivative(uptr->_arg,v) * (1-sqr(uptr->_arg));
        default:
            ARIADNE_THROW(std::runtime_error,"derivative(RealExpression,RealVariable)",
                          "Cannot compute derivative of "<<e<<"\n");
    }
}


Expression<Real> indicator(Expression<tribool> e, Sign sign) {
    ExpressionInterface<tribool>* eptr=const_cast<ExpressionInterface<tribool>*>(e._ptr.operator->());
    ConstantExpression<Tribool>* cnptr;
    BinaryExpression<Tribool,Operator,Real,Real>* cptr;
    BinaryExpression<Tribool,Operator,Tribool,Tribool>* bptr;
    UnaryExpression<tribool,Operator,Tribool>* nptr;
    tribool value;
    switch(eptr->op()) {
        case CNST:
            cnptr=dynamic_cast<ConstantExpression<Tribool>*>(eptr);
            value=( sign==POSITIVE ? cnptr->value() : !cnptr->value() );
            if(value==true) { return RealExpression(+1.0); }
            else if(value==false) {  return RealExpression(-1.0); }
            else { return RealExpression(0.0); }
        case GEQ: case GT:
            cptr=dynamic_cast<BinaryExpression<Tribool,Operator,Real,Real>*>(eptr);
            assert(cptr);
            if(sign==POSITIVE) { return cptr->_arg1-cptr->_arg2; }
            else { return cptr->_arg2-cptr->_arg1; }
        case LEQ: case LT:
            cptr=dynamic_cast<BinaryExpression<Tribool,Operator,Real,Real>*>(eptr);
            assert(cptr);
            if(sign==POSITIVE) { return cptr->_arg2-cptr->_arg1; }
            else { return cptr->_arg1-cptr->_arg2; }
        case AND:
            bptr=dynamic_cast<BinaryExpression<Tribool,Operator,Tribool,Tribool>*>(eptr);
            assert(bptr);
            return min(indicator(bptr->_arg1,sign),indicator(bptr->_arg2,sign));
        case OR:
            bptr=dynamic_cast<BinaryExpression<tribool,Operator,Tribool,Tribool>*>(eptr);
            assert(bptr);
            return max(indicator(bptr->_arg1,sign),indicator(bptr->_arg2,sign));
        case NOT:
            nptr=dynamic_cast<UnaryExpression<tribool,Operator,Tribool>*>(eptr);
            assert(nptr);
            return neg(indicator(nptr->_arg,sign));
        default:
            ARIADNE_FAIL_MSG("Cannot compute indicator function of expression " << *eptr);
    }
}


tribool opposite(Expression<tribool> e1, Expression<tribool> e2) {
    // Simple test if two expressions are negations of each other.
    // Current

    typedef BinaryExpression<Tribool,Operator,Real,Real> ComparisonInterface;

    ExpressionInterface<tribool>* e1ptr=const_cast<ExpressionInterface<tribool>*>(e1._raw_pointer());
    ExpressionInterface<tribool>* e2ptr=const_cast<ExpressionInterface<tribool>*>(e2._raw_pointer());

    Operator e1op, e2op;
    switch(e1ptr->op()) {
        case GEQ: case GT:
            e1op=GEQ; break;
        case LEQ: case LT:
            e1op=LEQ; break;
        default:
            return indeterminate;
    }
    switch(e2ptr->op()) {
        case GEQ: case GT:
            e2op=GEQ; break;
        case LEQ: case LT:
            e2op=LEQ; break;
        default:
            return indeterminate;
    }

    // Both expressions are <=,<,>=,> comparisons
    ComparisonInterface* c1ptr=dynamic_cast<ComparisonInterface*>(e1ptr);
    ComparisonInterface* c2ptr=dynamic_cast<ComparisonInterface*>(e2ptr);
    assert(c1ptr); assert(c2ptr);
    Expression<Real> e1arg1=c1ptr->_arg1;
    Expression<Real> e1arg2=c1ptr->_arg2;
    Expression<Real> e2arg1=c2ptr->_arg1;
    Expression<Real> e2arg2=c2ptr->_arg2;

    // Test if the expressions are of the form a1<=a2; a1>=a2 or a1<=a2; a2<=a1
    if(e1op==e2op) {
        if(identical(e1arg1,e2arg2) && identical(e1arg2,e2arg1)) { return true; }
        else { return indeterminate; }
    } else {
        if(identical(e1arg1,e2arg1) && identical(e1arg2,e2arg2)) { return true; }
        else { return indeterminate; }
    }

}

uint dimension(const Space<Real>& spc)
{
    return spc.size();
}

uint len(const List< Variable<Real> >& vars)
{
    return vars.size();
}


Formula<Real> formula(const Expression<Real>& e, const Space<Real>& spc)
{
    const ExpressionInterface<Real>* eptr=e._raw_pointer();
    const ConstantExpression<Real>* cptr=static_cast<const ConstantExpression<Real>*>(eptr);
    const VariableExpression<Real>* vptr=static_cast<const VariableExpression<Real>*>(eptr);
    const UnaryExpression<Real>* uptr=static_cast<const UnaryExpression<Real>*>(eptr);
    const BinaryExpression<Real>* bptr=static_cast<const BinaryExpression<Real>*>(eptr);
    switch(eptr->op()) {
        case CNST:
            return Formula<Real>::constant(cptr->value());
        case VAR:
            return Formula<Real>::coordinate(uint(spc.index(vptr->variable())));
        case ADD:
            return formula(bptr->_arg1,spc)+formula(bptr->_arg2,spc);
        case SUB:
            return formula(bptr->_arg1,spc)-formula(bptr->_arg2,spc);
        case MUL:
            return formula(bptr->_arg1,spc)*formula(bptr->_arg2,spc);
        case DIV:
            return formula(bptr->_arg1,spc)/formula(bptr->_arg2,spc);
        case NEG:
            return neg(formula(uptr->_arg,spc));
        case REC:
            return rec(formula(uptr->_arg,spc));
        case SQR:
            return sqr(formula(uptr->_arg,spc));
        case EXP:
            return exp(formula(uptr->_arg,spc));
        case LOG:
            return log(formula(uptr->_arg,spc));
        case SIN:
            return sin(formula(uptr->_arg,spc));
        case COS:
            return cos(formula(uptr->_arg,spc));
        case TAN:
            return tan(formula(uptr->_arg,spc));
        default:
            ARIADNE_THROW(std::runtime_error,"formula(RealExpression,RealSpace)",
                          "Cannot compute formula for "<<e<<"\n");
    }
}

Formula<Real> formula(const Expression<Real>& e, const List< Variable<Real> >& vars)
{
    return formula(e,Space<Real>(vars));
}

Formula<Real> formula(const Expression<Real>& out, const List< Assignment< Variable<Real>, Expression<Real> > >& aux, const Space<Real> spc)
{
    return formula(substitute(out,aux),spc);
}

List< Formula<Real> > formula(const List< Expression<Real> >& out, const List< Assignment< Variable<Real>, Expression<Real> > >& aux, const Space<Real> spc)
{
    List< Formula<Real> > res;
    for(uint i=0; i!=out.size(); ++i) {
        res.append(formula(out[i],aux,spc));
    }
    return res;
}



} // namespace Ariadne
