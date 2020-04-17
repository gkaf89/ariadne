/***************************************************************************
 *            hybrid/hybrid_paving.cpp
 *
 *  Copyright  2008-20  Pieter Collins
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

#include "../function/functional.hpp"
#include "../config.hpp"

#include "../hybrid/hybrid_expression_set.hpp"
#include "../hybrid/hybrid_set.hpp"
#include "../hybrid/hybrid_paving.hpp"

#include "../numeric/real.hpp"

#include "../symbolic/expression_set.hpp"
#include "../geometry/function_set.hpp"

#include "../hybrid/hybrid_space.hpp"
#include "../hybrid/hybrid_time.hpp"
#include "../hybrid/hybrid_orbit.hpp"
#include "../hybrid/hybrid_automaton_interface.hpp"
#include "../output/graphics.hpp"
#include "../hybrid/hybrid_graphics.hpp"
#include "../numeric/rounding.hpp"
#include "../symbolic/assignment.hpp"
#include "../output/graphics_interface.hpp"
#include "../geometry/function_set.hpp"

namespace Ariadne {


HybridGridTreePaving::ConstIterator HybridGridTreePaving::begin() const {
    return ConstIterator(this->_map,this->_hgrid.space(),false);
}

HybridGridTreePaving::ConstIterator HybridGridTreePaving::end() const {
    return ConstIterator(this->_map,this->_hgrid.space(),true);
}


Void HybridGridTreePaving::clear() {
    for(HybridGridTreePaving::LocationsIterator _loc_iter=this->locations_begin(); _loc_iter!=this->locations_end(); ++_loc_iter) {
        _loc_iter->second.clear();
    }
}

Void HybridGridTreePaving::adjoin(const HybridGridCell& hgc) {
    this->_provide_location(hgc.location()).adjoin(hgc.euclidean_set());
}

Void HybridGridTreePaving::adjoin(const ListSet<HybridGridCell>& hgcls) {
    for(ListSet<HybridGridCell>::ConstIterator iter=hgcls.begin(); iter!=hgcls.end(); ++iter) {
        this ->adjoin(*iter);
    }
}

Void HybridGridTreePaving::adjoin(const HybridGridTreePaving& hgts) {
    for(HybridGridTreePaving::LocationsConstIterator _loc_iter=hgts.locations_begin(); _loc_iter!=hgts.locations_end(); ++_loc_iter) {
        this->_provide_location(_loc_iter->first).adjoin(_loc_iter->second);
    }
}

Void HybridGridTreePaving::remove(const HybridGridTreePaving& hgts) {
    for(HybridGridTreePaving::LocationsConstIterator _loc_iter=hgts.locations_begin(); _loc_iter!=hgts.locations_end(); ++_loc_iter) {
        this->_provide_location(_loc_iter->first).remove(_loc_iter->second);
    }
}

Void HybridGridTreePaving::restrict(const HybridGridTreePaving& hgts) {
    for(HybridGridTreePaving::LocationsConstIterator _loc_iter=hgts.locations_begin(); _loc_iter!=hgts.locations_end(); ++_loc_iter) {
        this->_provide_location(_loc_iter->first).restrict(_loc_iter->second);
    }
}

Void HybridGridTreePaving::restrict_to_extent(Nat h) {
    for(LocationsIterator _loc_iter=locations_begin(); _loc_iter!=locations_end(); ++_loc_iter) {
        _loc_iter->second.restrict_to_extent(h);
    }
}

Void HybridGridTreePaving::adjoin_inner_approximation(const HybridSetInterface& hset, const Nat fineness) {
    Set<DiscreteLocation> locations=hset.locations();
    for(auto location : locations) {
        RealSpace space = this->space(location);
        this->_provide_location(location).adjoin_inner_approximation(hset.euclidean_set(location,space),fineness);
    }
}

Void HybridGridTreePaving::adjoin_inner_approximation(const HybridExactBoxes& hbxs, const Nat fineness) {
    for(HybridExactBoxes::ConstIterator _loc_iter=hbxs.begin();
            _loc_iter!=hbxs.end(); ++_loc_iter) {
        DiscreteLocation const& loc=_loc_iter->first;
        LabelledSet<ExactBoxType> const& vbx=_loc_iter->second;
        ARIADNE_ASSERT(vbx.space() == this->space(loc));
        this->_provide_location(loc).adjoin_inner_approximation(vbx.euclidean_set(),fineness);
    }
}

Void HybridGridTreePaving::adjoin_lower_approximation(const HybridOvertSetInterface& hs, const Nat extent, const Nat fineness) {
    Set<DiscreteLocation> hlocs=dynamic_cast<const HybridBoundedSetInterface&>(hs).locations();
    for(Set<DiscreteLocation>::ConstIterator _loc_iter=hlocs.begin();
            _loc_iter!=hlocs.end(); ++_loc_iter) {
        DiscreteLocation loc=*_loc_iter;
        RealSpace spc=this->space(loc);
        this->_provide_location(loc).adjoin_lower_approximation(hs.euclidean_set(loc,spc),extent,fineness);
    }
}

Void HybridGridTreePaving::adjoin_outer_approximation(const HybridCompactSetInterface& hs, const Nat fineness) {
    Set<DiscreteLocation> hlocs=hs.locations();
    for(Set<DiscreteLocation>::ConstIterator _loc_iter=hlocs.begin();
            _loc_iter!=hlocs.end(); ++_loc_iter) {
        DiscreteLocation loc=*_loc_iter;
        RealSpace spc=this->space(loc);
        this->_provide_location(loc).adjoin_outer_approximation(hs.euclidean_set(loc,spc),fineness);
    }
}

Void HybridGridTreePaving::adjoin_outer_approximation(const HybridExactBoxes& hbxs, const Nat fineness) {
    for(HybridExactBoxes::ConstIterator _loc_iter=hbxs.begin();
            _loc_iter!=hbxs.end(); ++_loc_iter) {
        DiscreteLocation const& loc=_loc_iter->first;
        LabelledSet<ExactBoxType> const& vbx=_loc_iter->second;
        ARIADNE_ASSERT(vbx.space() == this->space(loc));
        this->_provide_location(_loc_iter->first).adjoin_outer_approximation(vbx.euclidean_set(),fineness);
    }
}


GridTreePaving& HybridGridTreePaving::operator[](DiscreteLocation q) {
    return this->_provide_location(q);
}

const GridTreePaving& HybridGridTreePaving::operator[](DiscreteLocation q) const {
    ARIADNE_ASSERT_MSG(this->has_location(q),"q="<<q);
    return const_cast<HybridGridTreePaving*>(this)->_provide_location(q);
}

Bool HybridGridTreePaving::is_empty() const {
    for(LocationsConstIterator _loc_iter=this->locations_begin();
        _loc_iter!=this->locations_end(); ++_loc_iter) {
        if(!_loc_iter->second.is_empty()) { return false; }
    }
    return true;
}

SizeType HybridGridTreePaving::size() const {
    SizeType result=0;
    for(LocationsConstIterator _loc_iter=this->locations_begin(); _loc_iter!=this->locations_end(); ++_loc_iter) {
        result+=_loc_iter->second.size();
    }
    return result;
}

HybridListSet<ExactBoxType> HybridGridTreePaving::boxes() const {
    HybridListSet<ExactBoxType> result;
    for(ConstIterator iter=this->begin(); iter!=this->end(); ++iter) {
        result.adjoin(iter->location(),iter->euclidean_set().box());
    }
    return result;
}

Void HybridGridTreePaving::mince(Nat fineness) {
    for(LocationsIterator _loc_iter=this->locations_begin();
        _loc_iter!=this->locations_end(); ++_loc_iter) {
        _loc_iter->second.mince(fineness);
    }
}

Void HybridGridTreePaving::recombine() {
    for(LocationsIterator _loc_iter=this->locations_begin();
        _loc_iter!=this->locations_end(); ++_loc_iter) {
        _loc_iter->second.recombine();
    }
}

ValidatedLowerKleenean HybridGridTreePaving::separated(const HybridExactBox& hbx) const {
    LocationsConstIterator _loc_iter = this->_map.find( hbx.location() );
    return _loc_iter != this->locations_end() || _loc_iter->second.separated( hbx.euclidean_set() );
}

ValidatedLowerKleenean HybridGridTreePaving::overlaps(const HybridExactBox& hbx) const {
    LocationsConstIterator _loc_iter = this->_map.find( hbx.location() );
    return _loc_iter != this->locations_end() && _loc_iter->second.overlaps( hbx.euclidean_set() );
}

ValidatedLowerKleenean HybridGridTreePaving::covers(const HybridExactBox& hbx) const {
    LocationsConstIterator _loc_iter=this->_map.find(hbx.location());
    return _loc_iter!=this->locations_end() && _loc_iter->second.covers( hbx.euclidean_set() );
}

ValidatedLowerKleenean HybridGridTreePaving::inside(const HybridExactBoxes& hbx) const  {
    for( LocationsConstIterator _loc_iter = this->locations_begin(); _loc_iter != this->locations_end(); ++_loc_iter ) {
        if( !_loc_iter->second.is_empty() ) {
            DiscreteLocation const& loc = _loc_iter->first;
            RealSpace spc=this->space(loc);
            return this->euclidean_set(loc).inside(hbx.euclidean_set(loc,spc));
        }
    }
    return true;
}

HybridUpperBoxes HybridGridTreePaving::bounding_box() const {
    HybridExactBoxes result;
    for( LocationsConstIterator _loc_iter = this->locations_begin(); _loc_iter != this->locations_end(); ++_loc_iter ) {
        if( !_loc_iter->second.is_empty() ) {
            DiscreteLocation const& loc = _loc_iter->first;
            RealSpace const& spc=this->space(loc);
            result.insert(loc,spc,cast_exact_box(_loc_iter->second.bounding_box()));
        }
    }
    return result;
}

Bool subset(const HybridGridTreePaving& hgts1, const HybridGridTreePaving& hgts2) {
    for( typename HybridGridTreePaving::LocationsConstIterator _loc_iter = hgts1.locations_begin(); _loc_iter != hgts1.locations_end(); ++_loc_iter ) {
        if( !_loc_iter->second.is_empty() ) {
            DiscreteLocation const& loc = _loc_iter->first;
            RealSpace spc=hgts1.space(loc);
            if(not subset(hgts1.euclidean_set(loc),hgts2.euclidean_set(loc,spc))) {
                return false;
            }
        }
    }
    return true;
}

Bool intersect(const HybridGridTreePaving& hgts1, const HybridGridTreePaving& hgts2) {
    for( typename HybridGridTreePaving::LocationsConstIterator _loc_iter = hgts1.locations_begin(); _loc_iter != hgts1.locations_end(); ++_loc_iter ) {
        if( !_loc_iter->second.is_empty() ) {
            DiscreteLocation const& loc = _loc_iter->first;
            RealSpace spc=hgts1.space(loc);
            if(intersect(hgts1.euclidean_set(loc),hgts2.euclidean_set(loc,spc))) {
                return true;
            }
        }
    }
    return false;
}

OutputStream& HybridGridTreePaving::_write(OutputStream& os) const {
    return os << this->_map;
}

Void HybridGridTreePaving::draw(CanvasInterface& canvas, const Set<DiscreteLocation>& locations, const Variables2d& axis_variables) const {
    for(LocationsConstIterator loc_iter=this->locations_begin(); loc_iter!=this->locations_end(); ++loc_iter) {
        if(locations.empty() || locations.contains(loc_iter->first)) {
            RealSpace const& space=this->space(loc_iter->first);
            Projection2d projection(space.dimension(),space.index(axis_variables.x_variable()),space.index(axis_variables.y_variable()));
            loc_iter->second.draw(canvas,projection);
        }
    }
}


GridTreePaving& HybridGridTreePaving::_provide_location(const DiscreteLocation& q) {
    std::map<DiscreteLocation,GridTreePaving>::iterator iter=this->_map.find(q);
    if(iter==this->_map.end()) {
        this->_map.insert(std::make_pair(q,GridTreePaving(this->_hgrid[q])));
        iter=this->_map.find(q);
    }
    return iter->second;
}


Grid HybridScalings::grid(const DiscreteLocation& loc, const RealSpace& space) const
{
    Vector<RawFloatDP> lengths(space.size());
    for(SizeType i=0; i!=space.size(); ++i) {
        lengths[i] = (this->scaling(loc,space.variable(i))).raw();
    }
    return Grid(lengths);
}

HybridGridTreePaving extend_auxiliary(const HybridGridTreePaving& hybrid_paving,
                                      const HybridAutomatonInterface& hybrid_automaton) {
    HybridGrid hybrid_state_grid = hybrid_paving.grid();
    HybridSpace hybrid_state_space = hybrid_state_grid.space();
    HybridScalings scalings = hybrid_state_grid.scalings();

    HybridSpace hybrid_state_auxiliary_space = hybrid_automaton.state_auxiliary_space();
    HybridGrid hybrid_state_auxiliary_grid(hybrid_state_auxiliary_space,scalings);

    HybridGridTreePaving result(hybrid_state_auxiliary_grid);
    for (auto iter = hybrid_paving.locations_begin(); iter!=hybrid_paving.locations_end(); ++iter) {
        DiscreteLocation const& location=iter->first;
        GridTreePaving const& state_paving=iter->second;
        RealSpace auxiliary_space=hybrid_automaton.continuous_auxiliary_space(location);
        EffectiveVectorMultivariateFunction auxiliary_function=hybrid_automaton.auxiliary_function(location);
        Grid auxiliary_grid=scalings.grid(location,auxiliary_space);
        GridTreePaving state_auxiliary_paving=outer_skew_product(state_paving,auxiliary_grid,auxiliary_function);
        result[location]=state_auxiliary_paving;
    }
    return result;
}



struct Orbit<HybridGridCell>::Data {
    Data(const HybridGrid& grid)
        : initial(grid), reach(grid), intermediate(grid), final(grid) { }
    HybridGridTreePaving initial;
    HybridGridTreePaving reach;
    HybridGridTreePaving intermediate;
    HybridGridTreePaving final;
};

Orbit<HybridGridCell>::
Orbit(const HybridGridTreePaving& initial_set)
    : _data(new Data(initial_set.grid()))
{
    this->_data->initial=initial_set;
}

Orbit<HybridGridCell>::
Orbit(const HybridGridTreePaving& initial_set,
      const HybridGridTreePaving& reach_set,
      const HybridGridTreePaving& intermediate_set,
      const HybridGridTreePaving& final_set)
    : _data(new Data(initial_set.grid()))
{
    this->_data->initial=initial_set;
    this->_data->reach=reach_set;
    this->_data->intermediate=intermediate_set;
    this->_data->final=final_set;
}

HybridGridTreePaving const&
Orbit<HybridGridCell>::
initial() const
{
    return this->_data->initial;
}

HybridGridTreePaving const&
Orbit<HybridGridCell>::
reach() const
{
    return this->_data->reach;
}

HybridGridTreePaving const&
Orbit<HybridGridCell>::
intermediate() const
{
    return this->_data->intermediate;
}

HybridGridTreePaving const&
Orbit<HybridGridCell>::
final() const
{
    return this->_data->final;
}

Orbit<HybridGridCell> extend_auxiliary(const Orbit<HybridGridCell>& orbit, const HybridAutomatonInterface& ha) {
    return Orbit<HybridGridCell>(extend_auxiliary(orbit.initial(),ha),extend_auxiliary(orbit.reach(),ha),
                                 extend_auxiliary(orbit.intermediate(),ha),extend_auxiliary(orbit.final(),ha));
}

} // namespace Ariadne
