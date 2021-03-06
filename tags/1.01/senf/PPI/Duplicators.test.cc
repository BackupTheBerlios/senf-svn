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
    \brief Duplicators unit tests */

//#include "Duplicators.test.hh"
//#include "Duplicators.test.ih"

// Custom includes
#include "Duplicators.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include <senf/Packets/Packets.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

SENF_AUTO_UNIT_TEST(activeDuplicator)
{
    debug::ActiveSource source;
    module::ActiveDuplicator duplicator;
    debug::PassiveSink sink1;
    debug::PassiveSink sink2;

    ppi::connect(source, duplicator);
    ppi::connect(duplicator, sink1);
    ppi::connect(duplicator, sink2);
    ppi::init();

    senf::Packet p (senf::DataPacket::create());

    source.submit(p);
    BOOST_CHECK_EQUAL( sink1.size(), 1u );
    BOOST_CHECK_EQUAL( sink2.size(), 1u );
    BOOST_CHECK( sink1.pop_front() == p );
    BOOST_CHECK( sink2.pop_front() == p );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
