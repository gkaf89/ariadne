/***************************************************************************
 *            integer.h
 *
 *  Copyright  2004-6  Alberto Casagrande, Pieter Collins
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
 
/*! \file integer.h
 *  \brief Multiple-precision integer type and interger functions.
 */

#ifndef ARIADNE_INTEGER_H
#define ARIADNE_INTEGER_H

#include <gmpxx.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../numeric/numerical_traits.h"
#include "../numeric/conversion.h"
#include "../numeric/arithmetic.h"

namespace Ariadne {
  namespace Numeric {

    /*!\ingroup Numeric
     * \brief An integer of arbitrary size.
     *  
     * An element of the ring of integers.
     * Must allow denotation of any integer, including arbitrarily large values.
     * Integer quotient and remainder must be supported.
     *
     * Currently implemented using mpz_class from the GNU Multiple Precision Library.
     */
    class Integer : public mpz_class 
    { 
     public:
      //@{
      //! \name Constructors and assignment operators
      /*! \brief Default constructor constructs the integer 0. */
      Integer() : mpz_class() { }
      /*! \brief Construct from a string literal. */
      Integer(const std::string& str);
      /*! \brief Copy constructor. */
      Integer(const Integer& z) : mpz_class(z) { }
      /*! \brief Copy assignment operator. */
      Integer& operator=(const Integer& z) { this->mpz_class::operator=(z); return *this; }

      /*! \brief Convert from another numerical type. */
      template<class N> Integer(const N& n) : mpz_class(n) { }
      /*! \brief Conversion assignment operator from another numerical type. */
      template<class N> Integer& operator=(const N& n) {
        (*this)=Integer(n); return *this; }
      //@}

#ifdef DOXYGEN
      //@{
      //! \name Arithmetic operations
      /*! \brief The minimum of z1 and z2. */
      friend Integer min(const Integer& z1, const Integer& z2);
      /*! \brief The maximum of z1 and z2. */
      friend Integer max(const Integer& z1, const Integer& z2);
      /*! \brief The absolute value \a z. */
      friend Integer abs(const Integer& z);
      
      /*! \brief In-place increment. */
      friend Integer& operator++(Integer& z);
      /*! \brief In-place addition. */
      friend Integer& operator+=(Integer& z1, const Integer& z2);
      /*! \brief In-place subtraction of a number. */
      friend Integer& operator-=(Integer& z1, const Integer& z2);
      /*! \brief In-place multiplication. */
      friend Integer& operator*=(Integer& z1, const Integer& z2);

      /*! \brief Negation. */
      friend Integer operator-(const Integer& z);
      /*! \brief Addition. */
      friend Integer operator+(const Integer& z1, const Integer& z2);
      /*! \brief Subtraction. */
      friend Integer operator-(const Integer& z1, const Integer& z2);
      /*! \brief Multiplication. */
      friend Integer operator*(const Integer& z1, const Integer& z2);
      /*! \brief Division. */
      friend Rational operator/(const Integer& z1, const Integer& z2);
      /*! \brief Power by a positive integer. */
      friend Integer pow(const Integer& z, const unsigned int& n);
      //@}
      
      
      //@{
      //! \name Comparison operators.
      /*! \brief Ezuality operator. */
      friend bool operator==(const Integer& z1, const Integer& z2); 
      /*! \brief Inezuality operator. */
      friend bool operator!=(const Integer& z1, const Integer& z2); 
      /*! \brief Less than operator. */
      friend bool operator<(const Integer& z1, const Integer& z2);  
      /*! \brief Greater than operator. */
      friend bool operator>(const Integer& z1, const Integer& z2);
      /*! \brief Less than or ezual to operator. */
      friend bool operator<=(const Integer& z1, const Integer& z2);
      /*! \brief Greater than or ezual to operator. */
      friend bool operator>=(const Integer& z1, const Integer& z2);
      //@}

      //@{
      //! \name Input/output operators.
      /*! \brief Stream insertion operator. */
      friend std::ostream& operator<<(std::ostream& os, const Integer& z);
      /*! \brief Stream extraction operator. */
      friend std::istream& operator>>(std::istream& is, Integer& z);
      //@}
#endif
    };
  
    template<> inline int convert_to<int>(const Numeric::Integer& n) { return n.get_si(); }
    template<> inline long convert_to<long>(const Numeric::Integer& n) { return n.get_si(); }
    
    template<> inline int conv_down<int>(const int& n) { return n; }
    template<> inline int conv_up<int>(const int& n) { return n; }
    
    inline Integer& operator++(Integer& i) {
      i=i+1; return i;
    }

  inline Integer::Integer(const std::string& str) { std::stringstream ss(str); ss >> *this; }

    //! \name %Integer arithmetic
    //@{
    //! \ingroup Numeric
    /*! \brief Minimum. */
    template<> inline Integer min(const Integer& n1, const Integer& n2) {
      //std::cerr << "min<" << name<R>() << ">" << std::endl;
      return n1<=n2 ? n1 : n2;
    }
  
