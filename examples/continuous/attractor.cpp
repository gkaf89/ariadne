/***************************************************************************
 *            attractor.cpp
 *
 *  Copyright  2017-20  Luca Geretti, Pieter Collins
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

#include "ariadne.hpp"

using namespace std;
using namespace Ariadne;

int main(int argc, const char* argv[]) {
    ARIADNE_LOG_SET_VERBOSITY(get_verbosity(argc,argv));
    Logger::instance().use_blocking_scheduler();

    RealVariable x("x"), y("y");
    VectorField system = {{dot(x)=2*x-x*y,dot(y)=2*x*x-y}};
    RealExpressionBoundedConstraintSet initial_set = {{0.9_dec<=x<=1,-2.2_dec<=y<=-2},{sqr(x)+sqr(y+2)<=1}};
    RealExpressionBoundedConstraintSet safe_set = {{-1<=x<=4,-4<=y<=6},{sqr(x-2)+sqr(y-1)<=22}};

    ARIADNE_LOG_PRINTLN_VAR(system);
    ARIADNE_LOG_PRINTLN_VAR(initial_set);
    ARIADNE_LOG_PRINTLN_VAR(safe_set);

    BoundedConstraintSet initial_constraint_set = initial_set.euclidean_set(system.state_space());
    BoundedConstraintSet safe_constraint_set = safe_set.euclidean_set(system.state_space());

    ARIADNE_LOG_PRINTLN_VAR(initial_constraint_set);
    ARIADNE_LOG_PRINTLN_VAR(safe_constraint_set);

    Figure f(ApproximateBoxType{{-5,5},{-4,6}},Projection2d(2,0,1));
    f.set_fill_colour(lightgrey);
    f.draw(safe_constraint_set);
    f.set_fill_colour(orange);
    f.draw(initial_constraint_set);
    f.write("attractor_initial_safe_sets");

    TimeVariable t;

    VectorFieldSimulator simulator(system);
    simulator.configuration().set_step_size(0.1);
    ARIADNE_LOG_PRINTLN("Simulating...");
    auto orbit = simulator.orbit(initial_set,70);

    LabelledFigure g(Axes2d{{-5<=x<=5},{-4<=y<=6}});
    g << orbit.curve();
    g.write("attractor_simulation");

    TaylorPicardIntegrator integrator(0.01);
    ARIADNE_LOG_PRINTLN("Evolving...");
    VectorFieldEvolver evolver(system,integrator);
    evolver.configuration().set_maximum_step_size(0.1);
    auto evolver_orbit = evolver.orbit(initial_set,52.25_dec);
    ARIADNE_LOG_PRINTLN_VAR(evolver_orbit.reach().bounding_box());
    g.clear();
    g << evolver_orbit.reach();
    g.write("attractor_evolution");

    ContinuousReachabilityAnalyser analyser(evolver);
    analyser.configuration().set_transient_time(0.75_dec);
    analyser.configuration().set_lock_to_grid_time(0.75_dec);
    analyser.configuration().set_maximum_grid_extent(5);

    ARIADNE_LOG_PRINTLN("Computing safety...");
    auto safety = analyser.verify_safety(initial_constraint_set,safe_constraint_set);
    ARIADNE_LOG_PRINTLN_VAR(safety.is_safe);
    g.clear();
    g << fill_colour(lightgrey) << safety.safe_set
      << fill_colour(orange) << safety.chain_reach_set;
    g.write("attractor_chain_reach");
}
