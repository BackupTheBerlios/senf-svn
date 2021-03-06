// $Id$
//
// Copyright (C) 2009 
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
    \brief Duplicators non-inline non-template implementation */

#include "Duplicators.hh"
//#include "Duplicators.ih"

// Custom includes

//#include "Duplicators.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void
senf::ppi::module::ActiveDuplicator::connectorSetup(ActiveDuplicator::ConnectorType & conn)
{
    route(input, conn);
}

prefix_ void senf::ppi::module::ActiveDuplicator::request()
{
    Packet p (input());
    ActiveDuplicator::ContainerType::iterator i (connectors().begin());
    ActiveDuplicator::ContainerType::iterator const i_end (connectors().end());
    for (; i != i_end; ++i)
        (*i)(p);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Duplicators.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