    /*! \brief Maximum. */
    template<> inline Integer max(const Integer& n1, const Integer& n2) {
      //std::cerr << "min<" << name<R>() << ">" << std::endl;
      return n1>=n2 ? n1 : n2;
    }
     
    /*! \brief Absolute value. */
    template<> inline 
    Integer abs(const Integer& n) {
      return n>=0 ? n : static_cast<Integer>(-n);
    }
  
    /*! \brief Unary negation. */
    template<> inline
    Integer neg(const Integer& n) {
      return -n;
    }
    
    /*! \brief Addition. */
    template<> inline
    Integer add(const Integer& n1,const Integer& n2) {
      return n1+n2;
    }

    
    /*! \brief Subtraction. */
    template<> inline
    Integer sub(const Integer& n1,const Integer& n2) {
      return n1-n2;
    }

    
    /*! \brief Multiplication. */
    template<> inline
    Integer mul(const Integer& n1,const Integer& n2) {
      return n1*n2;
    }
    
    /*! \brief The power of an integer by an unsigned integer. */
    template<> inline 
    Integer pow(const Integer& n, const uint& i) {
      Integer r=1; Integer p=n; uint e=1;
      while(e<=i) { if(e&i) { r*=p; } p*=p; e*=2; }
      return r; 
    }

    /*! \brief The power of an integer by an unsigned integer. */
    template<> inline 
    uint pow(const uint& n, const uint& i) {
      uint r=1; uint p=n; uint e=1;
      while(e<=i) { if(e&i) { r*=p; } p*=p; e*=2; }
      return r; 
    }
    //@}
    
    //! \name %Integer functions
    //@{
    /*! \brief Factorial with mixed argument and result types. */
    /*! \brief Factorial. */
    template<class N> inline
    N factorial(const N& n) {
      N result=1;
      if(n<=0) { return 1; }
      for(N i=1; i!=n; ++i) { result*=i; }
      return result*n;
    }

    /*!\brief The number of ways of choosing \a k objects from \a n. */
    template<class N> inline 
    N choose(const N& n, const N& k) 
    {
      //std::cerr << "choose(" << n << "," << k << ")=" << std::flush;
      if(k==0 || k==n) { return 1; }
      if(n<0 || k<0 || k>n) { return 0; }
      N m=(k < n-k) ? k : static_cast<N>(n-k);
      N result=1;
      for(N i=n; i!=n-m; --i) { result*=i; }
      for(N i=m; i!=1; --i) { result/=i; }
      //std::cerr << result << std::endl;
      return result;
    }
    
    /*!\brief The number of ways of choosing \a k objects from \a n. */
    template<> inline 
    int choose<int>(const int& n, const int& k) 
    {
      //std::cerr << "choose<int>(" << n << "," << k << ")=" << std::flush;
      long result=1;
      if(k==0 || k==n) { }
      else if(n<0 || k<0 || k>n) { result=0; }
      else {
        int m=(k < n-k) ? k : n-k;
        for(int i=n; i!=n-m; --i) { result*=i; }
        for(int i=m; i!=1; --i) { result/=i; }
      }
      //std::cerr << result << std::endl;
      return result;
    }
    
    template<> inline 
    uint choose<uint>(const uint& n, const uint& k) 
    {
      //std::cerr << "choose<uint>(" << n << "," << k << ")=" << std::flush;
      ulong result=1;
      if(k==0 || k==n) { }
      else if(n<0 || k<0 || k>n) { result=0; }
      else {
        uint m=(k < n-k) ? k : (n-k);
        for(uint i=n; i!=n-m; --i) { result*=i; }
        for(uint i=m; i!=1; --i) { result/=i; }
      }
      //std::cerr << result << std::endl;
      return result;
    }
    
    /*! \brief Greatest common divisor. */
    template<class N> inline 
    N gcd(const N &a, const N &b) {
      N aa=a; N bb=b; N cc=aa%bb;
      while(cc!=0) { aa=bb; bb=cc; cc=aa%bb; }
      return bb;
    }

    /*! \brief Least common multiple. */
    template<class N> inline 
    N lcm(const N &a, const N &b) {
      return ((a*b)/gcd(a,b));
    }
    //@}

    //! \name %Integer bit-shift functions
    //@{
    /*! \brief The integer power \f$2^n\f$. */
    template<class N> inline
    N exp2(const N& n) {
      return 1<<n;
    }

    /*! \brief The floor of the logarithm of \a n in base 2. */
    template<class N> inline
    N log2_floor(const N& n) {
      if(n<1) { throw std::invalid_argument(__PRETTY_FUNCTION__); }
      N r=0;
      N y=n;
      while(y>=n) {
        y/=2;
        r+=1;
      }
      return r;
    }


    /*! \brief The ceiling of the logarithm of \a n in base 2. */
    template<class N> inline
    N log2_ceil(const N& n) {
      if(n<1) { throw std::invalid_argument(__PRETTY_FUNCTION__); }
      N r=0;
      N y=n;
      while(y>1) {
        y/=2;
        r+=1;
      }
      return r;
    }

    //@}
  }
}

#endif /* ARIADNE_INTEGER_H */
