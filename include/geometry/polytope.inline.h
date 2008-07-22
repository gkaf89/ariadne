/***************************************************************************
 *            polytope.inline.h
 *
 *  Copyright  2005-6  Alberto Casagrande, Pieter Collins
 *  casagrande@dimi.uniud.it   Pieter.Collins@cwi.nl
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

namespace Ariadne {
  

    template<class X>
    class PolytopeVerticesIterator
      : public boost::iterator_facade<PolytopeVerticesIterator<X>,
                                      Point<X>,
                                      boost::forward_traversal_tag,
                                      Point<X> const&,
                                      Point<X> const*
                                     >
    {
     public:
      PolytopeVerticesIterator(const Polytope<X>& plytp, const size_type& j) 
        : _p(&plytp), _j(j) { }
      bool equal(const PolytopeVerticesIterator<X>& other) const {
        return this->_j==other._j && this->_p ==other._p; }
      void increment() {
        ++this->_j; }
      const Point<X>& dereference() const {
        this->_v=this->_p->vertex(this->_j); return this->_v; }
     private:
      const Polytope<X>* _p; size_type _j; mutable Point<X> _v;
    };
    

 
template<class X> template<class XX> inline
Polytope<X>::Polytope(const Polyhedron<XX>& p)
  : _dimension(p.dimension()), _number_of_vertices(), _data()
{   
  (*this)=polytope(Polyhedron<X>(p));
}

template<class X> template<class XX> inline
Polytope<X>::Polytope(const Polytope<XX>& p)
  : _dimension(p.dimension()), _number_of_vertices(p.number_of_vertices()), _data(p.data())
{ 
}

template<class X> template<class XX> inline
Polytope<X>&
Polytope<X>::operator=(const Polytope<XX>& p)
{
  if(this!=(void*)&p) { 
    this->_dimension=p.dimension();
    this->_number_of_vertices=p.number_of_vertices();
    this->_data=p.data();
  }
  return *this;
}


template<class X> inline
Box<typename Polytope<X>::real_type> 
bounding_box(const Polytope<X>& p)  
{
  return p.bounding_box();
}




template<class X> inline
std::ostream& 
operator<<(std::ostream& os, const Polytope<X>& p)
{
  return p.write(os); 
}


template<class X> inline
std::istream& 
operator>>(std::istream& os, Polytope<X>& p) 
{
  return p.read(os); 
}



} // namespace Ariadne
