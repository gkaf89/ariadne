/***************************************************************************
 *            test_taylor_model.cc
 *
 *  Copyright 2008  Pieter Collins
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

#include <iostream>
#include <iomanip>
#include "numeric.h"
#include "vector.h"
#include "matrix.h"
#include "multi_index.h"
#include "differential.h"
#include "taylor_model.h"
#include "function.h"
#include "models.h"

#include "test.h"
using namespace Ariadne;

Vector<Float> v(uint n, uint i) { return Vector<Float>::unit(n,i); }
TaylorModel ctm(uint m, double c) { return TaylorModel::constant(m,c); }
TaylorModel tm(uint m, uint i) { return TaylorModel::variable(m,i); }


class TestTaylorModel
{
    typedef Expansion<Float> e;
  public:
    void test();
  private:
    void test_concept();
    void test_constructors();
    void test_predicates();
    void test_approximation();
    void test_evaluate();
    void test_arithmetic();
    void test_functions();
    void test_rescale();
    void test_restrict();
    void test_antiderivative();
    void test_compose();
    void test_flow();
    void test_solve();
    void test_implicit();
};


void TestTaylorModel::test()
{
    ARIADNE_TEST_CALL(test_constructors());
    ARIADNE_TEST_CALL(test_predicates());
    ARIADNE_TEST_CALL(test_approximation());
    ARIADNE_TEST_CALL(test_arithmetic());
    ARIADNE_TEST_CALL(test_functions());
    ARIADNE_TEST_CALL(test_rescale());
    ARIADNE_TEST_CALL(test_restrict());
    ARIADNE_TEST_CALL(test_antiderivative());
    ARIADNE_TEST_CALL(test_compose());
    ARIADNE_TEST_CALL(test_flow());
    ARIADNE_TEST_CALL(test_solve());
    //ARIADNE_TEST_CALL(test_implicit());
}


void TestTaylorModel::test_concept()
{
    const Float f=0.0;
    const Interval i;
    const Vector<Float> vf;
    const Vector<Interval> vi;
    const TaylorModel  t;
    TaylorModel tr;

    tr=t+f; tr=t-f; tr=t*f; tr=t/f;
    tr=f+t; tr=f-t; tr=f*t; tr=f/t;
    tr=t+i; tr=t-i; tr=t*i; tr=t/i;
    tr=i+t; tr=i-t; tr=i*t; tr=i/t;
    tr=t+t; tr=t-t; tr=t*t; tr=t/t;

    tr+=f; tr-=f; tr*=f; tr/=f;
    tr+=i; tr-=i; tr*=i; tr/=i;
    tr+=t; tr-=t;

    tr=exp(t); tr=log(t); tr=sqrt(t);
    tr=sin(t); tr=cos(t); tr=tan(t);
    //tr=asin(t); tr=acos(t); tr=atan(t);

    tr.sweep(); tr.truncate(); tr.clean();

    t.evaluate(vi); evaluate(t,vi);
    t.domain(); t.range(); t.expansion(); t.error();

}

void TestTaylorModel::test_constructors()
{
    ARIADNE_TEST_CONSTRUCT(TaylorModel,tv1,(e(2,3, 1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0), 0.25));

    ARIADNE_ASSERT_EQUAL(tv1.value(),1.0);
    ARIADNE_ASSERT_EQUAL(tv1.error(),0.25);
}

void TestTaylorModel::test_predicates()
{
    TaylorModel tv1(e(1,2, 1.00,2.00,3.00), 0.75);
    TaylorModel tv2(e(1,2, 1.00,1.75,3.25), 0.25);
    TaylorModel tv3(e(1,2, 1.125,1.75,3.25), 0.25);
    TaylorModel tv4(e(1,3, 1.00,2.25,3.00,-0.25), 0.25);

    ARIADNE_TEST_BINARY_PREDICATE(refines,tv1,tv1);
    ARIADNE_TEST_BINARY_PREDICATE(refines,tv2,tv1);
    ARIADNE_TEST_BINARY_PREDICATE(!refines,tv3,tv1);
    ARIADNE_TEST_BINARY_PREDICATE(refines,tv4,tv1);
}

void TestTaylorModel::test_approximation()
{
    ARIADNE_TEST_CONSTRUCT(TaylorModel,tv2,(e(1,2,1.0,2.0,3.0),0.25));
}

void TestTaylorModel::test_evaluate()
{
    Vector<Interval> iv(2, 0.25,0.5, -0.75,-0.5);
    TaylorModel tv(e(2,2,1.0,2.0,3.0,4.0,5.0,6.0),0.25);
    ARIADNE_TEST_EQUAL(evaluate(tv,iv),Interval(-1,1));
}

void TestTaylorModel::test_arithmetic()
{
    //Operations which can be performed exactly with floating-point arithmetic.
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)+(-3), TaylorModel(e(1,2, -2.0,-2.0,3.0), 0.75));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)-(-3), TaylorModel(e(1,2, 4.0,-2.0,3.0), 0.75));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)*(-3), TaylorModel(e(1,2, -3.0,6.0,-9.0), 2.25));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)/(-4), TaylorModel(e(1,2, -0.25,0.5,-0.75), 0.1875));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)+Interval(-1,2), TaylorModel(e(1,2, 1.5,-2.0,3.0), 2.25));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)-Interval(-1,2), TaylorModel(e(1,2, 0.5,-2.0,3.0), 2.25));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)*Interval(-1,2), TaylorModel(e(1,2, 0.5,-1.0,1.5), 10.5));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)/Interval(0.25,2.0), TaylorModel(e(1,2, 2.25,-4.5,6.75), 13.5));
    ARIADNE_TEST_EQUAL(+TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75), TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75));
    ARIADNE_TEST_EQUAL(-TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75), TaylorModel(e(1,2, -1.0,2.0,-3.0), 0.75));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)+TaylorModel(e(1,2, 3.0,2.0,-4.0), 0.5), TaylorModel(e(1,2, 4.0,0.0,-1.0), 1.25));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)-TaylorModel(e(1,2, 3.0,2.0,-4.0), 0.5), TaylorModel(e(1,2, -2.0,-4.0,7.0), 1.25));
    ARIADNE_TEST_EQUAL(TaylorModel(e(1,2, 1.0,-2.0,3.0), 0.75)*TaylorModel(e(1,2, 3.0,2.0,-4.0), 0.5), TaylorModel(e(1,4, 3.0,-4.0,1.0,14.0,-12.0), 10.125));
}

void TestTaylorModel::test_functions()
{
    TaylorModel xz(e(1,1, 0.0, 0.5), 0.0);
    TaylorModel xo(e(1,1, 1.0, 0.5), 0.0);

    //Functions based on their natural defining points
    ARIADNE_TEST_BINARY_PREDICATE(refines,exp(xz),TaylorModel(e(1,6, 1.00000,0.50000,0.12500,0.02083,0.00260,0.00026,0.00002), 0.00003));
    ARIADNE_TEST_BINARY_PREDICATE(refines,sin(xz),TaylorModel(e(1,6, 0.00000,0.50000,0.0000,-0.02083,0.00000,0.00026,0.00000), 0.00003));
    ARIADNE_TEST_BINARY_PREDICATE(refines,cos(xz),TaylorModel(e(1,6, 1.00000,0.0000,-0.12500,0.00000,0.00260,0.0000,-0.00002), 0.00003));

    ARIADNE_TEST_BINARY_PREDICATE(refines,rec(xo),TaylorModel(e(1,6,  1.000000,-0.500000, 0.250000,-0.125000, 0.062500,-0.031250, 0.015625), 0.018));
    ARIADNE_TEST_BINARY_PREDICATE(refines,sqrt(xo),TaylorModel(e(1,6, 1.000000, 0.250000,-0.031250, 0.007813,-0.002441, 0.000854,-0.000320), 0.0003));
    ARIADNE_TEST_BINARY_PREDICATE(refines,log(xo),TaylorModel(e(1,6,  0.000000, 0.500000,-0.125000, 0.041667,-0.015625, 0.006250,-0.002604), 0.003));

}


void TestTaylorModel::test_rescale()
{
}

void TestTaylorModel::test_restrict()
{
}


void TestTaylorModel::test_antiderivative()
{
    Interval unit_interval(-1,+1);
    TaylorModel tm=TaylorModel::constant(2,1.0);
    TaylorModel atm=antiderivative(tm,1);
}


void TestTaylorModel::test_compose()
{
}

template<class X> Vector<X> join(const X& x1, const X& x2) {
    Vector<X> r(2); r[0]=x1; r[1]=x2; return r;
}

Float norm(const TaylorModel& tm) {
    set_rounding_mode(upward);
    Float res=tm.error();
    for(TaylorModel::const_iterator iter=tm.begin(); iter!=tm.end(); ++iter) {
        res+=abs(iter->second);
    }
    return res;
}


namespace Ariadne {
Vector<TaylorModel> _implicit1(const Vector<TaylorModel>& f, uint n=4);
Vector<TaylorModel> _implicit2(const Vector<TaylorModel>& f, uint n=4);
Vector<TaylorModel> _implicit3(const Vector<TaylorModel>& f, uint n=4);
Vector<TaylorModel> _implicit4(const Vector<TaylorModel>& f, uint n=4);
Vector<TaylorModel> _implicit5(const Vector<TaylorModel>& f, uint n=4);
}

void TestTaylorModel::test_solve()
{
    TaylorModel f(e(1,3, 0,1.0, 1,4.0, 2,1.0),0.125);
    Interval x=solve(Vector<TaylorModel>(1u,f))[0];
    ARIADNE_TEST_PRINT(x);
    ARIADNE_TEST_PRINT(f.evaluate(Vector<Interval>(1u,x)));
}


void TestTaylorModel::test_implicit()
{
    std::cerr<<TaylorModel()<<"\n"<<TaylorModel(0)<<"\n"<<TaylorModel::constant(1,0.0)<<"\n";
    //TaylorModel f(e(2,3, 1,0,1.0, 0,1,4.0, 0,2,1.0),0.0);
    std::cerr<<std::setprecision(16);
    std::cout<<std::setprecision(16);
    std::clog<<std::setprecision(16);
    TaylorModel f(e(2,5, 0,0,0.0000002, 1,0,1.000000000000003, 2,0,0.000000000000003, 0,1,4.000000000000001, 0,2,1.000000000000001),0.0);
    //TaylorModel ha=implicit_approx(Vector<TaylorModel>(1u,f),8)[0];
    //TaylorModel h1=_implicit1(Vector<TaylorModel>(1u,f),4)[0];
    //TaylorModel h2=_implicit2(Vector<TaylorModel>(1u,f),4)[0];
    //TaylorModel h3=_implicit3(Vector<TaylorModel>(1u,f),4)[0];
    //std::cerr<<"\n\nh1="<<h1<<"\nh2="<<h2<<"\nh3="<<h3<<std::endl;
    TaylorModel h2=_implicit2(Vector<TaylorModel>(1u,f))[0];
    TaylorModel h5=_implicit5(Vector<TaylorModel>(1u,f))[0];
    ARIADNE_TEST_PRINT(h2);
    ARIADNE_TEST_PRINT(h5);
    return;
    TaylorModel h=implicit(f);
    TaylorModel id(e(1,1, 1,1.0),0.0);
    TaylorModel z(1);
    TaylorModel c=compose(f,join(id,h));
    TaylorModel hh=implicit_step(f,h);
    // Compute the power series for square root
    TaylorModel s(1); double cc=2; MultiIndex a(1);
    for(int i=1; i!=24; ++i) { cc*=(((2*i-3)*0.25)/(2*i)); ++a[0]; s[a]=cc; }
    ARIADNE_TEST_PRINT(s);


    ARIADNE_TEST_PRINT(f);
    ARIADNE_TEST_PRINT(h);
    ARIADNE_TEST_PRINT(s);
    ARIADNE_TEST_PRINT(id);
    ARIADNE_TEST_PRINT(join(id,h));
    ARIADNE_TEST_PRINT(compose(f,join(id,h)));
    ARIADNE_TEST_PRINT(h-s);
    ARIADNE_TEST_BINARY_PREDICATE(operator<,norm(c),1e-2);
    ARIADNE_TEST_BINARY_PREDICATE(operator<,norm(h-s),1e-4);
    ARIADNE_TEST_BINARY_PREDICATE(refines,hh,h);
    ARIADNE_TEST_BINARY_PREDICATE(refines,z,c);
    ARIADNE_TEST_BINARY_PREDICATE(refines,s,h);
    double he=h.error(); h.set_error(0);
    std::cerr<<"\n\n";
    std::cerr<<"hh="<<hh<<"\nh="<<h<<"\n";
    TaylorModel d=h-hh;
    std::cerr<<"h-hh="<<d<<"\n";
    std::cerr<<"|h-hh|="<<norm(h-hh)<<" he="<<he<<"\n\n\n";
}

namespace Ariadne {
Vector<Interval> range(const Vector<TaylorModel>& tm) {
    Vector<Interval> r(tm.size()); for(uint i=0; i!=tm.size(); ++i) { r[i]=tm[i].range(); } return r; }
}

void TestTaylorModel::test_flow()
{
    Vector<TaylorModel> vf=ctm(2,2.0)*v(2,0)+tm(2,1)*v(2,1);
    Vector<Interval> d(2, -0.5,0.5, -0.5,0.5);
    Interval h(-0.25,0.25);
    uint o=6;

    Vector<TaylorModel> phi=flow(vf,d,h,o);
    Vector<TaylorModel> next_phi=antiderivative(compose(vf,phi),2);
    ARIADNE_TEST_BINARY_PREDICATE(operator<,(norm(Ariadne::range(phi-next_phi))),0.1);
}


int main() {
    TestTaylorModel().test();
    std::cout << "INCOMPLETE " << std::flush;
    return ARIADNE_TEST_FAILURES;
}
