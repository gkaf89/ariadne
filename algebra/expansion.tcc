/***************************************************************************
 *            expansion.tcc
 *
 *  Copyright 2008-15  Pieter Collins
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


#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <boost/iterator.hpp>
#include <boost/iterator_adaptors.hpp>

#include "algebra/vector.h"
#include "algebra/multi_index.h"
#include "algebra/expansion.h"



namespace Ariadne {

template<class FwdIter, class Op>
FwdIter unique_key(FwdIter first, FwdIter last, Op op) {
    FwdIter curr=first;
    FwdIter next=curr;
    while(next!=last) {
        if(curr!=next) { *curr=*next; }
        ++next;
        while(next!=last && curr->key()==next->key()) {
            if(curr->key()==next->key()) {
                curr->data()=op(curr->data(),next->data());
                ++next;
            }
        }
        // Removes zero entries; the code below is preferred to the case "curr->data()!=0" for Tribool results
        if(curr->data()==0) { }
        else { ++curr; }
    }
    return curr;
}



template<class X> Expansion<X>::Expansion() : _argument_size() { }

template<class X> Expansion<X>::Expansion(unsigned int as) : _argument_size(as) { }

template<class X> Expansion<RawFloat>& Expansion<X>::raw() { return reinterpret_cast<Expansion<RawFloat>&>(*this); }

template<class X> Expansion<RawFloat>const& Expansion<X>::raw() const { return reinterpret_cast<Expansion<RawFloat>const&>(*this); }

template<class X> void Expansion<X>::swap(Expansion<X>& other) {
    std::swap(this->_argument_size,other._argument_size);
    std::swap(this->_coefficients,other._coefficients);
}

template<class X> bool Expansion<X>::operator==(const Expansion<X>& other) const {
    if(this->argument_size()!=other.argument_size()) { return false; }
    if(this->number_of_nonzeros()!=other.number_of_nonzeros()) { return false; }
    const_iterator self_iter=this->begin(); const_iterator other_iter=other.begin();
    while(self_iter!=this->end()) {
        if(self_iter->key()!=other_iter->key() || self_iter->data()!=other_iter->data()) { return false; }
        ++self_iter; ++other_iter; }
    return true;
}

template<class X> bool Expansion<X>::operator!=(const Expansion<X>& other) const { return !this->operator==(other); }

template<class X> SizeType Expansion<X>::argument_size() const { return this->_argument_size; }

template<class X> SizeType Expansion<X>::number_of_nonzeros() const { return _coefficients.size()/_element_size(); }

template<class X> DegreeType Expansion<X>::degree() const {
   DegreeType deg=0u; for(const_iterator iter=this->begin(); iter!=this->end(); ++iter) {
        deg=std::max(deg,iter->key().degree());
    }
    return deg;
}

template<class X> const std::vector<WordType>& Expansion<X>::coefficients() const { return this->_coefficients; }

template<class X> bool Expansion<X>::empty() const { return this->_coefficients.empty(); }
template<class X> unsigned int Expansion<X>::size() const { return this->_coefficients.size()/_element_size(); }
template<class X> void Expansion<X>::reserve(size_type nnz) { this->_coefficients.reserve(nnz*_element_size()); }
template<class X> void Expansion<X>::resize(size_type nnz) { this->_coefficients.resize(nnz*_element_size()); }



template<class X> void Expansion<X>::append(const MultiIndex& a, const RealType& c) {
    this->_append(a,c);
}
template<class X> void Expansion<X>::prepend(const MultiIndex& a, const RealType& c) {
    this->_prepend(a,c);
}
template<class X> void Expansion<X>::append(const MultiIndex& a1, const MultiIndex& a2, const RealType& c) {
    this->_append(a1,a2,c);
}

template<class X> const X& Expansion<X>::operator[](const MultiIndex& a) const {
    const_iterator iter=this->find(a);
    if(iter==this->end()) { return _zero; }
    else { return iter->data(); }
}

template<class X> typename Expansion<X>::iterator Expansion<X>::begin() { return iterator(_argument_size,_index_size(),_begin_ptr()); }
template<class X> typename Expansion<X>::iterator Expansion<X>::end() { return iterator(_argument_size,_index_size(),_end_ptr()); }
template<class X> typename Expansion<X>::iterator Expansion<X>::find(const MultiIndex& a) {
    iterator iter=this->end(); while(iter!=this->begin()) { --iter; if(iter->key()==a) { return iter; } } return this->end(); }

template<class X> typename Expansion<X>::const_iterator Expansion<X>::begin() const { return const_iterator(_argument_size,_index_size(),const_cast<word_type*>(_begin_ptr())); }
template<class X> typename Expansion<X>::const_iterator Expansion<X>::end() const { return const_iterator(_argument_size,_index_size(),const_cast<word_type*>(_end_ptr())); }
template<class X> typename Expansion<X>::const_iterator Expansion<X>::find(const MultiIndex& a) const {
    const_iterator iter=this->end(); while(iter!=this->begin()) { --iter; if(iter->key()==a) { return iter; } } return this->end();
}

template<class X> typename Expansion<X>::reference Expansion<X>::front() { return *(this->begin()); }
template<class X> typename Expansion<X>::reference Expansion<X>::back() { return *(--this->end()); }
template<class X> typename Expansion<X>::const_reference Expansion<X>::front() const { return *(this->begin()); }
template<class X> typename Expansion<X>::const_reference Expansion<X>::back() const { return *(--this->end()); }

template<class X> void Expansion<X>::erase(iterator iter) { iter->data()=static_cast<X>(0u); }
template<class X> void Expansion<X>::clear() { _coefficients.clear(); }

template<class X> void Expansion<X>::remove_zeros() {
    this->resize(std::remove_if(this->begin(),this->end(),DataIsZero())-this->begin()); }

template<class X> void Expansion<X>::combine_terms() {
    iterator curr=this->begin(); const_iterator adv=this->begin(); const_iterator end=this->end();
    while(adv!=end) { curr->key()=adv->key(); curr->data()=adv->data(); ++adv;
        while(adv!=end && curr->key()==adv->key()) { curr->data()+=adv->data(); ++adv; } ++curr; }
    this->resize(curr-this->begin()); }

template<> void Expansion<ExactFloat>::combine_terms() { ARIADNE_NOT_IMPLEMENTED; }
template<> void Expansion<ExactInterval>::combine_terms() { ARIADNE_NOT_IMPLEMENTED; }

template<class X> template<class CMP> void Expansion<X>::sort(const CMP& cmp) {
    std::sort(this->begin(),this->end(),cmp);
}

template<class X> void Expansion<X>::graded_sort() {
    std::sort(this->begin(),this->end(),GradedKeyLess());
}

template<class X> void Expansion<X>::lexicographic_sort() {
    std::sort(this->begin(),this->end(),LexicographicKeyLess());
}

template<class X> void Expansion<X>::reverse_lexicographic_sort() {
    std::sort(this->begin(),this->end(),ReverseLexicographicKeyLess());
}

template<class X> void Expansion<X>::check() const { }

template<class X> typename Expansion<X>::size_type Expansion<X>::_vector_size() const {
    return _coefficients.size(); }
template<class X> typename Expansion<X>::size_type Expansion<X>::_index_size() const {
    return (_argument_size+sizeof_word)/sizeof_word; }

template<class X> typename Expansion<X>::size_type Expansion<X>::_element_size() const {
    return (_argument_size+sizeof_word)/sizeof_word+sizeof_data/sizeof_word; }

template<class X> const typename Expansion<X>::word_type* Expansion<X>::_begin_ptr() const { return _coefficients.begin().operator->(); }
template<class X> const typename Expansion<X>::word_type* Expansion<X>::_end_ptr() const { return _coefficients.end().operator->(); }
template<class X> typename Expansion<X>::word_type* Expansion<X>::_begin_ptr() { return _coefficients.begin().operator->(); }
template<class X> typename Expansion<X>::word_type* Expansion<X>::_end_ptr() { return _coefficients.end().operator->(); }

template<class X> typename Expansion<X>::iterator Expansion<X>::_insert(iterator p, const MultiIndex& a, const RealType& x) {
        //std::cerr<<"_insert "<<*this<<" "<<p._ptr()<<" "<<a<<" "<<x<<std::endl;
        if(_coefficients.size()+_element_size()>_coefficients.capacity()) {
            difference_type i=p-begin();
            _coefficients.resize(_coefficients.size()+_element_size());
            p=begin()+i;
        } else {
            _coefficients.resize(_coefficients.size()+_element_size());
        }
        return _allocated_insert(p,a,x);
}

template<class X> typename Expansion<X>::iterator Expansion<X>::_allocated_insert(iterator p, const MultiIndex& a, const RealType& x) {
        //std::cerr<<"_allocated_insert "<<*this<<" "<<p<<" "<<p-begin()<<" "<<a<<" "<<x<<std::endl;
        iterator curr=this->end()-1; iterator prev=curr;
        while(curr!=p) { --prev; *curr=*prev; curr=prev; }
        curr->key()=a; curr->data()=x; return p; }

template<class X> void Expansion<X>::_prepend(const MultiIndex& a, const RealType& x) {
        //std::cerr<<"_prepend "<<*this<<" "<<a<<" "<<x<<std::endl;
        _coefficients.resize(_coefficients.size()+_element_size());
        _allocated_insert(begin(),a,x); }
template<class X> void Expansion<X>::_append(const MultiIndex& a, const RealType& x) {
        //std::cerr<<"_append "<<*this<<" "<<a<<" "<<x<<"... "<<std::flush;
        _coefficients.resize(_coefficients.size()+_element_size());
        word_type* vp=_end_ptr()-_element_size(); const word_type* ap=a.word_begin();
        for(unsigned int j=0; j!=_index_size(); ++j) { vp[j]=ap[j]; }
        data_type* xp=reinterpret_cast<data_type*>(this->_end_ptr())-1; *xp=x;
        //std::cerr<<"done"<<std::endl;
    }
template<class X> void Expansion<X>::_append(const MultiIndex&  a1, const MultiIndex&  a2, const RealType& x) {
        //std::cerr<<"_append "<<*this<<" "<<a1<<" "<<a2<<" "<<x<<std::endl;
        _coefficients.resize(_coefficients.size()+_element_size());
        word_type* vp=_end_ptr()-_element_size();
        const word_type* ap1=a1.word_begin(); const word_type* ap2=a2.word_begin();
        for(unsigned int j=0; j!=_index_size(); ++j) { vp[j]=ap1[j]+ap2[j]; }
        data_type* xp=reinterpret_cast<data_type*>(this->_end_ptr())-1; *xp=x; }

template<class X> X Expansion<X>::_zero = static_cast<X>(0);

template<class X>
Expansion<X>::Expansion(unsigned int as, unsigned int deg, std::initializer_list<X> lst)
    : _argument_size(as)
{
    MultiIndex a(as); X x;
    typename std::initializer_list<X>::const_iterator iter = lst.begin();
    while(a.degree()<=deg) {
        x=*iter;
        if(x!=X(0)) { this->append(a,x); }
        ++a;
        ++iter;
    }
}

template<class X>
Expansion<X>::Expansion(unsigned int as, std::initializer_list< std::pair<std::initializer_list<int>,X> > lst)
    : _argument_size(as)
{
    MultiIndex a;
    X x;
    for(typename std::initializer_list< std::pair<std::initializer_list<int>,X> >::const_iterator iter=lst.begin();
        iter!=lst.end(); ++iter)
    {
        a=MultiIndex(iter->first);
        x=iter->second;
        if(x!=X(0)) { this->append(a,x); }
    }
}

template<class X>
Expansion<X>::Expansion(std::initializer_list< std::pair<std::initializer_list<int>,X> > lst)
    : _argument_size(lst.size()==0?0u:lst.begin()->first.size())
{
    MultiIndex a;
    X x;
    for(typename std::initializer_list< std::pair<std::initializer_list<int>,X> >::const_iterator iter=lst.begin();
        iter!=lst.end(); ++iter)
    {
        a=iter->first;
        x=iter->second;
        if(x!=X(0)) { this->append(a,x); }
    }
}

template<class X> template<class XX>
Expansion<X>::Expansion(const std::map<MultiIndex,XX>& m)
{
    ARIADNE_ASSERT(!m.empty());
    this->_argument_size=m.begin()->first.size();
    for(typename std::map<MultiIndex,XX>::const_iterator iter=m.begin(); iter!=m.end(); ++iter) {
        this->append(iter->first,X(iter->second));
    }
}



template<class X> Expansion<X>& Expansion<X>::operator=(const X& c) {
    this->clear();
    this->append(MultiIndex::zero(this->argument_size()),c);
    return *this;
}

template<class X> Expansion<X> Expansion<X>::variable(unsigned int n, unsigned int i) {
    Expansion<X> p(n); p.append(MultiIndex::unit(n,i),X(1));
    return p;
}




template<class X>
Expansion<X> Expansion<X>::_embed(unsigned int before_size, unsigned int after_size) const
{
    const Expansion<X>& x=*this;
    uint old_size=x.argument_size();
    uint new_size=before_size+old_size+after_size;
    Expansion<X> r(new_size);
    MultiIndex old_index(old_size);
    MultiIndex new_index(new_size);
    for(typename Expansion<X>::const_iterator iter=x.begin(); iter!=x.end(); ++iter) {
        old_index=iter->key();
        for(uint j=0; j!=old_size; ++j) {
            uint aj=old_index[j];
            new_index[j+before_size]=aj;
        }
        r.append(new_index,iter->data());
    }
    return r;
}


template<class T> Expansion<MidpointType<T>> midpoint(const Expansion<T>& pse) {
    Expansion<MidpointType<T>> r(pse.argument_size());
    for(typename Expansion<T>::const_iterator iter=pse.begin(); iter!=pse.end(); ++iter) {
        r.append(iter->key(),midpoint(iter->data())); }
    return r;
}


template<class X>
std::ostream& Expansion<X>::write(std::ostream& os, const Array<std::string>& variable_names) const
{
    ARIADNE_ASSERT(this->argument_size()==variable_names.size());
    const Expansion<X>& p=*this;
    if(p.size()==0) {
        os << "0";
    } else {
        bool first_term=true;
        for(const_iterator iter=p.begin(); iter!=p.end(); ++iter) {
            MultiIndex a=iter->key();
            X v=iter->data();
            if(decide(v>=X(0)) && !first_term) { os<<"+"; }
            first_term=false;
            bool first_factor=true;
            if(decide(v<X(0))) { os<<"-"; }
            if(abs(v)!=X(1) || a.degree()==0) { os<<abs(v); first_factor=false; }
            for(uint j=0; j!=a.size(); ++j) {
                if(a[j]!=0) {
                    if(first_factor) { first_factor=false; } else { os <<"*"; }
                    os<<variable_names[j]; if(a[j]!=1) { os<<"^"<<int(a[j]); }
                }
            }
        }
    }
    return os;
}


template<class X>
std::ostream& Expansion<X>::write(std::ostream& os) const {
    const Expansion<X>& p=*this;
    Array<std::string> variable_names(p.argument_size());
    for(uint j=0; j!=p.argument_size(); ++j) {
        std::stringstream sstr;
        sstr << 'x' << j;
        variable_names[j]=sstr.str();
    }
    return p.write(os,variable_names);
}





template<class X> Vector< Expansion<X> > operator*(const Expansion<X>& e, const Vector<Float> v) {
    Vector< Expansion<X> > r(v.size(),Expansion<X>(e.argument_size()));
    for(uint i=0; i!=r.size(); ++i) {
        ARIADNE_ASSERT(v[i]==0.0 || v[i]==1.0);
        if(v[i]==1.0) { r[i]=e; }
    }
    return r;
}


template<class T>
inline Vector< Expansion<MidpointType<T>> > midpoint(const Vector< Expansion<T> >& pse) {
    Vector< Expansion<MidpointType<T>> > r(pse.size(),Expansion<MidpointType<T>>());
    for(uint i=0; i!=pse.size(); ++i) {
        r[i]=midpoint(pse[i]); }
    return r;
}








}

