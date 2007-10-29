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
    \brief Target non-inline non-template implementation */

#include "Target.hh"
//#include "Target.ih"

// Custom includes

//#include "Target.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::log::Target

prefix_ senf::log::Target::Target()
{
    TargetRegistry::instance().registerTarget(this);
}

prefix_ senf::log::Target::~Target()
{
    while( ! rib_.empty()) {
        // This is terribly slow but simplifies the area cache handling and removing a target should
        // be quite seldom
        RIB::reverse_iterator i (rib_.rbegin());
        unroute(i->stream, i->area, i->level, i->action);
    }
    TargetRegistry::instance().unregisterTarget(this);
}

prefix_ void senf::log::Target::updateRoutingCache(detail::StreamBase const * stream,
                                                   detail::AreaBase const * area)
{
    if (! stream) {
        StreamRegistry::Registry::iterator i (StreamRegistry::instance().registry_.begin());
        StreamRegistry::Registry::iterator const i_end (StreamRegistry::instance().registry_.end());
        for (; i != i_end ; ++i)
            updateRoutingCache(i->second, area);
        return;
    }
    if (! area) {
        AreaRegistry::Registry::iterator i (AreaRegistry::instance().registry_.begin());
        AreaRegistry::Registry::iterator const i_end (AreaRegistry::instance().registry_.end());
        for (; i != i_end ; ++i)
            updateRoutingCache(stream, i->second);
        return;
    }
    unsigned limit (DISABLED::value);
    RIB::iterator i (rib_.begin());
    RIB::iterator const i_end (rib_.end());
    for(; i != i_end; ++i)
        if ( (! i->stream || i->stream == stream) &&
             (! i->area || i->area == area) &&
             i->action == ACCEPT ) {
            unsigned l (i->level == NONE::value ? i->stream->defaultRuntimeLimit() : i->level);
            if (l < limit)
                limit = l;
        }
    if (limit == DISABLED::value)
        area->removeRoutingCache(*this, *stream);
    else
        area->updateRoutingCache(*this, *stream, limit);
}

prefix_ void senf::log::Target::write(boost::posix_time::ptime timestamp,
                                      detail::StreamBase const & stream,
                                      detail::AreaBase const & area, unsigned level,
                                      std::string const & message)
{
    RIB::iterator i (rib_.begin());
    RIB::iterator const i_end (rib_.end());
    for (; i != i_end; ++i)
        if ( (! i->stream || i->stream == &stream) &&
             (! i->area || i->area == &area) &&
             (i->level == NONE::value ? i->stream->defaultRuntimeLimit() : i->level) <= level ) {
            if (i->action == ACCEPT)
                v_write(timestamp, stream.v_name(), area.v_name(), level, message);
            return;
        }
}

///////////////////////////////////////////////////////////////////////////
// senf::log::TargetRegistry

prefix_ void senf::log::TargetRegistry::write(detail::StreamBase const & stream,
                                              detail::AreaBase const & area, unsigned level,
                                              std::string msg)
{
    boost::posix_time::ptime timestamp (boost::posix_time::microsec_clock::universal_time());
    area.write(timestamp, stream, level, msg);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Target.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
