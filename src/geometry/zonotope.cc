/***************************************************************************
 *            zonotope.cc
 *
 *  Copyright  2006  Alberto Casagrande, Pieter Collins
 *  casagrande@dimi.uniud.it, Pieter.Collins@cwi.nl
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

#include "numeric/rational.h"
#include "numeric/float.h"
#include "numeric/interval.h"

#include "geometry/zonotope.h"
#include "geometry/zonotope.code.h"

namespace Ariadne {
  namespace Geometry {

    using namespace Numeric;
    
    template class Zonotope<Rational,ExactTag>;

#ifdef ENABLE_FLOAT64
    template class Zonotope<Float64,ExactTag>;
    template class Zonotope<Float64,UniformErrorTag>;
    template class Zonotope<Float64,IntervalTag>;
#endif
  
#ifdef ENABLE_FLOATMP
    template class Zonotope<FloatMP,ExactTag>;
    template class Zonotope<FloatMP,UniformErrorTag>;
    template class Zonotope<FloatMP,IntervalTag>;
#endif

  }
}
