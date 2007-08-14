/***************************************************************************
 *            clonable_container.h
 *
 *  Copyright  2007  Pieter Collins  pieter.collins@cwi.nl
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

#include <vector>
#include <set>
#include <map>

#include <stdexcept>

#include "container_utilities.h"

#ifndef ARIADNE_CLONABLE_CONTAINER_H
#define ARIADNE_CLONABLE_CONTAINER_H

namespace Ariadne {
  namespace Base {

    /*! \brief A vector whose elements are copied using a <code>clone()</code> method. */
    template< class T >
    class clonable_vector
    {
      typedef std::vector< T* > base_type;
     public:
      typedef typename base_type::size_type size_type;
      typedef T value_type;
      typedef T* pointer;
      typedef T& reference;
      typedef T const& const_reference;
      typedef boost::indirect_iterator<typename base_type::iterator> iterator;
      typedef boost::indirect_iterator<typename base_type::const_iterator> const_iterator;
      
      clonable_vector() : _vector() { }
      template<class Iter> clonable_vector(Iter first, Iter last)
        : _vector() { for( ; first!=last; ++first) { this->_vector.push_back(first->clone()); } }
      iterator begin() { return iterator(this->_vector.begin()); }
      iterator end() { return iterator(this->_vector.end()); }
      const_iterator begin() const { return const_iterator(this->_vector.begin()); }
      const_iterator end() const { return const_iterator(this->_vector.end()); }
      bool empty() const { return this->_vector.empty(); }
      size_type size() const { return this->_vector.size(); }
      size_type max_size() const { return this->_vector.max_size(); }
      size_type capacity() const { return this->_vector.capacity(); }
      size_type reserve(size_type n) { this->_vector.reserve(n); }
      // no resize() operation since no default constructor
      reference operator[](size_type i) { return *this->_vector[i]; }
      const_reference operator[](size_type i) const { return *this->_vector[i]; }
      reference at(size_type i) { return *this->_vector.at(i); }
      const_reference at(size_type i) const { return *this->_vector.at(i); }
      reference front() { return *this->_vector.front(); }
      reference back() { return *this->_vector.back(); }
      const_reference front() const { return *this->_vector.front(); }
      const_reference back() const { return *this->_vector.back(); }
      void push_back(reference t) { this->_vector.push_back(t.clone()); }
      void pop_back() { T* ptr = this->_vector.back(); this->_vector.pop_back(); delete ptr; }
      void clear() { 
        for(typename base_type::iterator iter=this->_vector.begin(); iter!=this->_vector.end(); ++iter) { T* ptr=*iter; delete ptr; } 
        this->_vector.clear(); }
     private:
      std::vector< T* > _vector;

    };

    /*! \brief A set whose elements are copied using a <code>clone()</code> method. */
    template< class Key, class Compare=std::less<Key> >
    class clonable_set
    {
      typedef std::set< Key const*, dereference_compare<Compare> > base_type;
     public:
      typedef typename base_type::size_type size_type;
      typedef Key key_type;
      typedef Key value_type;
      typedef Key const* pointer;
      typedef Key const& reference;
      typedef Key const& const_reference;
      typedef boost::indirect_iterator<typename base_type::iterator> iterator;
      typedef boost::indirect_iterator<typename base_type::const_iterator> const_iterator;
      
      const_iterator begin() const { return const_iterator(this->_set.begin()); }
      const_iterator end() const { return const_iterator(this->_set.end()); }
      bool empty() const { return this->_set.empty(); }
      size_type size() const { return this->_set.size(); }
      bool contains(Key const& k) const { 
        return this->_set.find(&k)!=this->_set.end(); }
      std::pair<const_iterator,bool> insert(Key const& k) { 
        typename base_type::iterator pos=this->_set.find(&k);
        if(pos==this->_set.end()) {
          std::pair<typename base_type::iterator, bool> base_insert=this->_set.insert(k.clone());
          return std::pair<const_iterator,bool>(base_insert.first,base_insert.second); 
        } else { 
          return std::pair<const_iterator,bool>(pos,false); } }
      template<class InputIterator> void insert(InputIterator first, InputIterator last) {
        for( ; first!=last; ++first) { this->insert(*first); } }
      size_type erase(const key_type& k) { 
        typename base_type::iterator iter=this->_set.find(&k); 
        Key const* ptr = *iter; this->_set.erase(iter); delete ptr; return 1; }
      void erase(const_iterator pos) { 
        Key const* ptr = pos.operator->(); this->_set.erase(pos.base()); delete ptr; }
      void erase(const_iterator first, const_iterator last) { 
        const_iterator next=first; while(first!=last) { ++next; this->erase(first); first=next; } }
      void clear() { 
        while(!this->empty()) { this->erase(this->begin()); } }
     private:
      std::set< Key const*, dereference_compare<Compare> > _set;

    };




    /*! \brief A map whose data are copied using a <code>clone()</code> method. */
    template< class Key, class Data, class Compare=std::less<Key> >
    class clonable_data_map
    {
      typedef std::map< Key, Data*, Compare > base_type;
      typedef _map_reference< clonable_data_map<Key,Data,Compare> > map_reference_class;
      friend class _map_reference< clonable_data_map<Key,Data,Compare> >;
     public:
      typedef typename base_type::size_type size_type;
      typedef Key key_type;
      typedef Data data_type;
      typedef _reference_data_map_value<Key,Data> value_type;
      typedef _reference_data_map_value<Key,Data>& reference;
      typedef const _reference_data_map_value<Key,Data>& const_reference;
      typedef _indirect_map_iterator<typename base_type::iterator,value_type,reference> iterator;
      typedef _indirect_map_iterator<typename base_type::const_iterator,value_type,const_reference> const_iterator;
      
      iterator begin() { return iterator(this->_map.begin()); }
      iterator end() { return iterator(this->_map.end()); }
      const_iterator begin() const { return const_iterator(this->_map.begin()); }
      const_iterator end() const { return const_iterator(this->_map.end()); }
      bool empty() const { return this->_map.empty(); }
      size_type size() const { return this->_map.size(); }
      iterator find(Key const& k) { return iterator(this->_map.find(k)); }
      const_iterator find(Key const& k) const { return const_iterator(this->_map.find(k)); }
      map_reference_class operator[](Key const& k) { return map_reference_class(*this,k); }
      const data_type& operator[](Key const& k) const { return this->_get(k); }
      bool has_key(Key const& k) const { return this->_map.find(k)!=this->_map.end(); }
      void insert(Key const& k, Data& d) { this->_map.insert(std::pair<Key,Data*>(k,d.clone())); }
      size_type erase(const key_type& k) { typename base_type::iterator pos = this->_map.find(k); delete pos->second; return this->_map.erase(k); }
      size_type erase(iterator iter) { Data* ptr=&iter->data(); this->_map.erase(iter.base()); delete ptr; }
      void clear() { while(!this->empty()) { this->erase(this->begin()); } }
     private:
      Data& _get(Key const& k) { iterator iter=this->find(k); if(iter!=this->end()) { return iter->data(); } throw std::out_of_range(); }
      Data const& _get(Key const& k) const { const_iterator iter=this->find(k); if(iter!=this->end()) { return iter->data(); } throw std::out_of_range(); }
      void _set(Key const& k, Data& x) { iterator iter=this->find(k); if(iter==this->end()) { this->insert(k,x); } else { iter->data()=x; } }
     private:
      base_type _map;
    };

  }
}

#endif // ARIADNE_CLONABLE_CONTAINER_H

