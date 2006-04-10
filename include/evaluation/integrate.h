/***************************************************************************
 *            integrate.h
 *
 *  Copyright  2006  Alberto Casagrande, Pieter Collins
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
 
/*! \file integrate.h
 *  \brief Methods for integrating points and sets under a vector field.
 */

#ifndef _ARIADNE_INTEGRATE_H
#define _ARIADNE_INTEGRATE_H

#include "../declarations.h"

namespace Ariadne {
  namespace Evaluation {
   

    /*! \brief Integrate a rectangle. */
    template<typename R>
    Geometry::Rectangle<R> 
    integrate(const VectorField<R>& vector_field, const Geometry::Rectangle<R>& initial_set, const R& time, const R& step_size);

    /*! \brief Integrate a parallelotope. */
    template<typename R>
    Geometry::Parallelotope<R> 
    integrate(const VectorField<R>& vector_field, const Geometry::Parallelotope<R>& initial_set, const R& time, const R& step_size);

    
    /*! \brief Integrate a set. */
    template<typename R, template<typename> class BS>
    Geometry::ListSet<R,BS> 
    integrate(const VectorField<R>& vector_field, 
              const Geometry::ListSet<R,BS>& initial_set, 
              const R& time, 
              const R& step_size);

    /*! \brief Integrate a set within a boundin box using a C1 algorithm. */
    template<typename R>
    Geometry::GridMaskSet<R> 
    integrate(const VectorField<R>& vector_field, 
              const Geometry::GridMaskSet<R>& initial_set, 
              const Geometry::GridMaskSet<R>& bounding_set,
              const R& time, 
              const R& step_size);

    
    /*! \brief Compute the reachable set. */
    template<typename R, template<typename> class BS>
    Geometry::ListSet<R,BS> 
    reach(const VectorField<R>& vector_field, 
          const Geometry::ListSet<R,BS>& initial_set, 
          const R& time, 
          const R& step_size);

    /*! \brief Integrate a set using a C1 algorithm. */
    template<typename R>
    Geometry::GridMaskSet<R> 
    reach(const VectorField<R>& vector_field, 
          const Geometry::GridMaskSet<R>& initial_set, 
          const Geometry::GridMaskSet<R>& bounding_set, 
          const R& time, 
          const R& step_size);

    
/*
    template<typename R>
    Ariadne::Geometry::GridMaskSet<R> 
    chainreach(const VectorField<R>& f, 
               const Ariadne::Geometry::ListSet<R,Ariadne::Geometry::Rectangle>& is, 
               const Ariadne::Geometry::FiniteGrid<R>& g, 
               const Ariadne::Geometry::Rectangle<R>& bb);
*/
    
  }
}

#endif /* _ARIADNE_INTEGRATE_H */
