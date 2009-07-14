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


class Event;
class EventSet;

class HybridTime;
class HybridSpace;
class HybridSet;
class HybridGrid;

class DiscreteState;
class DiscreteSpace;

class ExpressionInterface;
class FunctionInterface;

template<class R> class FormulaInterface;
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
    typedef int DiscreteState;
    typedef boost::shared_ptr<const FunctionInterface> FunctionPtr;
    typedef boost::shared_ptr<const FunctionInterface> ExpressionPtr;
  private:

    // The discrete mode's discrete state.
    DiscreteState _location;

    // The discrete mode's vector field.
    FunctionPtr _dynamic;
    // The discrete mode's invariants.
    std::map< Event, FunctionPtr > _invariants;

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
    const std::map< Event, FunctionPtr >& invariants() const {
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
    Event _event;

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
    Event event() const {
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
    DiscreteTransition(Event event,
                       const DiscreteMode& source,
                       const DiscreteMode& target,
                       const FunctionInterface& reset,
                       const FunctionInterface& activation,
                       bool forced=false);

    // Construct from shared pointers (for internal use). */
    DiscreteTransition(Event event,
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

    
/*
    typedef std::map< Event, boost::shared_ptr<const FunctionInterface> >::const_iterator invariant_const_iterator;
    typedef std::set< DiscreteTransition >::const_iterator discrete_transition_const_iterator;
    typedef std::set< DiscreteMode >::const_iterator discrete_mode_const_iterator;
*/
  private:
  public:


    //! \brief The list of the hybrid automaton's discrete modes.
    //std::set< DiscreteMode > _modes;

    //struct DiscreteEquation { DiscretePredicate loc; DiscreteVariable lhs; DiscreteFormula rhs; };
    struct DifferentialEquation { DiscretePredicate loc; RealVariable lhs; RealFormula rhs; };
    struct AlgebraicEquation { DiscretePredicate loc; RealVariable lhs; RealFormula rhs; };
    struct DiscreteAssignment { EventSet evnts; DiscretePredicate loc; EnumeratedVariable lhs; EnumeratedFormula rhs;  };
    struct UpdateEquation { EventSet evnts; DiscretePredicate loc; RealVariable lhs; RealFormula rhs;  };
    struct GuardPredicate { EventSet evnts; DiscretePredicate loc; ContinuousPredicate pred; };
    struct DisabledEvents { EventSet evnts; DiscretePredicate loc; };
    struct InvariantPredicate { DiscretePredicate loc; ContinuousPredicate pred; };

    std::vector<DifferentialEquation> _differential_equations;
    std::vector<AlgebraicEquation> _algebraic_equations;
    std::vector<DiscreteAssignment> _discrete_assignments;
    std::vector<UpdateEquation> _update_equations;
    std::vector<GuardPredicate> _guard_predicates;
    std::vector<InvariantPredicate> _invariant_predicates;
    std::vector<DisabledEvents> _disabled_events;

    typedef std::vector<DifferentialEquation>::const_iterator dynamic_const_iterator;
    typedef std::vector<AlgebraicEquation>::const_iterator relation_const_iterator;
    typedef std::vector<UpdateEquation>::const_iterator update_const_iterator;
    typedef std::vector<GuardPredicate>::const_iterator guard_const_iterator;
    typedef std::vector<DiscreteAssignment>::const_iterator switch_const_iterator;
    typedef std::vector<DisabledEvents>::const_iterator disabled_const_iterator;
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

    // Methods for rules valid in certain modes
    //! \brief Adds a algebraic equation to the system.
    void new_equation(DiscretePredicate q, RealAssignment a) {
        AlgebraicEquation eqn={q,a.lhs,a.rhs}; _algebraic_equations.push_back(eqn); };
    //! \brief Adds a differential equation to the system.
    void new_dynamic(DiscretePredicate q, RealDynamic d) {
        DifferentialEquation eqn={q,d.lhs.base,d.rhs}; _differential_equations.push_back(eqn); };
    //! \brief Adds a discrete reset to the system.
    void new_reset(EventSet e, DiscretePredicate q, EnumeratedUpdate a) {
        DiscreteAssignment eqn={e,q,a.lhs.base,a.rhs}; _discrete_assignments.push_back(eqn); }
    //! \brief Adds a reset equation to the system.
    void new_reset(EventSet e, DiscretePredicate q, RealUpdate a) {
        UpdateEquation eqn={e,q,a.lhs.base,a.rhs}; _update_equations.push_back(eqn); }
    //! \brief Adds a guard predicate to the system.
    void new_guard(EventSet e, DiscretePredicate q, ContinuousPredicate p) {
        GuardPredicate eqn={e,q,p}; _guard_predicates.push_back(eqn); }
    //! \brief Adds a guard predicate to the system.
    void new_guard(EventSet e, DiscretePredicate q, bool p) {
        GuardPredicate eqn={e,q,ContinuousPredicate(tribool(p))}; _guard_predicates.push_back(eqn); }
    //! \brief Adds an invariant to the system.
    void new_invariant(DiscretePredicate q, ContinuousPredicate p) {
        InvariantPredicate eqn={q,p}; _invariant_predicates.push_back(eqn); }
    // //! \brief Disables events in a given set of locations.
    //void new_disabled_events(EventSet e, DiscretePredicate q) {
    //    DisabledEvents dis={e,q}; _disabled_events.push_back(dis); }

    // Methods for rules valid in all modes.
    //! \brief Adds a algebraic equation to the system, valid in all modes.
    void new_equation(RealAssignment a) { this->new_equation(DiscretePredicate(true),a); }
    //! \brief Adds a differential equation to the system.
    void new_dynamic(RealDynamic d) { this->new_dynamic(DiscretePredicate(true),d); }
    //! \brief Adds a discrete reset to the system, valid in all modes.
    void new_reset(EventSet e, EnumeratedUpdate du) { this->new_reset(e,DiscretePredicate(true),du); }
    //! \brief Adds a reset equation to the system, valid in all modes.
    void new_reset(EventSet e, RealUpdate u) { this->new_reset(e,DiscretePredicate(true),u); }
    //! \brief Adds a guard predicate to the system, valid in all modes.
    void new_guard(EventSet e, ContinuousPredicate p) { this->new_guard(e,DiscretePredicate(true),p); }
    //! \brief Adds a guard predicate to the system, valid in all modes.
    void new_guard(EventSet e, bool p) { this->new_guard(e,DiscretePredicate(true),ContinuousPredicate(tribool(p))); }
    //! \brief Adds an invariant to the system, valid in all modes.
    void new_invariant(ContinuousPredicate p) { this->new_invariant(DiscretePredicate(true),p); }

    // Methods for rules valid for all events.
    //! \brief Adds a discrete reset to the system, valid in all modes and for all events.
    void new_reset(EnumeratedUpdate du) { this->new_reset(EventSet::all(),DiscretePredicate(true),du); }
    //! \brief Adds a reset equation to the system, valid in all modes and for all events.
    void new_reset(RealUpdate u) { this->new_reset(EventSet::all(),DiscretePredicate(true),u); }

    //@}

    //@{
    //! \name Data access and queries.

    StateSpace discrete_variables() const;
    EventSet events() const;
    VariableSet result_variables(const Valuation& state) const;
    VariableSet argument_variables(const Valuation& state) const;
    VariableSet continuous_variables(const Valuation& state) const;
    VariableSet state_variables(const Valuation& state) const;
    VariableSet algebraic_variables(const Valuation& state) const;
    VariableSet auxiliary_variables(const Valuation& state) const;
    VariableSet input_variables(const Valuation& state) const;
    VariableSet output_variables(const Valuation& state) const;

    bool check_dynamic(const Valuation& location) const;
    bool check_reset(const Event& event, const Valuation& source, const Valuation& target) const;
    bool check_guards(const Valuation& location) const;


    Valuation target(const Event& event, const Valuation& source) const;
    std::set<RealAssignment> unordered_equations(const Valuation& state) const;

    std::vector<RealAssignment> equations(const Valuation& state) const;
    std::vector<RealDynamic> dynamic(const Valuation& state) const;
    std::vector<RealUpdate> reset(const Event& event, const Valuation& state) const;
    std::map<Event,ContinuousPredicate> guards(const Valuation& state) const;
    ContinuousPredicate guard(const Event& event, const Valuation& state) const;

    //@}

    //@{
    //! \name Old-style data access and queries.

    //! \brief Test if the hybrid automaton has a discrete mode with discrete state \a state.
    bool has_mode(DiscreteState state) const;

    //! \brief Test if the hybrid automaton has a discrete transition with \a event_id and \a source_id.
    bool has_transition(Event event, DiscreteState source) const;

    //! \brief The discrete mode with given discrete state.
    const DiscreteMode& mode(DiscreteState state) const;

    //! \brief The discrete transition with given \a event and \a source location.
    const DiscreteTransition& transition(Event event, DiscreteState source) const;

    //! \brief The set of discrete modes. (Not available in Python interface)
    const std::set< DiscreteMode >& modes() const;

    //! \brief The set of discrete transitions. (Not available in Python interface)
    const std::set< DiscreteTransition >& transitions() const;

    //! \brief The discrete transitions from location \a source.
    std::set< DiscreteTransition > transitions(DiscreteState source) const;

    //! \brief The blocking events (invariants and urgent transitions) in \a location.
    std::map<Event,FunctionPtr> blocking_guards(DiscreteState location) const;

    //! \brief The permissive events (invariants and urgent transitions) in \a location.
    std::map<Event,FunctionPtr> permissive_guards(DiscreteState location) const;

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

std::ostream& operator<<(std::ostream& os, const HybridSystem& hs);
std::ostream& operator<<(std::ostream& os, const HybridSystem::AlgebraicEquation& ae);
std::ostream& operator<<(std::ostream& os, const HybridSystem::DifferentialEquation& de);
std::ostream& operator<<(std::ostream& os, const HybridSystem::DiscreteAssignment& da);
std::ostream& operator<<(std::ostream& os, const HybridSystem::UpdateEquation& re);
std::ostream& operator<<(std::ostream& os, const HybridSystem::GuardPredicate& g);
std::ostream& operator<<(std::ostream& os, const HybridSystem::InvariantPredicate& inv);




} // namespace Ariadne

#endif // ARIADNE_HYBRID_SYSTEM_H
