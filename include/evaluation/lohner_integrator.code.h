/***************************************************************************
 *            lohner_integrator.code.h
 *
 *  Copyright  2006-7  Alberto Casagrande, Pieter Collins
 *  casagrande@dimi.uniud.it, pieter.collins@cwi.nl
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
 
//#define DEBUG

#include <iosfwd>
#include <string>
#include <sstream>
#include <algorithm>
#include <typeinfo>

#include <list>
#include <set>
#include <vector>
#include <valarray>

#include "lohner_integrator.h"

#include "base/stlio.h"
#include "base/array.h"
#include "base/exceptions.h"
#include "numeric/interval.h"

#include "linear_algebra/vector.h"
#include "linear_algebra/matrix.h"

#include "function/affine_model.h"
#include "function/taylor_model.h"

#include "geometry/box.h"
#include "geometry/zonotope.h"

#include "system/vector_field.h"

#include "evaluation/integrator.h"
#include "evaluation/standard_integrator.h"

#include "output/logging.h"

namespace {

using namespace Ariadne;

template<class R>
LinearAlgebra::Matrix<R>
symmetrize(const LinearAlgebra::Vector< Numeric::Interval<R> >& iv)
{
  LinearAlgebra::Matrix<R> A(iv.size(),iv.size()+1);
  for(size_type i=0; i!=A.number_of_rows(); ++i) {
    A(i,i)=radius(iv(i));
    A(i,iv.size())=midpoint(iv(i));
  }
  return A;
}

}



namespace Ariadne { 

namespace Evaluation { static int& verbosity = integrator_verbosity; }



template<class R>
Evaluation::LohnerIntegrator<R>*
Evaluation::LohnerIntegrator<R>::clone() const
{
  return new LohnerIntegrator<R>(*this);
}


template<class R>
Evaluation::LohnerIntegrator<R>::LohnerIntegrator(smoothness_type temporal_order)
  : _integrator(new IntegratorBase<R>(temporal_order, 1u))
{
}



template<class R> inline
std::pair< Numeric::Rational, Geometry::Box<R> >
Evaluation::LohnerIntegrator<R>::flow_bounds(const System::VectorField<R>& vf, 
                                             const Geometry::Box<R>& bx,
                                             const Numeric::Rational& t) const
{
  return this->_integrator->flow_bounds(vf,bx,t);
}


template<class R>
Geometry::Zonotope<R>
Evaluation::LohnerIntegrator<R>::integration_step(const System::VectorField<R>& vector_field, 
                                                  const Geometry::Zonotope<R>& initial_set, 
                                                  const Numeric::Rational& step_size, 
                                                  const Geometry::Box<R>& flow_bounding_box) const
{
  using namespace Numeric;
  using namespace LinearAlgebra;
  using namespace Function;
  using namespace Geometry;
  using namespace System;

  ARIADNE_LOG(5,"LohnerIntegrator::integration_step(VectorField,Zonotope,Time,Box)\n");
  ARIADNE_LOG(8,"spacial_order="<<this->_integrator->spacial_order()<<"\n");
  ARIADNE_LOG(6,"temporal_order="<<this->_integrator->temporal_order()<<"\n");
  ARIADNE_LOG(6,"flow_bounding_box="<<flow_bounding_box<<"\n");
  ARIADNE_LOG(6,"initial_set="<<initial_set<<"\n");
  AffineModel<R> affine_flow_model=this->_integrator->affine_flow_model(vector_field,initial_set.centre(),initial_set.bounding_box(),step_size,flow_bounding_box);
  TaylorModel<R> taylor_flow_model=this->_integrator->taylor_flow_model(vector_field,initial_set.centre(),initial_set.bounding_box(),step_size,flow_bounding_box);
  ARIADNE_LOG(6,"affine_flow_model="<<affine_flow_model<<"\n");
  ARIADNE_LOG(6,"taylor_flow_model="<<taylor_flow_model<<"\n");
  /*
  affine_flow_model=AffineModel<R>(Geometry::Box<R>(taylor_flow_model.domain()),
                                   Geometry::Point<R>(taylor_flow_model.centre()),
                                   Geometry::Point<I>(taylor_flow_model.evaluate(taylor_flow_model.centre())),
                                   LinearAlgebra::Matrix<I>(taylor_flow_model.jacobian(taylor_flow_model.domain().position_vectors())));
  */
  ARIADNE_LOG(6,"affine_flow_model="<<affine_flow_model<<"\n");
  Zonotope<R> flow_set=Geometry::apply(affine_flow_model,initial_set);
  ARIADNE_LOG(6,"flow_set="<<flow_set<<"\n");

  return Geometry::orthogonal_over_approximation(flow_set);
}




template<class R>
Geometry::Zonotope<R> 
Evaluation::LohnerIntegrator<R>::reachability_step(const System::VectorField<R>& vector_field, 
                                                   const Geometry::Zonotope<R>& initial_set,
                                                   const Numeric::Rational& step_size,
                                                   const Geometry::Box<R>& bounding_box) const
{
  using namespace Numeric;
  using namespace LinearAlgebra;
  using namespace Function;
  using namespace Geometry;
  using namespace System;

  ARIADNE_LOG(6,"LohnerIntegrator::reachability_step(VectorField,Zonotope<Interval>,Interval,Box) const\n");
  Rational half_step_size=step_size/2;

  /*
  Zonotope<R> midset=this->integration_step(vector_field,initial_set,half_step_size,bounding_box);
  Vector<I> ihhf=I(half_step_size)*vector_field(bounding_box);
  Vector<R> hhf=midpoint(ihhf);
  Zonotope<R> result(midset.centre()+(ihhf-hhf),concatenate_columns(midset.generators(),hhf));
  */

  AffineModel<R> flow_model=this->_integrator->affine_flow_model(vector_field,initial_set.centre(),initial_set.bounding_box(),half_step_size,bounding_box);
  Point<I> phic=flow_model.value();
  Matrix<I> Dphi=flow_model.jacobian();
  Matrix<I> gen=Dphi*initial_set.generators();
  Vector<I> hhf=I(half_step_size)*vector_field(bounding_box);
  Vector<I> err=Dphi*(I(-1,1)*initial_set.error());

  Zonotope<R> result(phic+err,concatenate_columns(gen,hhf));

  return result;
}



template<class R>
std::ostream&
Evaluation::LohnerIntegrator<R>::write(std::ostream& os) const
{
  return os << "LohnerIntegrator( temporal_order=" << this->_integrator->temporal_order() <<" )";
}



}
