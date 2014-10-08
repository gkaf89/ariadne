/***************************************************************************
 *            nonlinear_programming.h
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

/*! \file nonlinear_programming.h
 *  \brief Nonlinear programming.
 */

#ifndef ARIADNE_NONLINEAR_PROGRAMMING_H
#define ARIADNE_NONLINEAR_PROGRAMMING_H

#include "utility/declarations.h"

#include "utility/logging.h"
#include "numeric/numeric.h"
#include "utility/tuple.h"


namespace Ariadne {

template<class X, class R> class Constraint;
typedef Constraint<EffectiveScalarFunction,EffectiveNumber> EffectiveConstraint;
typedef Constraint<ValidatedScalarFunction,ValidatedNumber> ValidatedConstraint;

class InfeasibleProblemException : public std::runtime_error {
  public: InfeasibleProblemException() : std::runtime_error("InfeasibleProblemException") { }
};
class IndeterminateFeasibilityException : public std::runtime_error {
  public: IndeterminateFeasibilityException() : std::runtime_error("IndeterminateFeasibilityException") { }
};
class DegenerateNonlinearFeasibilityProblemException : public std::runtime_error {
  public: DegenerateNonlinearFeasibilityProblemException() : std::runtime_error("DegenerateNonlinearFeasibilityProblemException") { }
};
class NearBoundaryOfFeasibleDomainException : public std::runtime_error {
  public: NearBoundaryOfFeasibleDomainException() : std::runtime_error("NearBoundaryOfFeasibleDomainException") { }
};

//! \ingroup OptimisationModule EvaluationModule
//! Interface for nonlinear programming solvers.
class OptimiserInterface {
  public:
    //! \brief Virtual destructor.
    virtual ~OptimiserInterface() { }
    //! \brief Create a dynamically-allocated copy.
    virtual OptimiserInterface* clone() const = 0;

    //! \brief Solve the general nonlinear programming problem \f$\min f(x) \text{ such that } x\in D \text{ and } g(x)\in C\f$.
    virtual Vector<ValidatedNumber> minimise(ValidatedScalarFunction f, ExactBox D, ValidatedVectorFunction g, ExactBox C) const = 0;
    //! \brief Solve the standard nonlinear programming problem \f$\min f(x) \text{ such that } x\in ,D\ g(x)\leq 0  \text{ and } h(x) = 0\f$.
    virtual Vector<ValidatedNumber> minimise(ValidatedScalarFunction f, ExactBox D, ValidatedVectorFunction g, ValidatedVectorFunction h) const = 0;

    //! \brief Tests is the general nonlinear feasibility problem \f$x\in D \text{ and } g(x)\in C\f$ is feasible.
    virtual Tribool feasible(ExactBox D, ValidatedVectorFunction g, ExactBox C) const = 0;
    //! \brief Tests is the standard nonlinear feasibility problem \f$x\in D,\ g(x)\leq 0 \text{ and } h(x) = 0\f$ is feasible. Assumes \fD\f$ is bounded with nonempty interior.
    //! \internal This is one of the simplest nonlinear programming problems, and is a good test case for new algorithms.
    virtual Tribool feasible(ExactBox D, ValidatedVectorFunction g, ValidatedVectorFunction h) const = 0;

    //! \brief Tests if the point \a x is feasible, in that \f$x\in D\f$ and \f$g(x)\in N_\epsilon(C)\f$.
    virtual Bool almost_feasible_point(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                       ApproximateFloatVector x, ApproximateFloat eps) const = 0;
    //! \brief Tests if the point \a x is feasible.
    virtual Bool is_feasible_point(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                   ExactFloatVector x) const = 0;
    //! \brief Tests if the point \a x is near feasible.
    virtual Bool validate_feasibility(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                      ExactVector x) const = 0;
    //! \brief Tests if the point \a x is near feasible, using approximate multipliers \a y to guide the search.
    virtual Bool validate_feasibility(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                      ExactVector x, ExactVector y) const = 0;
    //! \brief Tests if the feasibility problem is definitely unsolvable, using multipliers \a y and local centering point \a x.
    virtual Bool validate_infeasibility(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                      ExactVector x, ExactVector y) const = 0;
    //! \brief Tests if the box \a X definitely containss a feasible point.
    virtual Tribool contains_feasible_point(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                            ValidatedFloatVector X) const = 0;
    //! \brief Tests if the Lagrange multipliers \a y are a certificate of infeasiblity.
    virtual Bool is_infeasibility_certificate(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                              ExactVector y) const = 0;
};

//! \ingroup OptimisationModule
//! Common routines for nonlinear minimisation
class OptimiserBase
    : public OptimiserInterface
    , public Loggable
{
  public:
    virtual Vector<ValidatedNumber> minimise(ValidatedScalarFunction f, ExactBox D, ValidatedVectorFunction g, ExactBox C) const = 0;
    virtual Vector<ValidatedNumber> minimise(ValidatedScalarFunction f, ExactBox D, ValidatedVectorFunction g, ValidatedVectorFunction h) const;

    virtual Tribool feasible(ExactBox D, ValidatedVectorFunction g, ExactBox C) const = 0;
    virtual Tribool feasible(ExactBox D, ValidatedVectorFunction g, ValidatedVectorFunction h) const;

    virtual Bool almost_feasible_point(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                       ApproximateVector x, ApproximateFloat error) const;
    virtual Bool is_feasible_point(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                   ExactVector x) const;
    virtual Bool validate_feasibility(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                      ExactVector x) const;
    virtual Bool validate_feasibility(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                      ExactVector x, ExactVector y) const;
    virtual Bool validate_infeasibility(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                        ExactVector x, ExactVector y) const;
    virtual Tribool contains_feasible_point(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                            ValidatedVector X) const;
    virtual Bool is_infeasibility_certificate(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                              ExactVector lambda) const;
};

//! \ingroup OptimisationModule
//! \brief Solver for feasibility problems based on a penalty-function approach
//!
//! For the feasibility problem \f$x\in D,\ g(x)\in C,\ h(x)=0\f$ where \f$D\f$ is bounded and \f$D,C\f$ have nonempty interiors.
//! Introduce slack variable \f$w=g(x)\f$, and minimise \f[ \sum_{j} (g_j(x)-w_j)^2 + \sum_k h_k(x)^2 \f] with \f$x\in D\f$ and \f$w\in C\f$.
//! Since the minimiser is not unique, we need to add penalty terms \f$-\mu/2(\log(x_u-x)+log(x-x_l))\f$ and \f$-\nu/2(\log(w_u-w)+log(w-w_l))\f$.
//! It suffices to add a penalty in \f$x\f$.
class PenaltyFunctionOptimiser
    : public OptimiserBase
{
  public:
    virtual PenaltyFunctionOptimiser* clone() const;
    virtual Tribool check_feasibility(ExactBox D, ValidatedVectorFunction g, ExactBox C, ExactVector x, ExactVector y) const;
    virtual Vector<ValidatedNumber> minimise(ValidatedScalarFunction f, ExactBox D, ValidatedVectorFunction g, ExactBox C) const;
    virtual Tribool feasible(ExactBox D, ValidatedVectorFunction g, ExactBox C) const;
    virtual Void feasibility_step(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                                  ApproximateFloatVector& x, ApproximateFloatVector& w, ApproximateFloat& mu) const;
    virtual Void feasibility_step(const ExactBox& D, const ValidatedVectorFunction& g, const ExactBox& C,
                                  ValidatedFloatVector& x, ValidatedFloatVector& w) const;
    virtual Void feasibility_step(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                                  ApproximateFloatVector& x, ApproximateFloatVector& y, ApproximateFloatVector& z) const;
};




//! \ingroup OptimisationModule
//! \brief Solver for linear programming problems using invalid interior point methods.
//! \details Introduces variables \f$w\f$ and attempts to find \f$x\in D\f$ and \f$w\in C\f$ such that \f$g(x)=w\f$.
//!   The dual variables \f$y\f$ are unconstrained Lagrange multipliers for \f$y\cdot(g(x)-w)=0\f$.
class NonlinearInfeasibleInteriorPointOptimiser
    : public OptimiserBase
{
  public:
    virtual NonlinearInfeasibleInteriorPointOptimiser* clone() const { return new NonlinearInfeasibleInteriorPointOptimiser(*this); }

    using OptimiserBase::minimise;
    using OptimiserBase::feasible;

    struct PrimalDualData;
    struct StepData;

    //! \brief Compute a \em local optimum of linear programming problem \f$\max f(x) \text{ such that } x\in D, g(x)\in C \text{ and } h(x)=0.\f$.
    //! \precondition The domain \f$D\f$ is bounded and has nonempty interior, and the codomain \f$C\f$ is nonempty.
    //! \return A box \f$X\f$ which definitely contains a feasible point, and contains a local optimum.
    virtual Vector<ValidatedNumber> minimise(ValidatedScalarFunction f, ExactBox D, ValidatedVectorFunction g, ExactBox C) const;
    //! \brief Tests is the nonlinear programming problem \f$x\in D \text{ and } g(x)\in C\f$ is feasible.
    virtual Tribool feasible(ExactBox D, ValidatedVectorFunction g, ExactBox C) const;

    //! \brief Test if the constraints \f$g(x)\in C\f$ are solvable for \f$x\in D\f$ using a nonlinear feasibility test,
    //! hotstarting the method with the overall primal and dual variables.
    Pair<Tribool,ApproximateFloatVector> feasible_hotstarted(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                                                const PrimalDualData& wxy0) const;

    Void setup_feasibility(const ExactBox& D, const ApproximateVectorFunctionInterface& g, const ExactBox& C,
                           StepData& stp) const;
    Void step(const ApproximateScalarFunctionInterface& f, const ExactBox& D, const ApproximateVectorFunctionInterface& g, const ExactBox& C,
              StepData& stp) const;
//    ApproximateFloat compute_mu(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
//                     ApproximateFloatVector& w, ApproximateFloatVector& x, ApproximateFloatVector& y) const;
};


//! \ingroup OptimisationModule
//! Solver for linear programming problems using interior point methods.
class NonlinearInteriorPointOptimiser
    : public OptimiserBase
{
  public:
    virtual NonlinearInteriorPointOptimiser* clone() const { return new NonlinearInteriorPointOptimiser(*this); }

    using OptimiserBase::minimise;
    using OptimiserBase::feasible;

    //! \brief Compute a \em local optimum of linear programming problem \f$\max f(x) \text{ such that } x\in D, g(x)\in C \text{ and } h(x)=0.\f$.
    //! \precondition The domain \f$D\f$ is bounded and has nonempty interior, and the codomain \f$C\f$ is nonempty.
    //! \return A box \f$X\f$ which definitely contains a feasible point, and contains a local optimum.
    virtual Vector<ValidatedNumber> minimise(ValidatedScalarFunction f, ExactBox D, ValidatedVectorFunction g, ExactBox C) const;
    //! \brief Tests is the nonlinear programming problem \f$x\in D, g(x)\in C \text{ and } h(x)= 0 \f$ is feasible.
    virtual Tribool feasible(ExactBox D, ValidatedVectorFunction g, ExactBox C) const;

    //! \brief Test if the constraints \f$g(y)\in C\f$ are solvable for \f$y\in D\f$ using a nonlinear feasibility test,
    //! hotstarting the method with the overall constraint violation, primal and dual variables.
    Pair<Tribool,ApproximateFloatVector> feasible_hotstarted(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                                  const ApproximateFloatVector& x0, const ApproximateFloatVector& lambda0, const ApproximateFloat& violation0) const;

    //! \brief Test if the constraints \f$g(y)\in C\f$ are solvable for \f$y\in D\f$ using a nonlinear feasibility test,
    //! hotstarting the method with the primal and dual.
    Pair<Tribool,ApproximateFloatVector> feasible_hotstarted(ExactBox D, ValidatedVectorFunction g, ExactBox C,
                                                  const ApproximateFloatVector& x0, const ApproximateFloatVector& lambda0) const;

    Void minimisation_step(const ApproximateScalarFunction& f, const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C, const ApproximateVectorFunction& h,
                           ApproximateFloatVector& x, ApproximateFloatVector& w, ApproximateFloatVector& kappa, ApproximateFloatVector& lambda, const ApproximateFloat& mu) const;
    Void setup_feasibility(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                           ApproximateFloatVector& x, ApproximateFloatVector& lambda) const;
    Void setup_feasibility(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                           ApproximateFloatVector& x, ApproximateFloatVector& lambda, ApproximateFloat& t) const;
    Void feasibility_step(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                          ApproximateFloatVector& x, ApproximateFloatVector& lambda) const;
    Void feasibility_step(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                          ApproximateFloatVector& x, ApproximateFloatVector& lambda, ApproximateFloat& violation) const;
    Void initialise_lagrange_multipliers(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                                         const ApproximateFloatVector& x, ApproximateFloatVector& lambda) const;
    ApproximateFloat compute_mu(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                     const ApproximateFloatVector& x, const ApproximateFloatVector& lambda) const;

  public: // Deprecated
    Void compute_tz(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                          ApproximateFloatVector& x, ApproximateFloat& t, ApproximateFloatVector& z) const { }
    Void feasibility_step(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                          ApproximateFloatVector& x, ApproximateFloatVector& y, ApproximateFloatVector& z, ApproximateFloat& violation) const { };
    Void linearised_feasibility_step(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                                     ApproximateFloat& slack, ApproximateFloatVector& x, ApproximateFloatVector& lambda) const { };
    Void linearised_feasibility_step(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                                     ApproximateFloatVector& x, ApproximateFloatVector& y, ApproximateFloatVector& z, ApproximateFloat& t) const { };
  private:
    ApproximateFloat compute_mu(const ApproximateScalarFunction& f, const ExactBox& D, const ValidatedVectorFunction& g, const ExactBox& C,
                     const ApproximateFloatVector& x, const ApproximateFloatVector& y) const;
    Void compute_violation(const ExactBox& D, const ApproximateVectorFunction& g, const ExactBox& C,
                           ApproximateFloatVector& x, ApproximateFloat& t) const;
};




class IntervalOptimiser
    : public NonlinearInteriorPointOptimiser
{
    virtual IntervalOptimiser* clone() const { return new IntervalOptimiser(*this); }
    virtual Tribool feasible(ExactBox D, ValidatedVectorFunction h) const;
    Void feasibility_step(const ExactFloatVector& xl, const ExactFloatVector& xu, const ValidatedVectorFunction& h,
                          ValidatedFloatVector& x, ValidatedFloatVector& y, ValidatedFloatVector& zl, ValidatedFloatVector zu, ValidatedFloat& mu) const;
};


class ApproximateOptimiser
    : public NonlinearInteriorPointOptimiser
{
    virtual ApproximateOptimiser* clone() const { return new ApproximateOptimiser(*this); }
    virtual Tribool feasible(ExactBox D, ValidatedVectorFunction h) const;
    Void feasibility_step(const ExactBox& D, const ApproximateVectorFunction& h,
                          ApproximateFloatVector& X, ApproximateFloatVector& Lambda) const;
};


/*//! \ingroup OptimisationModule
//! Solver for linear programming problems using interior point methods.
//! WARNING: This class currently does not work; maybe there is a problem with the algorithms.
class KrawczykOptimiser
    : public OptimiserBase
{

  public:
    virtual KrawczykOptimiser* clone() const { return new KrawczykOptimiser(*this); }

    //! \brief Solve the linear programming problem \f$\max f(x) \text{ such that } x\in D \text{ and } g(x)\in C\f$.
    virtual Vector<ValidatedNumber> minimise(ValidatedScalarFunction f, ExactBox D, ValidatedVectorFunction g, ExactBox C) const;
    //! \brief Tests is the nonlinear programming problem \f$x\in D \text{ and } g(x)\in C\f$ is feasible.
    virtual tribool feasible(ExactBox D, ValidatedVectorFunction g, ExactBox C) const;

  public:
    //! \brief Try to solve the nonlinear constraint problem by applying the Krawczyk contractor to the Kuhn-Tucker conditions,
    //! hotstarting the iteration with the primal and dual variables.
    tribool minimise(ValidatedScalarFunction f, ExactBox D, ValidatedVectorFunction g, ExactBox C,
                     const ValidatedFloat& t0, const ValidatedFloatVector& x0, const ValidatedFloatVector& y0, const ValidatedFloatVector& z0) const;

    //! \brief A primal-dual feasibility step for the problem \f$g(y)\in C;\ y\in D\f$.
    void minimisation_step(const ExactBox& D, const ValidatedVectorFunction& g, const ExactBox& C,
                           ValidatedFloatVector& x, ValidatedFloatVector& y, ValidatedFloatVector& z, ValidatedFloat& t) const;
    //! \brief A primal-dual feasibility step for the problem \f$g(y)\in C;\ y\in D\f$.
    void feasibility_step(const ExactBox& D, const ValidatedVectorFunction& g, const ExactBox& C,
                          ValidatedFloatVector& x, ValidatedFloatVector& y, ValidatedFloatVector& z, ValidatedFloat& t) const;

    //! \brief A primal feasibility step for the problem \f$g(y)\in C;\ y\in D\f$. \deprecated
    void feasibility_step(const ExactBox& D, const ValidatedVectorFunction& g, const ExactBox& C,
                          ValidatedFloatVector& y, ValidatedFloat& t) const;
    //! \brief A feasibility step for the problem \f$g(y)\leq 0\f$. \deprecated
    void feasibility_step(const ValidatedVectorFunction& g,
                          ValidatedFloatVector& x, ValidatedFloatVector& y, ValidatedFloatVector& z, ValidatedFloat& t) const;
    //! \brief An optimization step for the problem \f$\max f(y) \text{ s.t. } g(y)\leq 0\f$. \deprecated
    void minimisation_step(const ValidatedScalarFunction& f, const ValidatedVectorFunction& g,
                           ValidatedFloatVector& x, ValidatedFloatVector& y, ValidatedFloatVector& z) const;
  protected:
    void setup_feasibility(const ExactBox& D, const ValidatedVectorFunction& g, const ExactBox& C,
                           ValidatedFloatVector& x, ValidatedFloatVector& y, ValidatedFloatVector& z, ValidatedFloat& t) const;
    protected:
    void compute_tz(const ExactBox& D, const ValidatedVectorFunction& g, const ExactBox& C, const ValidatedFloatVector& y, ValidatedFloat& t, ValidatedFloatVector& z) const;
};

*/


} // namespace Ariadne

#endif
