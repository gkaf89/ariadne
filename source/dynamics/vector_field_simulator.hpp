/***************************************************************************
 *            dynamics/vector_field_simulator.hpp
 *
 *  Copyright  2009-21  Luca Geretti, Mirko Albanese
 *
 ****************************************************************************/

/*
 *  This file is part of Ariadne.
 *
 *  Ariadne is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ariadne is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne.  If not, see <https://www.gnu.org/licenses/>.
 */

/*! \file dynamics/vector_field_simulator.hpp
 *  \brief Simulator for continuous systems.
 */

#ifndef ARIADNE_VECTOR_FIELD_SIMULATOR_HPP
#define ARIADNE_VECTOR_FIELD_SIMULATOR_HPP

#include "conclog/logging.hpp"
#include "numeric/float.decl.hpp"
#include "numeric/floatdp.hpp"
#include "geometry/point.hpp"
#include "solvers/configuration_interface.hpp"
#include "dynamics/vector_field.hpp"
#include "dynamics/orbit.hpp"

#include "betterthreads/workload.hpp"

using namespace ConcLog;
using namespace BetterThreads;

namespace Ariadne {

enum class DiscretisationType
{
  Mince,
  Recombine
};

OutputStream& operator<<(OutputStream& os, const DiscretisationType& dtype) {

    if(dtype == DiscretisationType::Mince){ os << "Mince"; }
    else{ os << "Recombine"; }
    
    return os;
}

class VectorField;
class VectorFieldSimulatorConfiguration;

class RealExpressionBoundedConstraintSet;

template<class T> class Orbit;

/*! \brief A class for computing the simulated evolution of a continuous system.
 */
class VectorFieldSimulator
{
  public:
    typedef LabelledPoint<FloatDPApproximation> ApproximatePointType;
    typedef Vector<ApproximatePointType> ApproximateListPointType;
    typedef LabelledPoint<Real> RealPointType;
    typedef Vector<RealPointType> RealListPointType;
    typedef RealVariablesBox RealBoxType;
    typedef Real TerminationType;
    typedef VectorField SystemType;
    typedef VectorFieldSimulatorConfiguration ConfigurationType;
    typedef Orbit<ApproximateListPointType> OrbitListType;

    //! \brief Synchronised wrapping of orbit to allow concurrent adjoining
    struct SynchronisedOrbit : public OrbitListType {
        SynchronisedOrbit(ApproximateListPointType const& initial_points) : OrbitListType(initial_points) { }
        void insert(FloatDP const& t, ApproximatePointType const& pt, SizeType const& curve_number) {
            LockGuard<Mutex> lock(_mux); OrbitListType::insert(t,pt,curve_number); }
      private:
        Mutex _mux;
    };
    typedef StaticWorkload<Pair<SizeType,ApproximatePointType> const&, TerminationType const&, SharedPointer<SynchronisedOrbit>> WorkloadType;
  public:

    //! \brief Default constructor.
    VectorFieldSimulator(SystemType const& system);

    //!@{
    //! \name Configuration for the class.
    //! \brief A reference to the configuration controlling the evolution.
    ConfigurationType& configuration() { return *this->_configuration; }
    ConfigurationType const& configuration() const { return *this->_configuration; }

    //!@{
    //! \name Simulation using points.
    //! \brief Compute an approximation to the orbit set.
    OrbitListType orbit(RealBoxType const& initial_box, TerminationType const& termination) const;
    OrbitListType orbit(RealExpressionBoundedConstraintSet const& initial_set, TerminationType const& termination) const;
    OrbitListType orbit(ApproximateListPointType const& initial_list, TerminationType const& termination) const;
    OrbitListType orbit(UpperBoxType& initial_box, TerminationType const& termination) const;

  private:

    void _simulate_from_point(Pair<SizeType,ApproximatePointType> const& indexed_initial, TerminationType const& termination, SharedPointer<SynchronisedOrbit> orbit) const;
  private:
    SharedPointer<SystemType> _system;
    SharedPointer<ConfigurationType> _configuration;
};

//! \brief Configuration for a VectorFieldSimulator, essentially to control accuracy of evolution.
class VectorFieldSimulatorConfiguration : public ConfigurationInterface
{
  public:

    //! \brief Default constructor gives reasonable values.
    VectorFieldSimulatorConfiguration();

    virtual ~VectorFieldSimulatorConfiguration() = default;

  private:

    //! \brief The fixed integration step size to use.
    FloatDPApproximation _step_size;
    Nat _mince_dimension;
    Nat _num_subdivisions;
    DiscretisationType _discretisation_type;

  public:

    Void set_step_size(double h) { _step_size=h; }
    FloatDPApproximation const& step_size() const { return _step_size; }

    Void set_d_type(DiscretisationType type) { _discretisation_type = type; }
    DiscretisationType const& discretisation_type() const { return _discretisation_type; }

    Void set_num_sub_div(Nat num_sub_div) { _num_subdivisions = num_sub_div; }
    Nat const& num_subdivisions() const { return _num_subdivisions; }

    Void set_mince_dimension(Nat mince_dim) { _mince_dimension = mince_dim; }
    Nat const& mince_dimension() const { return _mince_dimension; }

    virtual OutputStream& _write(OutputStream& os) const;

};

} // namespace Ariadne

#endif // ARIADNE_VECTOR_FIELD_SIMULATOR_HPP
