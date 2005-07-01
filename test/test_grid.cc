/***************************************************************************
 *            test_grid.cc
 *
 *  9 May 2005
 *  Copyright  2005  Pieter Collins
 *  Email Pieter.Collins@cwi.nl, casagrande@dimi.uniud.it
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

#include <iostream>
#include <fstream>
#include <string>

#include "ariadne.h"
#include "exception.h"
#include "utility.h"
#include "numerical_type.h"
#include "state.h"
#include "rectangle.h"
#include "binary_word.h"
#include "grid.h"
#include "grid_rectangle.h"
#include "grid_denotable_set.h"
#include "grid_operations.h"

#include "test.h"

using namespace Ariadne;
using namespace Ariadne::Geometry;
using namespace std;

template class Rectangle<Rational>;
template class Grid<Rational>;
template class FiniteGrid<Rational>;
template class InfiniteGrid<Rational>;
template class PartitionGrid<Rational>;

template class GridCell<Rational>;
template class GridRectangle<Rational>;
template class PartitionTreeCell<Rational>;

template class GridRectangleListSet<Rational>;
template class GridCellListSet<Rational>;
template class GridMaskSet<Rational>;

int main() {

  cout << "test_grid: " << flush;
  ofstream clog("test_grid.log");

  ListSet<Rational,Rectangle> ls;

  string input("[ [[0,5/6],[0,4/3]], [[2/3,1],[1,4/3]], [[4/3,3/2],[4/3,5/2]] ]");
  stringstream is(input);
  is >> ls;
  clog << ls << endl;

  FiniteGrid<Rational> grid(ls);
  GridRectangleListSet<Rational> grls(ls);
  clog << grls << endl;
  clog << ListSet<Rational,Rectangle>(grls) << endl;

  GridCellListSet<Rational> gcls(grls);
  clog << gcls << endl;
  clog << ListSet<Rational,Rectangle>(gcls) << endl;

  GridMaskSet<Rational> gms(grls);
  clog << gms << endl;

  GridMaskSet<Rational> gcms(gcls);
  clog << gcms << endl;
  GridCellListSet<Rational> gclms(gms);
  clog << gclms << endl;

  clog << ListSet<Rational,Rectangle>(gms) << endl;

  ListSet<Rational,Rectangle> ls1,ls2;
  ls1.push_back(ls[0]);
  ls1.push_back(ls[2]);
  ls2.push_back(ls[1]);

  FiniteGrid<Rational> fg1(ls1);
  FiniteGrid<Rational> fg2(ls2);

  FiniteGrid<Rational> fgj(fg1,fg2);

  clog << fg1 << "\n" << fg2 << "\n" << fgj << "\n";
  clog << FiniteGrid<Rational>::index_translation(fg1,fgj) << "\n";
  clog << FiniteGrid<Rational>::index_translation(fg2,fgj) << "\n";

  GridRectangleListSet<Rational> grls1(ls1);
  GridRectangleListSet<Rational> grlsj1(fgj,grls1);
  clog << grlsj1 << "\n";

  GridRectangleListSet<Rational> grls2(ls2);
  GridRectangleListSet<Rational> grlsj2(fgj,grls2);
  clog << grlsj2 << "\n";

  GridCellListSet<Rational> gcls1(grls1);
  clog << gcls1 << "\n";
  GridRectangleListSet<Rational> grlsc1(fgj,gcls1);
  clog << grlsc1 << "\n";

  GridMaskSet<Rational> gms1(grlsj1);
  GridMaskSet<Rational> gms2(grlsj2);
  clog << regular_intersection(gms1,gms2);
  clog << join(gms1,gms2);

  clog.close();

  cout << "PASS\n";

  return 0;
}
