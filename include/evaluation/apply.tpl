/***************************************************************************
 *            apply.tpl
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
 
#include <iosfwd>
#include <string>
#include <sstream>
#include <algorithm>

#include <list>
#include <set>
#include <vector>
#include <valarray>

#include "../base/interval.h"

#include "../linear_algebra/vector.h"
#include "../linear_algebra/matrix.h"

#include "../linear_algebra/interval_vector.h"
#include "../linear_algebra/interval_matrix.h"

#include "../geometry/rectangle.h"
#include "../geometry/parallelotope.h"
#include "../geometry/list_set.h"
#include "../geometry/grid_set.h"

#include "../evaluation/apply.h"
#include "../evaluation/map.h"

namespace Ariadne {
  namespace Evaluation {

   
    template<typename R>
    Geometry::Rectangle<R> 
    apply(const Map<R>& f, const Geometry::Rectangle<R>& r) 
    {
      return f.apply(r);
    }
    
    
    template<typename R>
    Geometry::Parallelotope<R> 
    apply(const Map<R>& f, const Geometry::Parallelotope<R>& p) 
    {
      const size_type m=p.dimension();
      const size_type n=p.dimension();
      
      LinearAlgebra::vector< Interval<R> > cuboid_vector(m);
      const Interval<R> unit_interval(-1,1);
      for(size_type i=0; i!=cuboid_vector.size(); ++i) {
        cuboid_vector[i]=Interval<R>(-1,1);
      }
            
      const Geometry::Point<R>& c=p.centre();
      const LinearAlgebra::matrix<R>& g=p.generators();
      
      Geometry::Point<R> img_centre=f.apply(c);
      LinearAlgebra::matrix< Interval<R> > df_on_set = f.derivative(p.bounding_box());
      LinearAlgebra::matrix<R> df_at_centre = f.derivative(c);
      
      LinearAlgebra::matrix<R> img_generators = df_at_centre*g;
      
      LinearAlgebra::matrix<R> img_generators_inverse = LinearAlgebra::inverse(img_generators);
      
      LinearAlgebra::matrix< Interval<R> > cuboid_transform = img_generators_inverse * df_on_set * g;
      
      LinearAlgebra::vector< Interval<R> > new_cuboid = cuboid_transform * cuboid_vector;
      
      R new_cuboid_sup(0);
      for(size_type j=0; j!=n; ++j) {
        new_cuboid_sup=std::max( new_cuboid_sup, R(abs(new_cuboid[j].lower())) );
        new_cuboid_sup=std::max( new_cuboid_sup, R(abs(new_cuboid[j].upper())) );
      }
      
      Geometry::Parallelotope<R> result(img_centre,img_generators);
      return result;
    }

    template<typename R, template<typename> class BS>
    Ariadne::Geometry::ListSet<R,BS> 
    apply(const Map<R>& f, const Ariadne::Geometry::ListSet<R,BS>& ds) {
      Ariadne::Geometry::ListSet<R,BS> result(f.result_dimension());
      for(typename Geometry::ListSet<R,BS>::const_iterator iter=ds.begin(); iter!=ds.end(); ++iter) {
        result.push_back(apply(f,*iter));
      }
      return result;
    }
     
    
    template<typename R>
    Ariadne::Geometry::GridMaskSet<R> 
    chainreach(const Map<R>& f, 
               const Ariadne::Geometry::ListSet<R,Ariadne::Geometry::Rectangle>& is, 
               const Ariadne::Geometry::FiniteGrid<R>& g, 
               const Ariadne::Geometry::Rectangle<R>& bb) 
    {
      typedef typename Ariadne::Geometry::GridMaskSet<R>::const_iterator gms_const_iterator;
      
      Ariadne::Geometry::GridMaskSet<R> result(g);
      Ariadne::Geometry::GridMaskSet<R> found=over_approximation_of_intersection(is,bb,g);
      Ariadne::Geometry::GridMaskSet<R> image(g);
      
      while(!subset(found,result)) {
        found=difference(found,result);
        result.adjoin(found);
        image=Ariadne::Geometry::GridMaskSet<R>(g);
        uint size=0;
        for(gms_const_iterator iter=found.begin(); iter!=found.end(); ++iter) {
          ++size;
          Ariadne::Geometry::Rectangle<R> r=*iter;
          Ariadne::Geometry::Parallelotope<R> pp(r);
          Ariadne::Geometry::Parallelotope<R> fp(Ariadne::Evaluation::apply(f,pp));
          Geometry::GridCellListSet<R> oai=over_approximation_of_intersection(fp,bb,g);
          image.adjoin(oai);
        }
        std::cerr << size << std::endl;
        found=image;
      }
      return result;
    }

  }
}
