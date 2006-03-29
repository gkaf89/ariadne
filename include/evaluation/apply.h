/***************************************************************************
 *            apply.h
 *
 *  17 January 2006
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
 
/*! \file apply.h
 *  \brief Methods for computing the images of sets under maps.
 */

#ifndef _ARIADNE_APPLY_H
#define _ARIADNE_APPLY_H

#include "../geometry/geometry_declarations.h"

#include "../evaluation/evaluation_declarations.h"
#include "../evaluation/map.h"

namespace Ariadne {
  namespace Evaluation {

    template<typename R>
    Geometry::Rectangle<R> 
    apply(const Map<R>& f, const Geometry::Rectangle<R>& p);

    template<typename R>
    Geometry::Parallelotope<R> 
    apply(const Map<R>& f, const Geometry::Parallelotope<R>& p);

    template<typename R, template<typename> class BS>
    Ariadne::Geometry::ListSet<R,BS> 
    apply(const Map<R>& f, const Ariadne::Geometry::ListSet<R,BS>& ds);
     
    template<typename R>
    Ariadne::Geometry::GridMaskSet<R> 
    chainreach(const Map<R>& f, 
               const Ariadne::Geometry::ListSet<R,Ariadne::Geometry::Rectangle>& is, 
               const Ariadne::Geometry::FiniteGrid<R>& g, 
               const Ariadne::Geometry::Rectangle<R>& bb);
    
  }
}

#endif /* _ARIADNE_APPLY_H */
