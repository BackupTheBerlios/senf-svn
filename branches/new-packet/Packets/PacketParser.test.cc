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
    \brief PacketParser.test unit tests */

//#include "PacketParser.test.hh"
//#include "PacketParser.test.ih"

// Custom includes
#include "PacketParser.hh"
#include "PacketInterpreter.hh"
#include "PacketType.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {};

    struct SimpleParser : public senf::PacketParserBase
    {
        SimpleParser( senf::PacketData * data)
            : senf::PacketParserBase(data) {}
        
        using senf::PacketParserBase::check;
        using senf::PacketParserBase::validate;
    };
}

BOOST_AUTO_UNIT_TEST(packetParserBase)
{
    senf::PacketInterpreter<VoidPacket>::ptr pi (senf::PacketInterpreter<VoidPacket>::create(4u));

    SimpleParser p (&pi->data());

    BOOST_CHECK( pi->data().begin() == p.i() );
    BOOST_CHECK( p.check(4u) );
    BOOST_CHECK( ! p.check(5u) );
    BOOST_CHECK_NO_THROW( p.validate(4u) );
    BOOST_CHECK_THROW( p.validate(5u), senf::TruncatedPacketException );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
