/***************************************************************************
 *            affine_vector_field.h
 *
 *  Fri Feb  4 08:57:39 2005
 *  Copyright  2005  Alberto Casagrande
 *  casagrande@dimi.uniud.it
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
 
#ifndef _AFFINE_VECTOR_FIELD_H
#define _AFFINE_VECTOR_FIELD_H

#include "../linear_algebra/vector.h"
#include "../linear_algebra/matrix.h"

#include "../linear_algebra/interval_vector.h"
#include "../linear_algebra/interval_matrix.h"


#include "../evaluation/vector_field.h"
#include "../evaluation/affine_map.h"

namespace Ariadne {
  namespace Evaluation {

    template <typename R>
    class AffineVectorField : public VectorField<R> 
    {
      typedef typename Geometry::Polyhedron<R> Polyhedron;
      typedef typename Geometry::Rectangle<R> Rectangle;
    
     public:
      typedef typename Geometry::Point<R> State;
      
      typedef LinearAlgebra::matrix<R> Matrix;
      typedef LinearAlgebra::vector<R> Vector;
    
      typedef LinearAlgebra::vector< Interval<R> > IntervalVector;
      typedef LinearAlgebra::matrix< Interval<R> > IntervalMatrix;
    
      AffineVectorField(const AffineVectorField<R>& F) : _A(F.A()), _b(F.b()) { }
      AffineVectorField(const Matrix &A, const Vector &b) : _A(A), _b(b) { }
    
      Vector apply(const State& s) const { return this->_A*s.position_vector()+this->_b; }
      IntervalVector apply(const Rectangle& r) const {
        IntervalVector iv(this->dimension());
        for(dimension_type i=0; i!=this->dimension(); ++i) {
          iv[i]=r.interval(i);
        }
        return IntervalVector(this->_A*iv)+(this->_b);
      }
    
      Matrix derivative(const State& x) const { return this->_A; }
      IntervalMatrix df(const Rectangle& r) const { 
        IntervalMatrix result(this->dimension(),this->dimension());
        for(dimension_type i=0; i!=this->dimension(); ++i) {
          for(dimension_type j=0; j!=this->dimension(); ++j) {
            result(i,j)=this->_A(i,j);
          }
        }
        return result;
      }
      
      IntervalMatrix derivative(const Rectangle& r) const { 
        return this->_A;
      }
      
      inline const Matrix& A() const { return this->_A; }
      inline const Vector& b() const { return this->_b; }
      
      inline dimension_type dimension() const {
        return this->_b.size();
      }
      
     private:
      Matrix _A;
      Vector _b;
    };
 
  }
}

#endif /* _AFFINE_VECTOR_FIELD_H */
