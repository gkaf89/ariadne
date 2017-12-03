/***************************************************************************
 *            algebra/expansion.tpl.hpp
 *
 *  Copyright 2013-17  Pieter Collins
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

/*! \file algebra/expansion.tpl.hpp
 *  \brief
 */

#include <algorithm>
#include <exception>
#include <stdexcept>

#include "expansion.hpp"
#include "numeric/logical.hpp"

namespace Ariadne {

inline SizeType word_size(SizeType as) { return (1u+as)/sizeof(int)+1; }

inline double nul(double d) { return 0.0; }
inline double abs(double d) { return std::fabs(d); }


template<class X> Expansion<MultiIndex,X>::~Expansion()
{
    delete[] _indices;
    _indices=nullptr;
    delete[] _coefficients;
    _coefficients=nullptr;
}

template<class X> Expansion<MultiIndex,X>::Expansion(SizeType as)
    : Expansion<MultiIndex,X>(as,X()) {
}

template<class X> Expansion<MultiIndex,X>::Expansion(SizeType as, X const& z, SizeType cap)
    : _zero_coefficient(z), _capacity(cap), _size(0u), _argument_size(as)
    , _indices(new DegreeType[_capacity*(_argument_size+1)]), _coefficients(new CoefficientType[_capacity])
{
}


template<class X> Expansion<MultiIndex,X>::Expansion(InitializerList<Pair<InitializerList<DegreeType>,X>> lst)
    : Expansion( ( (ARIADNE_PRECONDITION(lst.size()!=0)) , Expansion(lst.begin()->first.size(),nul(lst.begin()->second)) ) )
{
    MultiIndex a;
    X x;
    for(auto iter=lst.begin();
        iter!=lst.end(); ++iter)
    {
        a=iter->first;
        x=iter->second;
        if(decide(x!=0)) { this->append(a,x); }
    }
}


//template<class X> Expansion<MultiIndex,X>::Expansion(InitializerList<Pair<InitializerList<DegreeType>,X>> lst) : Expansion(3) {
//    ARIADNE_PRECONDITION(lst.size()!=0);
//}

/*
// Call std::memcpy(dest,src,count) with same arguments as std::copy(src_begin,src_end,dest)
namespace std {
    template<> inline void copy(const unsigned char* b, const unsigned char* e, unsigned char* t) { memcpy(t,b,e-b); }
}
*/

template<class X> Expansion<MultiIndex,X>::Expansion(const Expansion<MultiIndex,X>& e)
    : _zero_coefficient(e._zero_coefficient), _capacity(e._capacity), _size(e._size), _argument_size(e._argument_size)
    , _indices(new DegreeType[_capacity*(_argument_size+1)]), _coefficients(new CoefficientType[_capacity])
{
    std::copy(e._indices,e._indices+_size*(_argument_size+1),_indices);
    std::copy(e._coefficients,e._coefficients+_size,_coefficients);
}

template<class X> Expansion<MultiIndex,X>& Expansion<MultiIndex,X>::operator=(const Expansion<MultiIndex,X>& e)
{
    if(this!=&e) {
        // Perform memory reallocation if necessary
        if(this->_capacity<e._size) {
            SizeType new_capacity=e._capacity;
            while(new_capacity/2>=e._size) { new_capacity/=2; }
            DegreeType* new_indices=new DegreeType[new_capacity*(e._argument_size+1)];
            CoefficientType* new_coefficients=new CoefficientType[new_capacity];
            _capacity=new_capacity;
            delete[] _indices;
            _indices=new_indices;
            delete[] _coefficients;
            _coefficients=new_coefficients;
        } else if(this->_argument_size!=e._argument_size) {
            DegreeType* new_indices=new DegreeType[this->_capacity*(e._argument_size+1)];
            delete[] _indices;
            _indices=new_indices;
        }
        _zero_coefficient=e._zero_coefficient;
        _size=e._size;
        _argument_size=e._argument_size;
        std::copy(e._indices,e._indices+_size*(_argument_size+1),_indices);
        std::copy(e._coefficients,e._coefficients+_size,_coefficients);
    }
    return *this;
}

template<class X> Expansion<MultiIndex,X>::Expansion(Expansion<MultiIndex,X>&& e)
    : _zero_coefficient(e._zero_coefficient), _capacity(e._capacity), _size(e._size), _argument_size(e._argument_size)
    , _indices(e._indices), _coefficients(e._coefficients)
{
    e._indices=nullptr;
    e._coefficients=nullptr;
}

template<class X> Expansion<MultiIndex,X>& Expansion<MultiIndex,X>::operator=(Expansion<MultiIndex,X>&& e)
{
    if(this!=&e) {
        assert(this->_indices!=e._indices);
        assert(this->_coefficients!=e._coefficients);

        _zero_coefficient=e._zero_coefficient;
        _capacity=e._capacity;
        _size=e._size;
        _argument_size=e._argument_size;

        delete[] _indices;
        _indices=e._indices;
        e._indices=nullptr;

        delete[] _coefficients;
        _coefficients=e._coefficients;
        e._coefficients=nullptr;
    }
    return *this;
}

template<class X> Void Expansion<MultiIndex,X>::swap(Expansion<MultiIndex,X>& other) {
    std::swap(this->_zero_coefficient,other._zero_coefficient);
    std::swap(this->_capacity,other._capacity);
    std::swap(this->_size,other._size);
    std::swap(this->_argument_size,other._argument_size);
    std::swap(this->_indices,other._indices);
    std::swap(this->_coefficients,other._coefficients);
}

template<class X> SizeType Expansion<MultiIndex,X>::number_of_terms() const {
    return this->size();
}

template<class X> SizeType Expansion<MultiIndex,X>::number_of_nonzeros() const {
    return this->size();
}

template<class X> Bool Expansion<MultiIndex,X>::empty() const {
    return this->_size==0;
}

template<class X> SizeType Expansion<MultiIndex,X>::size() const {
    return this->_size;
}

template<class X> SizeType Expansion<MultiIndex,X>::argument_size() const {
    return this->_argument_size;
}

template<class X> X const& Expansion<MultiIndex,X>::zero_coefficient() const {
    return this->_zero_coefficient;
}

template<class X> Void Expansion<MultiIndex,X>::reserve(SizeType new_capacity) {
    if(this->_capacity < new_capacity) {
        DegreeType* new_indices = new DegreeType[new_capacity*(this->_argument_size+1)];
        CoefficientType* new_coefficients = new CoefficientType[new_capacity];
        std::copy(this->_indices, this->_indices+this->_size*(this->_argument_size+1), new_indices);
        std::copy(this->_coefficients, this->_coefficients+this->_size, new_coefficients);
        this->_capacity=new_capacity;
        delete[] this->_indices;
        this->_indices=new_indices;
        delete[] this->_coefficients;
        this->_coefficients=new_coefficients;
    }
}

template<class X> Void Expansion<MultiIndex,X>::resize(SizeType new_size) {
    if(new_size<this->size()) {
        this->_size=new_size;
    } else {
        if(this->_capacity < new_size) {
            this->reserve(new_size);
        }
        MultiIndex a(this->argument_size());
        X c=_zero_coefficient;
        for (SizeType i=this->_size; i!=new_size; ++i) {
            this->append(a,c);
        }
    }
}

template<class X> SizeType Expansion<MultiIndex,X>::capacity() const {
    return this->_capacity;
}

template<class X> Void Expansion<MultiIndex,X>::clear() {
    this->_size=0;
}

template<class X> Void Expansion<MultiIndex,X>::remove_zeros() {
    this->resize(std::remove_if(this->begin(),this->end(),CoefficientIsZero())-this->begin());
}

template<class X, class Y> struct CanInplaceAdd {
    template<class XX, class YY, class = decltype(declval<XX&>()+=declval<YY>())> static True test(int);
    template<class XX, class YY> static False test(...);
    static const bool value = decltype(test<X,Y>(1))::value;
};

template<class X, EnableIf<CanInplaceAdd<X,X>> =dummy> Void combine_terms(Expansion<MultiIndex,X>& e) {
    auto begin=e.begin();
    auto end=e.end();
    auto curr=begin;
    auto adv=begin;
    while (adv!=end) {
        curr->index()=adv->index();
        curr->coefficient()=adv->coefficient();
        ++adv;
        while (adv!=end && adv->index()==curr->index()) {
            curr->coefficient() += adv->coefficient();
            ++adv;
        }
        ++curr;
    }
    e.resize(curr-begin);
}

template<class X, DisableIf<CanInplaceAdd<X,X>> =dummy> Void combine_terms(Expansion<MultiIndex,X>& e) {
    ARIADNE_ASSERT_MSG(false, "Cannot combine terms of an expansion if the coefficients do not support inplace addition.");
}

template<class X> Void Expansion<MultiIndex,X>::combine_terms() {
    Ariadne::combine_terms(*this);
}

template<class X> Void Expansion<MultiIndex,X>::check() const {
    ARIADNE_NOT_IMPLEMENTED;
}

template<class X> ExpansionValueReference<MultiIndex,X> Expansion<MultiIndex,X>::operator[](const MultiIndex& a) {
    return ExpansionValueReference<MultiIndex,X>(*this,a);
}

template<class X> const X& Expansion<MultiIndex,X>::operator[](const MultiIndex& a) const {
    return this->get(a);
}

template<class X> X& Expansion<MultiIndex,X>::at(const MultiIndex& a) {
    auto iter=this->find(a);
    if(iter==this->end()) { this->append(a,this->_zero_coefficient); iter=this->end()-1; }
    return iter->coefficient();
}

template<class X> Void Expansion<MultiIndex,X>::set(const MultiIndex& a, const X& c) {
    auto iter=this->find(a);
    if(iter==this->end()) { this->append(a,c); }
    else { iter->coefficient() = c; }
}

template<class X> const X& Expansion<MultiIndex,X>::get(const MultiIndex& a) const {
    auto iter=this->find(a);
    if(iter==this->end()) { return this->_zero_coefficient; }
    else { return iter->coefficient(); }
}

template<class X> Bool Expansion<MultiIndex,X>::operator==(const Expansion<MultiIndex,X>& other) const {
    auto iter1=this->begin();
    auto iter2=other.begin();
    auto end1=this->end();
    auto end2=other.end();

    if(this->size()!=other.size()) { return false; }
    if(this->argument_size()!=other.argument_size()) { return false; }

    while(true) {
        if(iter1!=end1 && iter2!=end2) {
            if(!decide(*iter1 == *iter2)) { return false; }
            ++iter1; ++iter2;
        } else if (iter1==end1 && iter2==end2) {
            return true;
        } else {
            return false;
        }
    }
}

template<class X> Bool Expansion<MultiIndex,X>::operator!=(const Expansion<MultiIndex,X>& other) const {
    return !(*this==other);
}

template<class X> auto Expansion<MultiIndex,X>::insert(Iterator pos, const MultiIndex& a, const X& c) -> Iterator {
    if(this->size()==this->capacity()) {
        SizeType where=pos-this->begin();
        this->append(a,c);
        pos=this->begin()+where;
    } else {
        this->append(a,c);
    }
    auto curr=this->end();
    auto prev=curr-1;
    while(prev!=pos) {
        --curr;
        --prev;
        *curr=*prev;
    }
    pos->index()=a;
    pos->coefficient()=c;
    return pos;
}

template<class X> auto Expansion<MultiIndex,X>::erase(Iterator pos) -> Iterator {
    auto curr=pos;
    auto next=curr;
    ++next;
    while(next!=this->end()) {
        *curr=*next;
        ++curr;
        ++next;
    }
    --this->_size;
    return pos;
}

template<class X> Void Expansion<MultiIndex,X>::prepend(const MultiIndex& a, const X& c) {
    this->append(a,c);
    auto curr=this->end();
    auto prev=curr-1;
    while(prev!=this->begin()) {
        --curr;
        --prev;
        *curr=*prev;
    }
    --curr;
    curr->index()=a;
    curr->coefficient()=c;
}

template<class X> Void Expansion<MultiIndex,X>::append(const MultiIndex& a, const X& c) {
    if(this->number_of_terms()==this->capacity()) {
        this->reserve(2*this->capacity());
    }
    DegreeType* _p=this->_indices+(this->_size)*(this->_argument_size+1);
    for(SizeType j=0; j!=this->_argument_size; ++j) { _p[j]=a[j]; }
    _p[this->_argument_size]=a.degree();
    _coefficients[this->_size]=c;
    ++this->_size;
}

template<class X> Void Expansion<MultiIndex,X>::append_sum(const MultiIndex& a1, const MultiIndex& a2, const X& c) {
    if(this->number_of_terms()==this->capacity()) {
        this->reserve(2*this->capacity());
    }
    DegreeType* _p=this->_indices+(this->_size)*(this->_argument_size+1);
    for(SizeType j=0; j<=this->_argument_size; ++j) { _p[j]=a1[j]+a2[j]; }
    _coefficients[this->_size]=c;
    ++this->_size;
}

template<class X> ExpansionIterator<MultiIndex,X> Expansion<MultiIndex,X>::begin() {
    return ExpansionIterator<MultiIndex,X>(_argument_size,_indices,_coefficients);
}

template<class X> ExpansionIterator<MultiIndex,X> Expansion<MultiIndex,X>::end() {
    return ExpansionIterator<MultiIndex,X>(_argument_size,_indices+_size*(_argument_size+1u),_coefficients+_size);
}

template<class X> ExpansionConstIterator<MultiIndex,X> Expansion<MultiIndex,X>::begin() const {
    return ExpansionConstIterator<MultiIndex,X>(_argument_size,const_cast<DegreeType*>(_indices),_coefficients);
}

template<class X> ExpansionConstIterator<MultiIndex,X> Expansion<MultiIndex,X>::end() const {
    return ExpansionConstIterator<MultiIndex,X>(_argument_size,const_cast<DegreeType*>(_indices)+_size*(_argument_size+1u),_coefficients+_size);
}

template<class X> ExpansionIterator<MultiIndex,X> Expansion<MultiIndex,X>::find(const MultiIndex& a) {
    ExpansionIterator<MultiIndex,X> iter=this->begin();
    ExpansionIterator<MultiIndex,X> end=this->end();
    while(iter!=end && iter->index()!=a) {
        ++iter;
    }
    return iter;
}

template<class X> ExpansionConstIterator<MultiIndex,X> Expansion<MultiIndex,X>::find(const MultiIndex& a) const {
    ExpansionConstIterator<MultiIndex,X> iter=this->begin();
    ExpansionConstIterator<MultiIndex,X> end=this->end();
    while(iter!=end && iter->index()!=a) {
        ++iter;
    }
    return iter;
}

template<class CMP> struct IndexComparison {
    CMP cmp;
    template<class M1, class M2> auto operator()(M1 const& m1, M2 const& m2) -> decltype(cmp(m1.index(),m2.index())) {
        return cmp(m1.index(),m2.index()); }
};

template<class X> Void Expansion<MultiIndex,X>::index_sort(ReverseLexicographicLess) {
    std::sort(this->begin(),this->end(),IndexComparison<ReverseLexicographicLess>());
}

template<class X> Void Expansion<MultiIndex,X>::index_sort(GradedLess) {
    std::sort(this->begin(),this->end(),IndexComparison<GradedLess>());
}

template<class X> Void Expansion<MultiIndex,X>::sort(ReverseLexicographicIndexLess) {
    std::sort(this->begin(),this->end(),IndexComparison<ReverseLexicographicLess>());
}

template<class X> Void Expansion<MultiIndex,X>::sort(GradedIndexLess) {
    std::sort(this->begin(),this->end(),IndexComparison<GradedLess>());
}

template<class X> Void Expansion<MultiIndex,X>::reverse_lexicographic_sort() {
    std::sort(this->begin(),this->end(),IndexComparison<ReverseLexicographicLess>());
}

template<class X> Void Expansion<MultiIndex,X>::graded_sort() {
    std::sort(this->begin(),this->end(),IndexComparison<GradedLess>());
}


template<class X>
Expansion<MultiIndex,X> Expansion<MultiIndex,X>::_embed(SizeType before_size, SizeType after_size) const
{
    const Expansion<MultiIndex,X>& x=*this;
    SizeType old_size=x.argument_size();
    SizeType new_size=before_size+old_size+after_size;
    Expansion<MultiIndex,X> r(new_size, x._zero_coefficient, x._capacity);
    MultiIndex old_index(old_size);
    MultiIndex new_index(new_size);
    for(typename Expansion<MultiIndex,X>::ConstIterator iter=x.begin(); iter!=x.end(); ++iter) {
        old_index=iter->key();
        for(Nat j=0; j!=old_size; ++j) {
            Nat aj=old_index[j];
            new_index[j+before_size]=aj;
        }
        r.append(new_index,iter->data());
    }
    return r;
}

template<class X> OutputStream& Expansion<MultiIndex,X>::write(OutputStream& os) const {
    os << "Expansion<MultiIndex," << class_name<X>() <<">";
    os << "{"<<this->number_of_terms()<<"/"<<this->capacity()<<","<<this->argument_size()<<"\n";
    for(auto iter=this->begin(); iter!=this->end(); ++iter) {
        os << "  "<<iter->index()<<":"<<iter->coefficient()<<",\n";
    }
    os << "}\n";
    return os;
}

template<class X>
OutputStream& Expansion<MultiIndex,X>::write(OutputStream& os, const Array<String>& variable_names) const
{
    const Expansion<MultiIndex,X>& p=*this;
    ARIADNE_ASSERT(p.argument_size()==variable_names.size());
    if(p.size()==0) {
        os << "0";
    } else {
        bool first_term=true;
        for(auto iter=p.begin(); iter!=p.end(); ++iter) {
            MultiIndex a=iter->index();
            X v=iter->coefficient();
            os << " ";
            if(decide(v>=0) && !first_term) { os<<"+"; }
            first_term=false;
            bool first_factor=true;
            if(decide(v<0)) { os<<"-"; }
            if(possibly(abs(v)!=1) || a.degree()==0) { os<<abs(v); first_factor=false; }
            for(SizeType j=0; j!=a.size(); ++j) {
                if(a[j]!=0) {
                    if(first_factor) { first_factor=false; } else { os <<"*"; }
                    os<<variable_names[j]; if(a[j]!=1) { os<<"^"<<int(a[j]); }
                }
            }
        }
    }
    return os;
}

template<class I, class X, class CMP> SortedExpansion<I,X,CMP>::SortedExpansion(Expansion<I,X> e)
    : Expansion<I,X>(std::move(e))
{
    this->sort();
}

/*
template<class I, class X, class CMP> auto SortedExpansion<I,X,CMP>::find(const I& a) -> Iterator {
    ExpansionValue<I,X> term(a,Expansion<I,X>::_zero_coefficient);
    return  std::lower_bound(this->begin(),this->end(),a,CMP());
}

template<class I, class X, class CMP> auto SortedExpansion<I,X,CMP>::find(const I& a) const -> ConstIterator {
    ExpansionValue<I,X> term(a,Expansion<I,X>::_zero_coefficient);
    return std::lower_bound(this->begin(),this->end(),a,CMP());
}
*/
template<class I, class X, class CMP> auto SortedExpansion<I,X,CMP>::get(const I& a) const -> CoefficientType const& {
    ExpansionValue<I,X> term(a,Expansion<I,X>::_zero_coefficient);
    auto iter=std::lower_bound(this->begin(),this->end(),term,CMP());
    if (iter==this->end() || iter->index()!=a) {
        return this->_zero_coefficient;
    } else {
        return iter->coefficient();
    }
}

template<class I, class X, class CMP> auto SortedExpansion<I,X,CMP>::at(const I& a) -> CoefficientType& {
    ExpansionValue<I,X> term(a,Expansion<I,X>::_zero_coefficient);
    auto iter=std::lower_bound(this->begin(),this->end(),term,CMP());
    if (iter==this->end() || iter->index()!=a) {
        iter=this->Expansion<I,X>::insert(iter,a,X(0));
    }
    return iter->coefficient();
}

template<class I, class X, class CMP> Void SortedExpansion<I,X,CMP>::insert(const I& a, const X& c) {
    ExpansionValue<I,X> term(a,Expansion<I,X>::_zero_coefficient);
    auto iter=std::lower_bound(this->begin(),this->end(),term,CMP());
    if (iter==this->end() || iter->index()!=a) {
        iter=this->Expansion<I,X>::insert(iter,a,c);
    } else {
        ARIADNE_THROW(std::runtime_error,"Expansion<I,X>::set(const I& a, const X& c):\n    e="<<*this,
                      " Index "<<a<<" already has a coefficient.");
    }
}

template<class I, class X, class CMP> void SortedExpansion<I,X,CMP>::set(const I& a, CoefficientType const& c) {
    this->at(a)=c;
}

template<class I, class X, class CMP> void SortedExpansion<I,X,CMP>::sort() {
    std::sort(this->begin(),this->end(),CMP());
}


} // namespace Ariadne
