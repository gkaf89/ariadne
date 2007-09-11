/***************************************************************************
 *            integrator_plugin_interface.h
 *
 *  Copyright  2006-7  Alberto Casagrande, Pieter Collins
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
 
/*! \file integrator_plugin_interface.h
 *  \brief Class for computing the image of a basic set under a map.
 */

#ifndef ARIADNE_INTEGRATOR_PLUGIN_INTERFACE_H
#define ARIADNE_INTEGRATOR_PLUGIN_INTERFACE_H

#include <boost/shared_ptr.hpp>

#include "../base/types.h"
#include "../base/declarations.h"
#include "../linear_algebra/declarations.h"
#include "../geometry/declarations.h"
#include "../system/declarations.h"

#include "applicator_plugin_interface.h"

namespace Ariadne {
  namespace Evaluation {


    /*! \brief A class for computing the image of a basic set under a map. 
     *  \ingroup Integrators
     */
    template<class BS>
    class IntegratorPluginInterface
    {
      typedef typename BS::real_type R;
      typedef Numeric::Interval<R> I;
     public:
      //@{ 
      //! \name Constructors and cloning operations.
      /*! \brief Virtual destructor. */
      virtual ~IntegratorPluginInterface() { }
      /*! \brief Make a dynamically-allocated copy. */
      virtual IntegratorPluginInterface<BS>* clone() const = 0;
      //@}


      //@{ 
      //! \name Methods for applying a system to a basic set.

      /*! \brief Compute the flow of a point. */
      virtual 
      Geometry::Point<I> 
      flow_step(const System::VectorFieldInterface<R>& f, 
                        const Geometry::Point<I>& s, 
                        const Numeric::Interval<R>& t, 
                        const Geometry::Rectangle<R>& bb) const = 0;


      /*! \brief Compute the image of a basic set under a continuous function. Returns a dynamically allocated set. */
      
      virtual 
      BS
      integration_step(const System::VectorFieldInterface<R>& f, 
                       const BS& s, 
                       const Numeric::Interval<R>& t, 
                       const Geometry::Rectangle<R>& bb) const = 0; 
      
      /*! \brief Compute the image of a basic set under a continuous function. Returns a dynamically allocated set. */
      virtual 
      BS
      reachability_step(const System::VectorFieldInterface<R>& f, 
                                const BS& s, 
                                const Numeric::Interval<R>& t, 
                                const Geometry::Rectangle<R>& bb) const = 0;
      //@}
    };


    /*! \brief A class for computing the image of a basic set under a differentiable map. 
     *  \ingroup Integrators
     */
    template<class BS>
    class DifferentiableIntegratorPluginInterface
      : public IntegratorPluginInterface<BS>
    {
      typedef typename BS::real_type R;
      typedef Numeric::Interval<R> I;
     public:
      /*! \brief Make a dynamically-allocated copy. */
      virtual DifferentiableIntegratorPluginInterface<BS>* clone() const = 0;
      //@{ 
      //! \name Methods for computing flow Jacobians. 
      /*! \brief Compute the spacial jacobian over a flow step of time \a t starting at \a p assuming that the flow remains within \a bb. */
      virtual LinearAlgebra::Matrix<I> flow_step_jacobian(const System::VectorFieldInterface<R>& vf,
                                                          const Geometry::Point<I>& p,
                                                          const Numeric::Interval<R>& t,
                                                          const Geometry::Rectangle<R>& bb) const = 0;
    };

  }
}


#endif /* ARIADNE_INTEGRATOR_PLUGIN_INTERFACE_H */
