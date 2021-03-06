// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief IdleEvent public header */

#ifndef HH_IdleEvent_
#define HH_IdleEvent_ 1

// Custom includes
#include "Events.hh"

//#include "IdleEvent.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Continually signaled event

        An IdleEvent is signaled continually and repeatedly while enabled. It will consume 100% of
        available CPU resources. The resource usage is controlled by adequate event throttling.
        
        \ingroup event_group
     */
    class IdleEvent
        : public EventImplementation<>
    {
    public:
        IdleEvent();

    protected:

    private:
        virtual void v_enable();
        virtual void v_disable();

        void cb();
        
        unsigned id_;
    };

}}


///////////////////////////////hh.e////////////////////////////////////////
#include "IdleEvent.cci"
//#include "IdleEvent.ct"
//#include "IdleEvent.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
