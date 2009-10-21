/***************************************************************************
 *            box.h
 *
 *  Copyright 2008  Alberto Casagrande, Pieter Collins
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

/*! \file box.h
 *  \brief Coordinate-aligned boxes in Euclidean space.
 */

#ifndef ARIADNE_BOX_H
#define ARIADNE_BOX_H

#include "numeric.h"
#include "vector.h"

#include "set_interface.h"
#include "graphics_interface.h"
#include "point.h"

namespace Ariadne {

class Point;

//! A box in Euclidean space.
class Box
    : public SetInterface,
      public DrawableInterface,
      public Vector<Interval>
{
  public:
    typedef Float real_type;

    //! Construct a singleton point in zero dimensions.
    Box() : Vector<Interval>() { }
    //! Construct from an integer giving the dimension and a list of floating-point values
    //! giving alternately lower and upper bounds.
    explicit Box(uint d, const Float& x0l, const Float& x0u, ...);

    // Templated constructor; useful for automatic conversion from vector expressions
    template<class T> Box(const T& t) : Vector<Interval>(t) { }
    template<class T1, class T2> Box(const T1& t1, const T2& t2) : Vector<Interval>(t1,t2) { }

    //! Construct from a string literal of the form "[a1,b1]x[a2,b2]x...x[ad,bd]".
    explicit Box(const std::string& str);

    //! The unit box \f$[-1,1]^n\f$ in \a n dimensions.
    static Box unit_box(uint n) {
        return Box(n,Interval(-1,1));
    }

    //! The upper quadrant box \f$[0,infty]^n\f$ in \a n dimensions.
    static Box upper_quadrant(uint n) {
        return Box(n,Interval(0,inf()));
    }

    //! An explicit case to an interval vector. Useful to prevent ambiguous function overloads.
    const Vector<Interval>& vector() const { return *this; }

    //! The set of vertices of the box.
    std::vector<Point> vertices() const;

    //! An approximation to the centre of the box.
    Point centre() const {
        return Point(midpoint(static_cast<const Vector<Interval>&>(*this)));
    }

    //! The radius of the box in the supremum norm.
    Float radius() const {
        Float dmax=0;
        for(uint i=0; i!=this->size(); ++i) {
            dmax = max( dmax, (*this)[i].width() );
        }
        return up(dmax/2);
    }

    //! \brief Test if the box is empty.
    bool empty() const {
        return Ariadne::empty(*this);
    }

    //! \brief Test if the box is bounded.
    bool bounded() const {
        for(uint i=0; i!=this->Vector<Interval>::size(); ++i) {
            if(!Ariadne::bounded((*this)[i])) {
                return false;
            }
        }
        return true;
    }

    //! \brief Make a dynamically-allocated copy.
    virtual Box* clone() const {
        return new Box(*this);
    }

    //! \brief Test if the box is a superset of another box.
    bool contains(const Point& pt) const {
        return Ariadne::contains(this->vector(),pt.vector());
    }

    //! \brief Test if the box is a subset of another box.
    bool subset(const Box& bx) const {
        return Ariadne::subset(this->vector(),bx.vector());
    }

    //! \brief Test if the box is a superset of another box.
    bool superset(const Box& bx) const {
        return Ariadne::subset(bx.vector(),this->vector());
    }

    //! \brief Test if the box intersects another box. Returns true even
    //! if the intersection occurs only on the boundary of the boxes.
    //! Use Box::overlaps(const Box& bx) to test robust intersection
    //! of the interiors.
    bool intersects(const Box& bx) const {
        return Ariadne::intersect(this->vector(),bx.vector());
    }

    //! \brief The dimension of the space the box lies in.
    virtual uint dimension() const {
        return this->size();
    }

    //! \brief Tests if the box is disjoint from another box.
    //! Only returns true if the closures are disjoint.
    virtual tribool disjoint(const Box& other) const {
        return Ariadne::disjoint(this->vector(), other.vector());
    }

    //! \brief Tests if the box overlaps another box.
    //! Only returns true if the interior of one box intersects the other.
    virtual tribool overlaps(const Box& other) const {
        return Ariadne::overlap(this->vector(), other.vector());
    }

    //! \brief Tests if the box covers another box.
    //! Only returns true if the interior of the box is a superset
    //! of the closure of the other.
    virtual tribool covers(const Box& other) const {
        return Ariadne::inside(other.vector(), this->vector());
    }

    //! \brief Tests if the box covers another box.
    //! Only returns true if the closure of the box is a subset
    //! of the interior of the other.
    virtual tribool inside(const Box& other) const {
        return Ariadne::inside(this->vector(), other.vector());
    }

    //! \brief Returns an enclosing bounding box for the set.
    //! The result is guaranteed to contain the box in its interior.
    virtual Box bounding_box() const {
        static const Float min(std::numeric_limits<double>::min());
        static const Interval eps(-min,+min);
        return Box((*this)+Vector<Interval>(this->dimension(),eps));
    }

    //! \brief Widens the box by the minimal floating-point increment.
    //! The result is guaranteed to contain the box in its interior.
    void widen() {
        static const Float min(std::numeric_limits<double>::min());
        static const Interval eps(-min,+min);
        static_cast<Vector<Interval>&>(*this) += Vector<Interval>(this->dimension(),eps);
    }


    //! \brief Draw on a canvas.
    virtual void draw(CanvasInterface& c) const;

    //! \brief Write to an output stream.
    virtual std::ostream& write(std::ostream& os) const {
        return os << *static_cast<const Vector<Interval>*>(this);
    }
};

Box make_box(const std::string& str);

} // namespace Ariadne

#endif /* ARIADNE_BOX_H */
