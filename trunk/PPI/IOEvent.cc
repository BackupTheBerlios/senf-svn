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
    \brief IOEvent non-inline non-template implementation */

#include "IOEvent.hh"
//#include "IOEvent.ih"

// Custom includes
#include <boost/bind.hpp>

//#include "IOEvent.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::IOEvent

////////////////////////////////////////
// private members

prefix_ void senf::ppi::IOEvent::v_enable()
{
    Scheduler::instance().add(fd_, boost::bind(&IOEvent::cb,this,_1,_2), 
                              Scheduler::EventId(events_));
}

prefix_ void senf::ppi::IOEvent::v_disable()
{
    Scheduler::instance().remove(fd_, Scheduler::EventId(events_));
}

prefix_ void senf::ppi::IOEvent::cb(int, Scheduler::EventId event)
{
    IOEventInfo info = { event };
    callback(info);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IOEvent.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End: