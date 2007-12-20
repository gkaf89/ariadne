/***************************************************************************
 *            python/export_postscript_output.cc
 *
 *  Copyright  2005-7  Alberto Casagrande, Pieter Collins
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is diself_ns::stributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include "python/float.h"

#include "geometry/point.h"
#include "geometry/rectangle.h"
#include "geometry/zonotope.h"
#include "geometry/polytope.h"
#include "geometry/polyhedron.h"
#include "geometry/list_set.h"
#include "geometry/grid.h"
#include "geometry/grid_set.h"
#include "geometry/partition_tree_set.h"

#include "output/epsstream.h"

using namespace Ariadne;
using namespace Ariadne::Numeric;
using namespace Ariadne::Geometry;
using namespace Ariadne::Output;
using namespace Ariadne::Python;

#include <boost/python.hpp>
using namespace boost::python;

template<class S> inline void write(epsfstream& eps, const S& s) { eps << s; }
template<class R> inline void write_rectangle(epsfstream& eps, const Rectangle<R>& r) { eps << r; }
template<class R> inline void write_rectangular_set(epsfstream& eps, const RectangularSet<R>& r) { eps << r; }
template<class R,class Tag> inline void write_zonotope(epsfstream& eps, const Zonotope<R,Tag>& z) { eps << z; }
template<class R> inline void write_polytope(epsfstream& eps, const Polytope<R>& p) { eps << p; }
template<class R> inline void write_polyhedron(epsfstream& eps, const Polyhedron<R>& p) { eps << p; }
template<class R> inline void write_polyhedral_set(epsfstream& eps, const PolyhedralSet<R>& p) { eps << p; }
template<class BS> inline void write_list_set(epsfstream& eps, const ListSet<BS>& ls) { eps << ls; }
template<class R> inline void write_polytope_list_set(epsfstream& eps, const ListSet< Polytope<R> >& s) { eps << s; }
template<class R> inline void write_grid_cell(epsfstream& eps, const GridCell<R>& r) { eps << Rectangle<R>(r); }
template<class R> inline void write_grid_block(epsfstream& eps, const GridBlock<R>& r) { eps << Rectangle<R>(r); }
template<class R> inline void write_grid_cell_list_set(epsfstream& eps, const GridCellListSet<R>& s) { eps << s; }
template<class R> inline void write_grid_mask_set(epsfstream& eps, const GridMaskSet<R>& s) { eps << s; }
template<class R> inline void write_partition_tree_set(epsfstream& eps, const PartitionTreeSet<R>& s) { eps << s; }
template<class R> inline void write_finite_grid(epsfstream& eps, const FiniteGrid<R>& fg) { eps << fg; }
template<class R> inline void write_partition_tree(epsfstream& eps, const PartitionTree<R>& s) { eps << s; }
template<class R> inline void epsfstream_open(epsfstream& eps, const Ariadne::Geometry::Rectangle<R>& bbox, int ix, int iy) { eps.open("Ariadne",bbox,ix,iy); }
template<class R> inline void epsfstream_open_with_defaults(epsfstream& eps, const Ariadne::Geometry::Rectangle<R>& bbox) { eps.open("Ariadne",bbox); }
inline void epsfstream_close(epsfstream& eps) { eps.close(); }

void export_postscript_output()
{

  class_<PlanarProjectionMap>("PlanarProjectionMap",init<dimension_type, dimension_type, dimension_type>())
    .def(self_ns::str(self))
  ;
    
  class_<epsfstream, boost::noncopyable>("EpsPlot",init<>())
    .def("open",(void(epsfstream::*)(const char* fn,const Rectangle<FloatPy>&))&epsfstream::open<FloatPy>)
    .def("open",(void(epsfstream::*)(const char* fn,const Rectangle<FloatPy>&,uint,uint))&epsfstream::open<FloatPy>)
    .def("open",(void(epsfstream::*)(const char* fn,const Rectangle<FloatPy>&,const PlanarProjectionMap&))&epsfstream::open<FloatPy>)
    .def("open",(void(epsfstream::*)(const char*,const Rectangle2d&,const PlanarProjectionMap&))&epsfstream::open)
    .def("open",&epsfstream_open_with_defaults<FloatPy>)
    .def("close",&epsfstream_close)
    .def("set_pen_colour",(void(epsfstream::*)(const char*))&epsfstream::set_pen_colour)
    .def("set_fill_colour",(void(epsfstream::*)(const char*))&epsfstream::set_fill_colour)
    .def("set_pen_colour",(void(epsfstream::*)(const Colour&))&epsfstream::set_pen_colour)
    .def("set_fill_colour",(void(epsfstream::*)(const Colour&))&epsfstream::set_fill_colour)
    .def("set_line_style",(void(epsfstream::*)(bool))&epsfstream::set_line_style)
    .def("set_fill_style",(void(epsfstream::*)(bool))&epsfstream::set_fill_style)
    .def("write",&write< Rectangle<FloatPy> >)
    .def("write",&write< RectangularSet<FloatPy> >)
    .def("write",&write< Zonotope<FloatPy,ExactTag> >)
    .def("write",&write< Zonotope<FloatPy,UniformErrorTag> >)
    .def("write",&write< Polytope<FloatPy> >)
    .def("write",&write< Polyhedron<FloatPy> >)
    .def("write",&write< PolyhedralSet<FloatPy> >)
    .def("write",&write< ListSet< Rectangle<FloatPy> > >)
    .def("write",&write< ListSet< Polytope<FloatPy> > >)
    .def("write",&write< ListSet< Zonotope<FloatPy,ExactTag> > >)
    .def("write",&write< ListSet< Zonotope<FloatPy,UniformErrorTag> > >)
    .def("write",&write< GridCell<FloatPy> >)
    .def("write",&write< GridBlock<FloatPy> >)
    .def("write",&write< GridCellListSet<FloatPy> >)
    .def("write",&write< GridMaskSet<FloatPy> >)
    .def("write",&write< PartitionTreeSet<FloatPy> >)
    .def("write",&write< FiniteGrid<FloatPy> >)
    .def("write",&write< PartitionTree<FloatPy> >)
  ;
  
}
