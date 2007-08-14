/***************************************************************************
 *            test_txtfstream.cc
 *
 *  22 June 2007
 *  Copyright  2007  Pieter Collins, Alberto Casagrande, Davide Bresolin
 *  Email Pieter.Collins@cwi.nl, casagrande@dimi.uniud.it, bresolin@sci.univr.it
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
#include <sstream>
#include <string>

#include "ariadne.h"
#include "test_float.h"

#include "base/utility.h"
#include "geometry/point.h"
#include "geometry/rectangle.h"
#include "geometry/zonotope.h"
#include "geometry/list_set.h"
#include "output/txtfstream.h"

#include "test.h"

using namespace Ariadne;
using namespace Ariadne::Numeric;
using namespace Ariadne::LinearAlgebra;
using namespace Ariadne::Geometry;
using namespace Ariadne::Output;
using namespace std;

int main() {

  Rectangle<Float> bbox(2);

  Point<Float> pt("(0.0,0.0)");

  Rectangle<Float> r1,r2,r3,r4;
  string input("[-0.125,1.125]x[-0.25, 3.25] "
               "[ 0.0125,1.0]x[0.0,2.0] "
               "[ 0.5,1.0]x[1.0,3.0] "
               "[ 0,0.3333333]x[2.3333,3] "
               "[ 0.06125,0.125]x[0.5,2.75] "
               );
  stringstream iss(input);

  iss >> bbox >> r1 >> r2 >> r3 >> r4;
  Zonotope<Float> z3(r3);
  Polytope<Float> p4(r4);
  
  cout << bbox << "\n";
  cout << r1 << "\n" << r2 << endl;
  cout << r3 << "\n" << r4 << endl;
  cout << z3 << "\n" << p4 << endl;
  cout << r1.vertices() << endl;
  cout << r2.vertices() << endl;
  cout << z3.vertices() << endl;
  cout << p4.vertices() << endl;
  cout << endl;
  
  
  // Test output of basic sets
  txtfstream txt;
  txt.open("test_txtfstream-1.txt");
  txt << r1;
  txt << r2;
  txt << z3 << p4;
  txt << pt;
  txt.close();

  cout << endl;

  // Test output of grid mask set
  Rectangle<Float> bb("[0,1]x[0,1]x[0,1]");
  Grid<Float> g(Vector<Float>("[0.25,0.25,0.25]"));
  GridMaskSet<Float> gms(g,bb);
  Rectangle<Float> r("[0.33,0.66]x[0.125,0.375]x[0.25,0.75]");
  cout << "gms.size()=" << gms.size() << endl;
  gms.adjoin_outer_approximation(r);
  cout << "gms.size()=" << gms.size() << endl;
  txt.open("test_txtfstream-2.txt");
  txt << gms;
  txt.close();
  
  return 0;
}
