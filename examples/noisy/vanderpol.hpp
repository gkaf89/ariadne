/***************************************************************************
 *            vanderpol.hpp
 *
 *  Copyright  2008-18 Luca Geretti
 *
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

#include "ariadne.hpp"

using namespace Ariadne;


Tuple<String,DottedRealAssignments,RealVariablesBox,RealVariablesBox,Real,double> VP()
{
    RealVariable x("x"), y("y"), u1("u1"), u2("u2");
    DottedRealAssignments dynamics={dot(x)=y+u1,dot(y)=-x+y*(1-pow(x,2))+u2};
    RealVariablesBox inputs={-1/20_q<=u1<=1/20_q,-1/10000_q<=u2<=1/10000_q};

    Real e=1/1024_q;
    RealVariablesBox initial={{1.21_dec-e<=x<=1.21_dec+e},{2.01_dec-e<=y<=2.01_dec+e}};

    Real evolution_time=24/4_q;
    double step=1.0/8;

    return make_tuple("VP",dynamics,inputs,initial,evolution_time,step);
}
