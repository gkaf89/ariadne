/***************************************************************************
 *            interval_vector.h
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
 
/*! \file interval_vector.h
 *  \brief Vectors of intervals.
  */

#ifndef _ARIADNE_INTERVAL_VECTOR_H
#define _ARIADNE_INTERVAL_VECTOR_H 

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include "../base/basic_type.h"
#include "../base/numerical_type.h"
#include "../base/interval.h"

namespace Ariadne {
  namespace LinearAlgebra {
    
    /*! \brief A vector of intervals. */
    template<typename R>
    class interval_vector
    {
     private:
      vector< Interval<R> > Base;
     public:
      template<typename E> interval_vector(const vector_expression<E> v) : Base(v()) { }
      interval_vector() : Base() { }
      interval_vector(const size_type& n) : Base(n) { }
      interval_vector(const vector<R>& v) : Base(v.size()) { 
        for(size_type i=0; i!=this->size(); ++i) {
          Base.operator()(i) = Interval<R>(v(i),v(i));
        }
      }
      interval_vector(const vector<R>& v, const R& r) : Base(v.size()) { 
        for(size_type i=0; i!=this->size(); ++i) {
          Base.operator()(i) = Interval<R>(v(i)-r,v(i)+r);
        }
      }
      size_type size() const { return Base.size(); }
      Interval<R>& operator() (const size_type& n) { return Base.operator()(n); }
      const Interval<R>& operator() (const size_type& n) const { return Base.operator()(n); }
    };
      
    template <typename R>
    inline
    interval_vector<R> 
    operator+(const interval_vector<R>& iv, const vector<R> v)
    {
      interval_vector<R> result(v.size());
      for(size_type i=0; i!=result.size(); ++i) {
        result(i)=v(i)+iv(i);
      }
      return result;
    }

    template <typename R>
    inline
    interval_vector<R> 
    operator+(const vector<R>& v, const interval_vector<R> iv)
    {
      interval_vector<R> result(v.size());
      for(size_type i=0; i!=result.size(); ++i) {
        result(i)=v(i)+iv(i);
      }
      return result;
    }

    template <typename R>
    inline
    interval_vector<R> 
    operator+(const interval_vector<R>& iv1, const interval_vector<R> iv2)
    {
      interval_vector<R> result(iv1.size());
      for(size_type i=0; i!=result.size(); ++i) {
        result(i)=iv1(i)+iv2(i);
      }
      return result;
    }

    template<typename R>
    inline
    interval_vector<R>
    operator*(const Interval<R>& s, const interval_vector<R>& v)
    {
      interval_vector<R> result(v.size());
      for(size_type i=0; i!=result.size(); ++i) {
        result(i)=s*v(i);
      }
      return result;
    }

    template<typename R>
    inline
    interval_vector<R>
    operator*(const R& s, const interval_vector<R>& v)
    {
      return Interval<R>(s)*v;
    }

    template<typename R>
    inline
    interval_vector<R>
    operator*(const Interval<R>& s, const vector<R>& v)
    {
      return s*interval_vector<R>(v); 
    }


    template<typename R>
    inline
    vector<R>
    centre(const interval_vector<R>& v)
    {
      vector<R> result(v.size());
      for (size_type i=0; i<v.size(); i++) {
        result(i)=(v(i).upper()+v(i).lower())/2;
      }
      return result;
    }
    
    template<typename R>
    inline
    R
    radius(const interval_vector<R>& v)
    {
      R diameter=0;
      for (size_type i=0; i<v.size(); i++) {
        diameter=max(diameter,R(v(i).upper()-v(i).lower()));
      }
      return diameter/2;
    }
    
    template<typename R>
    inline
    Interval<R>
    norm(const interval_vector<R>& v)
    {
      R lower_bound=0;
      R upper_bound=0;
      for (size_type i=0; i<v.size(); i++) {
        if(!(v(i).lower()<=0 && v(i).upper()>=0)) {
          lower_bound=std::min(lower_bound,std::min(abs(v(i).lower(),abs(v(i).upper()))));
        }
        upper_bound=std::max(upper_bound,std::max(abs(v(i).lower()),abs(v(i).upper())));
      }
      return Interval<R>(lower_bound,upper_bound);
    }
    
    template <typename R>
    inline
    std::ostream&
    operator<<(std::ostream& os, const interval_vector<R>& v)
    {
      os << "[";
      if(v.size()>0) {
        os << v(0);
      }
      for(uint i=1; i!=v.size(); ++i) {
        os << "," << v(i);
      }
      os << "]";
      return os;
    }
  }
}  

#endif /* _ARIADNE_INTERVAL_VECTOR_H */
