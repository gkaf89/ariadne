/***************************************************************************
 *            hybrid_system.h
 *
 *  Copyright  2009  Pieter Collins
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

/*! \file hybrid_system.h
 *  \brief Main compositional hybrid system class.
 */

#ifndef ARIADNE_HYBRID_SYSTEM_H
#define ARIADNE_HYBRID_SYSTEM_H

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include "formula.h"

namespace Ariadne {


class DiscreteState;
class DiscreteEvent;

class HybridTime;
class HybridSpace;
class HybridSet;
class HybridGrid;

class DiscreteMode;
class DiscreteTransition;
class HybridAutomaton;

class ExpressionInterface;
class FunctionInterface;
class FormulaInterface;
class Grid;

/*! \brief A discrete mode of a hybrid automaton, comprising continuous evolution given by a vector field
 * within and invariant constraint set.
 *
 * A %DiscreteMode can only be created using the new_mode() method in
 * the %HybridAutomaton class.
 *
 * \sa \link Ariadne::HybridAutomaton \c HybridAutomaton \endlink, \link Ariadne::DiscreteTransition \c DiscreteTransition \endlink
 */
class DiscreteMode {
    friend class HybridAutomaton;
    typedef boost::shared_ptr<const FunctionInterface> FunctionPtr;
    typedef boost::shared_ptr<const FunctionInterface> ExpressionPtr;
  private:

    // The discrete mode's discrete state.
    DiscreteState _location;

    // The discrete mode's vector field.
    FunctionPtr _dynamic;
    // The discrete mode's invariants.
    std::map< DiscreteEvent, FunctionPtr > _invariants;

    // The discrete mode's grid for reachability analysis.
    boost::shared_ptr< const Grid > _grid;
  public:
    //! \brief The mode's discrete state.
    DiscreteState location() const {
        return this->_location; }

    //! \brief The discrete mode's dynamic (a vector field).
    const FunctionInterface& dynamic() const {
        return *this->_dynamic; }

    //! \brief The discrete mode's dynamic (a vector field).
    FunctionPtr dynamic_ptr() const {
        return this->_dynamic; }

    //! \brief The discrete mode's invariants.
    const std::map< DiscreteEvent, FunctionPtr >& invariants() const {
        return this->_invariants; }

    //! \brief The discrete mode's default spacial grid.
    const Grid& grid() const {
        return *this->_grid; }

    //! \brief The dimension of the discrete mode.
    uint dimension() const;

    //! \brief Write to an output stream.
    std::ostream& write(std::ostream& os) const;

  private:
    // Construct discrete mode.
    //
    // \param id is the identifier of the mode.
    // \param dynamic is the mode's vector field.
    // \param invariants is the mode's invariants.
    DiscreteMode(DiscreteState location,
                 const FunctionInterface& dynamic);

    // Construct from objects managed by shared pointers (for internal use)
    DiscreteMode(DiscreteState location,
                 const FunctionPtr dynamic,
                 const std::vector< FunctionPtr >& invariants);

};


std::ostream& operator<<(std::ostream& os, const DiscreteMode& dm);

inline bool operator<(const DiscreteMode& mode1, const DiscreteMode& mode2) {
    return mode1.location() < mode2.location(); }




/*! \brief A discrete transition of a hybrid automaton, representing an instantaneous
 * jump from one discrete mode to another, governed by an activation set and a reset map.
 *
 * A %DiscreteTransition can only be created using the new_transition() method in
 * the %HybridAutomaton class.
 *
 * An invariant is modelled by a discrete transition with negative event id and null reset pointer.
 *
 * \sa \link Ariadne::HybridAutomaton \c HybridAutomaton \endlink, \link Ariadne::DiscreteMode \c DiscreteMode \endlink
 */
class DiscreteTransition
{
    friend class HybridAutomaton;
  private:
    // \brief The discrete transition's identificator.
    DiscreteEvent _event;

    // \brief The source of the discrete transition.
    const DiscreteMode* _source;

    // \brief The target of the discrete transition.
    const DiscreteMode* _target;

    // \brief The activation region of the discrete transition.
    boost::shared_ptr< const FunctionInterface > _activation;

    // \brief The reset of the discrete transition.
    boost::shared_ptr< const FunctionInterface > _reset;

    // \brief Whether or not the transition is forced.
    bool _forced;

  public:

    //! \brief The discrete event associated with the discrete transition.
    DiscreteEvent event() const {
        return this->_event; }

    //! \brief The source mode of the discrete transition.
    const DiscreteMode& source() const {
        return *this->_source; }

    //! \brief The target of the discrete transition.
    const DiscreteMode& target() const {
        return *this->_target; }


    //! \brief The activation region of the discrete transition.
    boost::shared_ptr<const FunctionInterface> activation_ptr() const {
        return this->_activation;
    }

    //! \brief The activation region of the discrete transition.
    const FunctionInterface& activation() const {
        return *this->_activation;
    }

    //! \brief The reset map of the discrete transition.
    const FunctionInterface& reset() const {
        return *this->_reset;
    }

    //! \brief The reset map of the discrete transition.
    boost::shared_ptr<const FunctionInterface> reset_ptr() const {
        return this->_reset;
    }

    //! \brief True if the transition is forced (occurs as soon as it is activated).
    bool forced() const {
        return this->_forced;
    }

  private:


    // Construct from shared pointers (for internal use).
    DiscreteTransition(DiscreteEvent event,
                       const DiscreteMode& source,
                       const DiscreteMode& target,
                       const FunctionInterface& reset,
                       const FunctionInterface& activation,
                       bool forced=false);

    // Construct from shared pointers (for internal use). */
    DiscreteTransition(DiscreteEvent event,
                       const DiscreteMode& source,
                       const DiscreteMode& target,
                       const boost::shared_ptr< FunctionInterface > reset,
                       const boost::shared_ptr< FunctionInterface > activation,
                       bool forced=false);
};

std::ostream& operator<<(std::ostream& os, const DiscreteTransition& dt);

inline bool operator<(const DiscreteTransition& transition1, const DiscreteTransition& transition2) {
    return transition1.event() < transition2.event()
        || (transition1.event() == transition2.event()
            && transition1.source().location() < transition2.source().location());
}





/*! \brief A hybrid system, comprising continuous-time behaviour
 *  at each discrete mode, coupled by instantaneous discrete transitions.
 *  The state space is given by a hybrid set.
 * \sa \link Ariadne::HybridAutomaton \c HybridAutomaton \endlink.

 */
class HybridSystem
{
  public:
    //! \brief The type used to represent time.
    typedef HybridTime TimeType;
    //! \brief The type used to represent real numbers.
    typedef double RealType ;
    //! \brief The type used to describe the state space.
    typedef HybridSpace StateSpaceType;

    typedef boost::shared_ptr<const ExpressionInterface> ExpressionPtr;
    typedef boost::shared_ptr<const FunctionInterface> FunctionPtr;
    typedef boost::shared_ptr<const FormulaInterface> FormulaPtr;


/*
    typedef std::map< DiscreteEvent, boost::shared_ptr<const FunctionInterface> >::const_iterator invariant_const_iterator;
    typedef std::set< DiscreteTransition >::const_iterator discrete_transition_const_iterator;
    typedef std::set< DiscreteMode >::const_iterator discrete_mode_const_iterator;
*/
  private:

    //! \brief The list of the hybrid automaton's discrete modes.
    //std::set< DiscreteMode > _modes;

    struct DifferentialEquation { DiscretePredicate loc; Variable lhs; Formula rhs; };
    struct AlgebraicEquation { DiscretePredicate loc; Variable lhs; Formula rhs; };

    std::vector<DifferentialEquation> _differential_equations;
    std::vector<AlgebraicEquation> _algebraic_equations;

  public:
    //@{
    //! \name Constructors and destructors

    //! \brief Construct an empty automaton with no name
    HybridSystem();

    //! \brief Construct dynamically-allocated copy. (Not currently implemented)
    HybridSystem* clone() const;

    //! \brief  Destructor.
    ~HybridSystem();
    //@}

    //@{
    //! \name Methods for building the automaton.

    void new_invariant(DiscretePredicate q, Variable d, Formula f) {
        AlgebraicEquation e={q,d,f}; _algebraic_equations.push_back(e); }

    void new_dynamic(DiscretePredicate q, DottedVariable d, Formula f) {
        DifferentialEquation e={q,d.base(),f}; _differential_equations.push_back(e); };
    void new_reset(DiscreteEvent e, DiscretePredicate q, DottedVariable d, Formula f);
    void new_guard(DiscreteEvent e, DiscretePredicate q, Predicate f);

    Space state_variables(const DiscreteState& state);
    Space algebraic_variables(const DiscreteState& state);
    Space input_variables(const DiscreteState& state);

    //! \brief Adds a discrete mode to the automaton.
    //!
    //!   \param state is the mode's discrete state.
    //!   \param dynamic is the mode's vector field.
    const DiscreteMode& new_mode(DiscreteState state,
                                 const FunctionInterface& dynamic);


    //! \brief Set the grid controlling relative scaling in the mode.
    void set_grid(DiscreteState location, const Grid& grid);

    //! \brief Set the grid controlling relative scaling. This method sets the same grid for every mode.
    void set_grid(const Grid& grid);

    //! \brief Set the hybrid grid controlling relative scaling.
    void set_grid(const HybridGrid& hgrid);

    //@}

    //@{
    //! \name Data access and queries.

    //! \brief Returns the hybrid automaton's name.
    const std::string& name() const;

    //! \brief Test if the hybrid automaton has a discrete mode with discrete state \a state.
    bool has_mode(DiscreteState state) const;

    //! \brief Test if the hybrid automaton has a discrete transition with \a event_id and \a source_id.
    bool has_transition(DiscreteEvent event, DiscreteState source) const;

    //! \brief The discrete mode with given discrete state.
    const DiscreteMode& mode(DiscreteState state) const;

    //! \brief The discrete transition with given \a event and \a source location.
    const DiscreteTransition& transition(DiscreteEvent event, DiscreteState source) const;

    //! \brief The set of discrete modes. (Not available in Python interface)
    const std::set< DiscreteMode >& modes() const;

    //! \brief The set of discrete transitions. (Not available in Python interface)
    const std::set< DiscreteTransition >& transitions() const;

    //! \brief The discrete transitions from location \a source.
    std::set< DiscreteTransition > transitions(DiscreteState source) const;

    //! \brief The blocking events (invariants and urgent transitions) in \a location.
    std::map<DiscreteEvent,FunctionPtr> blocking_guards(DiscreteState location) const;

    //! \brief The permissive events (invariants and urgent transitions) in \a location.
    std::map<DiscreteEvent,FunctionPtr> permissive_guards(DiscreteState location) const;

    //! \brief The state space of the system.
    HybridSpace state_space() const;

    //! \brief The hybrid set giving the invariants for each discrete location.
    HybridSet invariant() const;

    //! \brief The natural grid to use in the specified location.
    Grid grid(DiscreteState location) const;

    //! \brief The natural grid to use in the over all locations.
    HybridGrid grid() const;

    //@}

    //@{
    //! \name Operations on systems.

    //! \brief The parallel composition of two systems.
    HybridSystem parallel_composition(const HybridSystem&, const HybridSystem&);

    //@}
};

std::ostream& operator<<(std::ostream& os, const HybridAutomaton& ha);


} // namespace Ariadne

#endif // ARIADNE_HYBRID_SYSTEM_H
