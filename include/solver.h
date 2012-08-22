/***************************************************************************
 *            solver.h
 *
 *  Copyright  2006-9  Pieter Collins
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

/*! \file solver.h
 *  \brief Solver classes for algebraic equations.
 */

#ifndef ARIADNE_SOLVER_H
#define ARIADNE_SOLVER_H

#include <exception>
#include <stdexcept>
#include <string>

#include "solver_interface.h"
#include "function_interface.h"

#include "logging.h"
#include "attribute.h"
#include "pointer.h"
#include "container.h"
#include "numeric.h"

namespace Ariadne {

template<class X> class Vector;
template<class X> class FunctionModelFactoryInterface;

/*! \ingroup \ingroup Solvers
 *  \brief %Common functionality for solving (nonlinear) equations.
 */
class SolverBase
    : public SolverInterface
{
  public:
    /*! \brief Constructor. */
    SolverBase(double max_error, uint max_steps);

    /*! \brief The maximum permissible error of the solution. */
    double maximum_error() const { return this->_max_error; }
    /*! \brief Set the maximum error. */
    void set_maximum_error(double max_error) { this->_max_error=max_error; };

    /*! \brief The maximum number of steps allowed before the method must quit. */
    uint maximum_number_of_steps() const { return this->_max_steps; }
    /*! \brief Set the maximum number of steps. */
    void set_maximum_number_of_steps(uint max_steps) { this->_max_steps=max_steps; };

    /*! \brief The class which constructs functions for the implicit function solver. */
    const FunctionModelFactoryInterface<Interval>& function_factory() const;
    /*! \brief Set the class which constructs functions for the implicit function solver. */
    void set_function_factory(const FunctionModelFactoryInterface<Interval>& factory);

    /*! \brief Solve \f$f(x)=0\f$, starting in the interval point \a pt. */
    virtual Vector<Interval> zero(const IntervalVectorFunction& f,const Vector<Interval>& pt) const;
    /*! \brief Solve \f$f(x)=0\f$, starting in the interval point \a pt. */
    virtual Vector<Interval> fixed_point(const IntervalVectorFunction& f,const Vector<Interval>& pt) const;

    /*! \brief Solve \f$f(x)=0\f$, starting in the interval point \a pt. */
    virtual Vector<Interval> solve(const IntervalVectorFunction& f,const Vector<Interval>& pt) const;
    /*! \brief Solve \f$f(a,x)=0\f$ for a in \a par, looking for a solution with x in \a ix. */
    virtual IntervalVectorFunctionModel implicit(const IntervalVectorFunction& f, const Vector<Interval>& par, const Vector<Interval>& ix) const;
    /*! \brief Solve \f$f(a,x)=0\f$ for a in \a par, looking for a solution with x in \a ix. */
    virtual IntervalScalarFunctionModel implicit(const IntervalScalarFunction& f, const Vector<Interval>& par, const Interval& ix) const;

    /*! \brief Solve \f$f(x)=0\f$, starting in the interval point \a pt. */
    virtual Set< Vector<Interval> > solve_all(const IntervalVectorFunction& f,const Vector<Interval>& pt) const;
  protected:
    /*! \brief Perform one iterative step of the contractor. */
    virtual Vector<Interval> step(const IntervalVectorFunction& f,const Vector<Interval>& pt) const = 0;
    /*! \brief Perform one iterative step of the contractor. */
    virtual IntervalVectorFunctionModel implicit_step(const IntervalVectorFunction& f,const IntervalVectorFunctionModel& p,const IntervalVectorFunctionModel& x) const = 0;
  private:
    double _max_error;
    uint _max_steps;
    std::shared_ptr< FunctionModelFactoryInterface<Interval> > _function_factory_ptr;
};


/*! \ingroup Solvers
 *  \brief Interval Newton solver. Uses the contractor \f$[x']=x_0-Df^{-1}([x])f(x_0)\f$.
 */
class IntervalNewtonSolver
    : public SolverBase
{
  public:
    /*! \brief Constructor. */
    IntervalNewtonSolver(double max_error, uint max_steps) : SolverBase(max_error,max_steps) { }
    IntervalNewtonSolver(MaximumError max_error, MaximumNumberOfSteps max_steps) : SolverBase(max_error,max_steps) { }
    /*! \brief Cloning operator. */
    virtual IntervalNewtonSolver* clone() const { return new IntervalNewtonSolver(*this); }
    /*! \brief Write to an output stream. */
    virtual void write(std::ostream& os) const;

    using SolverBase::implicit;

    /*! \brief Solve \f$f(a,x)=0\f$ for a in \a par, looking for solutions with x in \a ix. */
    virtual IntervalVectorFunctionModel implicit_step(const IntervalVectorFunction& f, const IntervalVectorFunctionModel& p, const IntervalVectorFunctionModel& x) const;
    /*! \brief Solve \f$f(a,x)=0\f$ for a in \a par, looking for a solution with x in \a ix. */
    virtual IntervalScalarFunctionModel implicit(const IntervalScalarFunction& f, const Vector<Interval>& par, const Interval& ix) const;
  public:
    virtual Vector<Interval>
    step(const IntervalVectorFunction& f,
         const Vector<Interval>& pt) const;
};


/*! \ingroup Solvers
 *  \brief Interval Newton solver. Uses the contractor \f$[x']=x_0-Df^{-1}([x])f(x_0)\f$.
 *  Performs implicit function computation by first guessing a trial solution function.
 */
class GuessingIntervalNewtonSolver
    : public IntervalNewtonSolver
{
    /*! \brief Solve \f$f(a,x)=0\f$ for a in \a par, looking for a solution with x in \a ix. */
    virtual IntervalScalarFunctionModel implicit(const IntervalScalarFunction& f, const Vector<Interval>& par, const Interval& ix) const;
};

/*! \ingroup Solvers
 *  \brief Krawczyk solver. Uses the contractor \f$[x']=x_0-Mf(x_0)+(I-MDf([x])([x]-x_0)\f$
 *  where \f$M\f$ is typically \f$Df^{-1}(x_0)\f$.
 */
class KrawczykSolver
    : public SolverBase
{
  public:
    /*! \brief Constructor. */
    KrawczykSolver(double max_error, uint max_steps) : SolverBase(max_error,max_steps) { }
    KrawczykSolver(MaximumError max_error, MaximumNumberOfSteps max_steps) : SolverBase(max_error,max_steps) { }
    /*! \brief Cloning operator. */
    virtual KrawczykSolver* clone() const { return new KrawczykSolver(*this); }
    /*! \brief Write to an output stream. */
    virtual void write(std::ostream& os) const;

    /*! \brief Solve \f$f(a,x)=0\f$ for a in \a par, looking for solutions with x in \a ix. */
    virtual IntervalVectorFunctionModel implicit_step(const IntervalVectorFunction& f, const IntervalVectorFunctionModel& p, const IntervalVectorFunctionModel& x) const;

  public:
    /*! \brief A single step of the Krawczyk contractor. */
    virtual Vector<Interval>
    step(const IntervalVectorFunction& f,
          const Vector<Interval>& pt) const;
};


/*! \ingroup Solvers
 *  \brief Modified Krawczyk solver. Uses the contractor \f$[x']=x_0-J^{-1}(f(x_0)+(J-Df([x])([x]-x_0)\f$
 *  where \f$J\f$ is typically \f$Df(x_0)\f$.
 */
class FactoredKrawczykSolver
    : public KrawczykSolver
{
  public:
    /*! \brief Constructor. */
    FactoredKrawczykSolver(double max_error, uint max_steps) : KrawczykSolver(max_error,max_steps) { }
    FactoredKrawczykSolver(MaximumError max_error, MaximumNumberOfSteps max_steps) : KrawczykSolver(max_error,max_steps) { }
    /*! \brief Cloning operator. */
    virtual FactoredKrawczykSolver* clone() const { return new FactoredKrawczykSolver(*this); }
    /*! \brief Write to an output stream. */
    virtual void write(std::ostream& os) const;
  public:
    /*! \brief A single step of the modified Krawczyk contractor. */
    virtual Vector<Interval>
    step(const IntervalVectorFunction& f,
          const Vector<Interval>& pt) const;
};



} // namespace Ariadne

#endif /* ARIADNE_SOLVER_H */
