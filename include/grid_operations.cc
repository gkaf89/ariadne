/***************************************************************************
 *            grid_operations.cc
 *
 *  22 June 2005
 *  Copyright  2005  Alberto Casagrande, Pieter Collins
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

#include "grid_operations.h"

namespace Ariadne {
  namespace Geometry {

    class PositionIterator {
     public:
      PositionIterator(const IndexArray& l, const IndexArray& u)
        : _lower(l), _upper(u), _position(l) { }
      PositionIterator(const IndexArray& l, const IndexArray& u, const IndexArray& p)
        : _lower(l), _upper(u), _position(p) { }
      const IndexArray& operator*() const { return _position; }
      PositionIterator& operator++() {
        dimension_type d=0;
        _position[d]+=1;
        while(_position[d]==_upper[d] && (d+1u)!=_position.size() ) {
          _position[d]=_lower[d];
          d+=1;
          _position[d]+=1;
        }
        return *this;
      }
      bool end() const { return _position[dimension()-1]==_upper[dimension()-1]; }
     private:
      dimension_type dimension() const { return _position.size(); }
      const IndexArray& _lower;
      const IndexArray& _upper;
      IndexArray _position;
    };

    /* Inner product of two positive arrays */
    inline
    size_type
    inner_product(const array<size_type>& a1, const array<size_type>& a2)
    {
      size_type result=0;
      for(array<size_type>::size_type i=0; i!=a1.size(); ++i) {
        result += (a1[i] * a2[i]);
      }
      return result;
    }

    /* Compute a positive offset from two index sets */
    inline
    SizeArray
    operator-(const IndexArray& u, const IndexArray& l)
    {
      SizeArray result(l.size());
      for(dimension_type i=0; i!=result.size(); ++i) {
        assert(l[i]<=u[i]);
        result[i]=size_type(u[i]-l[i]);
      }
      return result;
    }

    inline
    BooleanArray&
    operator&=(BooleanArray& v1, const BooleanArray& v2)
    {
      assert(v1.size()==v2.size());
      typedef BooleanArray::const_iterator const_iterator;
      typedef BooleanArray::iterator iterator;

      iterator v1_iter=v1.begin();
      const_iterator v2_iter=v2.begin();
      iterator v1_end=v1.end();

      while(v1_iter!=v1_end) {
        (*v1_iter) = ( (*v1_iter) & (*v2_iter) );
        ++v1_iter;
        ++v2_iter;
      }
      return v1;
    }

    inline
    BooleanArray&
    operator|=(BooleanArray& v1, const BooleanArray& v2)
    {
      assert(v1.size()==v2.size());
      typedef BooleanArray::const_iterator const_iterator;
      typedef BooleanArray::iterator iterator;

      iterator v1_iter=v1.begin();
      const_iterator v2_iter=v2.begin();
      iterator v1_end=v1.end();

      while(v1_iter!=v1_end) {
        (*v1_iter) = ( (*v1_iter) | (*v2_iter) );
        ++v1_iter;
        ++v2_iter;
      }
      return v1;
    }

    inline
    BooleanArray&
    operator-=(BooleanArray& v1, const BooleanArray& v2)
    {
      assert(v1.size()==v2.size());
      typedef BooleanArray::const_iterator const_iterator;
      typedef BooleanArray::iterator iterator;

      iterator v1_iter=v1.begin();
      const_iterator v2_iter=v2.begin();
      iterator v1_end=v1.end();

      while(v1_iter!=v1_end) {
        (*v1_iter) = ( (*v1_iter) & (!(*v2_iter)) );
        ++v1_iter;
        ++v2_iter;
      }
      return v1;
    }

    BooleanArray
    operator&(const BooleanArray& v1, const BooleanArray& v2)
    {
      BooleanArray result(v1);
      return result&=v2;
    }

    BooleanArray
    operator|(const BooleanArray& v1, const BooleanArray& v2)
    {
      BooleanArray result(v1);
      return result|=v2;
    }

    BooleanArray
    operator-(const BooleanArray& v1, const BooleanArray& v2)
    {
      BooleanArray result(v1);
      return result-=v2;
    }

    inline
    bool
    operator<(const IndexArray& s1, const IndexArray& s2)
    {
      assert(s1.size() == s2.size());
      for(dimension_type i=0; i!=s1.size(); ++i) {
        if(s1[i]<s2[i]) {
          return true;
        }
        if(s1[i]>s2[i]) {
          return false;
        }
      }
      return false;
    }

    /* Check ordering of two cells */
    inline
    bool
    operator<(const IntegerCellList::const_reference& c1, const IntegerCellList::const_reference& c2)
    {
      assert(c1.size() == c2.size());
      for(dimension_type i=0; i!=c1.size(); ++i) {
        if(c1[i]<c2[i]) {
          return true;
        }
        if(c1[i]>c2[i]) {
          return false;
        }
      }
      return false;
    }

    /* Compute a positive offset from two index sets */
    inline
    size_type
    compute_index(const IndexArray& pos, const IndexArray& lower, const SizeArray& strides)
    {
      dimension_type dim=pos.size();
      size_type result=0;
      for(dimension_type i=0; i!=dim; ++i) {
        result += size_type(pos[i]-lower[i])*strides[i];
      }
      return result;
    }

    /* Compute the position of an index in a grid. */
    inline
    IndexArray
    compute_position(size_type index, const IndexArray& lower, const SizeArray& strides)
    {
      dimension_type dim=lower.size();
      IndexArray result(dim);
      for(dimension_type i=dim-1; i!=0; --i) {
        result[i] = index/strides[i];
        index = index%strides[i];
      }
      result[0]=index;
      return result;
    }

    /* Compute upper and lower bounds of the cell list cl. */
    inline
    void
    compute_cell_list_bounds(IndexArray* lptr, IndexArray* uptr, IntegerCellList cl)
    {
      assert(!cl.empty());
      dimension_type d = cl[0].size();

      IndexArray& l(*lptr);
      IndexArray& u(*uptr);

      l=cl[0];
      u=cl[0];
      for(dimension_type j=0; j!=d; ++j) {
        l[j]=cl[0][j];
        u[j]=cl[0][j];
      }

      for(size_type i=1; i!=cl.size(); ++i) {
        for(dimension_type j=0; j!=d; ++j) {
          l[j] = std::min(l[j],cl[i][j]);
          u[j] = std::max(u[j],cl[i][j]);
        }
      }

      for(dimension_type j=0; j!=d; ++j) {
        u[j]+=1;
      }

      return;
    }

    /* Compute strides from a list of sizes. */
    inline
    SizeArray
    compute_strides(const SizeArray& s) {
      SizeArray result(s.size()+1);
      result[0]=1;
      for(dimension_type i=0; i!=s.size(); ++i) {
        result[i+1]=s[i]*result[i];
      }
      return result;
    }

    /* Compute upper and lower bounds of the cell list cl. */
    inline
    void
    compute_rectangle_list_lower_bound(IndexArray* lptr, const IntegerRectangleList& rl)
    {
      IndexArray& l(*lptr);

      assert(!rl.empty());
      assert(rl.size()%2 == 0);
      dimension_type d = rl[0].size();

      l=rl[0];
      for(dimension_type j=0; j!=d; ++j) {
        l[j]=rl[0][j];
     }

      for(size_type i=1; i!=rl.size()/2; ++i) {
        for(dimension_type j=0; j!=d; ++j) {
          l[j] = std::min(l[j],rl[2*i][j]);
        }
      }

      return;
    }

    /* Compute upper bounds of the cell list cl. */
    inline
    void
    compute_rectangle_list_upper_bound(IndexArray* uptr, const IntegerRectangleList& rl)
    {
      IndexArray& u(*uptr);

      assert(!rl.empty());
      assert(rl.size()%2 == 0);
      dimension_type d = rl.array_size();

      u=rl[1];
      for(dimension_type j=0; j!=d; ++j) {
        u[j]=rl[1][j];
      }

      for(size_type i=1; i!=rl.size()/2; ++i) {
        for(dimension_type j=0; j!=d; ++j) {
          u[j] = std::max(u[j],rl[2*i+1][j]);
        }
      }

      return;
    }

    /* Compute upper and lower bounds of the cell list cl. */
    inline
    void
    compute_rectangle_list_bounds(IndexArray* lptr, IndexArray* uptr, const IntegerRectangleList& rl)
    {
      compute_rectangle_list_lower_bound(lptr,rl);
      compute_rectangle_list_upper_bound(uptr,rl);
      return;
    }

    void
    append_to_cell_list(IntegerCellList* clptr,
                        const IndexArray& lower,
                        const SizeArray& strides,
                        const BooleanArray& mask)
    {
      for(size_type index=0; index!=mask.size(); ++index) {
        if(mask[index]) {
          IndexArray position=compute_position(index,lower,strides);
          clptr->push_back(position);
        }
      }
    }

    void
    append_to_cell_list(IntegerCellList* clptr,
                        const IndexArray& lower,
                        const IndexArray& upper)
    {
      for(PositionIterator iter(lower,upper); !iter.end(); ++iter) {
        clptr->push_back(*iter);
      }
    }

    void
    append_to_cell_list(IntegerCellList* clptr,
                        const IntegerRectangleList rl)
    {
      for(size_type n=0; n!=rl.size()/2; ++n) {
        append_to_cell_list(clptr,rl[2*n],rl[2*n+1]);
      }
    }

    void
    compute_cell_mask(BooleanArray* maptr,
                 const SizeArray& grid_strides,
                 const IndexArray& grid_lower,
                 const IndexArray& position)
    {
      BooleanArray& ma(*maptr);
      size_type index=compute_index(position,grid_lower,grid_strides);
      ma[index]=true;
    }

    void
    compute_cell_list_mask(BooleanArray* maptr,
                 const SizeArray& grid_strides,
                 const IndexArray& grid_lower,
                 const IntegerCellList& cl)
    {
      for(size_type n=0; n!=cl.size(); ++n) {
        IndexArray position=cl[n];
        compute_cell_mask(maptr,grid_strides,grid_lower,position);
      }
    }

    void
    compute_rectangle_mask(BooleanArray* maptr,
                           const SizeArray& grid_strides,
                           const IndexArray& grid_lower,
                           const IndexArray& lower,
                           const IndexArray& upper)
    {
      dimension_type dim=grid_lower.size();
      BooleanArray& ma(*maptr);

      if(dim==1) {
        for(size_type i=lower[0]-grid_lower[0]; i!=size_type(upper[0]-grid_lower[0]); ++i) {
          ma[i]=true;
        }
        return;
      }

      if(dim==2) {
        SizeArray sizes=upper-lower;
        size_type index=compute_index(lower,grid_lower,grid_strides);
        for(size_type loop_end=index+sizes[1]*grid_strides[1]; index!=loop_end; index+=grid_strides[1]-sizes[0]) {
          for(size_type inner_loop_end=index + sizes[0]; index!=inner_loop_end; index+=1) {
            ma[index]=true;
          }
        }
        return;
      }

      if(dim==0) {
        return;
      }

      /* dim>2 */
      SizeArray sizes=upper-lower;

      size_type index=compute_index(lower,grid_lower,grid_strides);
      SizeArray pos(dim,0);

      while(true) {
        for(size_type loop_end=index+sizes[1]*grid_strides[1]; index!=loop_end; index+=grid_strides[1]-sizes[0]) {
          for(size_type inner_loop_end=index + sizes[0]; index!=inner_loop_end; index+=1) {
            ma[index]=true;
          }
        }
        pos[0]=0;
        pos[1]=sizes[1];

        dimension_type d=1;
        while(pos[d]==sizes[d]) {
          index-=sizes[d]*grid_strides[d];
          pos[d]=0;
          d+=1;
          if(d==pos.size()) {
            return;
          }
          index+=grid_strides[d];
          pos[d]+=1;
        }
      } /* while(true) */
    } /* compute_mask (of Rectangle) */

    void
    compute_rectangle_list_mask(BooleanArray* maptr,
                 const SizeArray& grid_strides,
                 const IndexArray& grid_lower,
                 const IntegerRectangleList& rl)
    {
      for(size_type n=0; n!=rl.size()/2; ++n) {
        IndexArray lower=rl[2*n];
        IndexArray upper=rl[2*n+1];
        compute_rectangle_mask(maptr,grid_strides,grid_lower,lower,upper);
      }
    }

    void
    translate_rectangle_coordinates(IntegerRectangleList* torlptr,
                                    const IntegerRectangleList& frrl,
                                    array< std::vector<index_type> > tr)
    {
      IntegerRectangleList& torl(*torlptr);
      dimension_type dim=tr.size();
      for(size_type n=0; n!=torl.size()/2; ++n) {
        for(dimension_type i=0; i!=dim; ++i) {
          torl[2*n][i]=tr[i][frrl[2*n][i]];
          torl[2*n+1][i]=tr[i][frrl[2*n+1][i]];
        }
      }
    }

    void
    translate_cell_coordinates(IntegerRectangleList* torlptr,
                               const IntegerCellList& frcl,
                               array< std::vector<index_type> > tr)
    {
      IntegerRectangleList& torl(*torlptr);
      dimension_type dim=tr.size();
      for(size_type n=0; n!=frcl.size(); ++n) {
        for(dimension_type i=0; i!=dim; ++i) {
          torl[2*n][i]=tr[i][frcl[n][i]];
          torl[2*n+1][i]=tr[i][frcl[n][i]+1];
        }
      }
    }


  }
}
