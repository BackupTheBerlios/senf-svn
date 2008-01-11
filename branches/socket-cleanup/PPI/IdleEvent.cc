// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief IdleEvent non-inline non-template implementation */

#include "IdleEvent.hh"
//#include "IdleEvent.ih"

// Custom includes
#include "../Scheduler/Scheduler.hh"

//#include "IdleEvent.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::IdleEvent

////////////////////////////////////////
// private members

prefix_ void senf::ppi::IdleEvent::v_enable()
{
    id_ = Scheduler::instance().timeout(manager().now(), boost::bind(&IdleEvent::cb,this));
}

prefix_ void senf::ppi::IdleEvent::v_disable()
{
    Scheduler::instance().cancelTimeout(id_);
    id_ = 0;
}

prefix_ void senf::ppi::IdleEvent::cb()
{
    callback();
    if (enabled())
        v_enable();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IdleEvent.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End: