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
    \brief SafePacketParser.test unit tests */

//#include "SafePacketParser.test.hh"
//#include "SafePacketParser.test.ih"

// Custom includes
#include "SafePacketParser.hh"
#include "ParseInt.hh"
#include "PacketType.hh"
#include "Packet.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket_Type : public senf::PacketTypeBase
    {};
    typedef senf::ConcretePacket<VoidPacket_Type> VoidPacket;
}

BOOST_AUTO_UNIT_TEST(safePacketParser)
{
    VoidPacket vp (VoidPacket::create(senf::Parse_UInt16::fixed_bytes));
    
    senf::SafePacketParser<senf::Parse_UInt16> p;
    
    BOOST_CHECK( !p );

    p =  senf::Parse_UInt16(vp.data().begin(),&vp.data());

    BOOST_CHECK( p );
    (*p) = 0x1234u;
    
    BOOST_CHECK_EQUAL( (*p), 0x1234u );
    BOOST_CHECK_EQUAL( vp.data()[0], 0x12u );

    vp.data().resize(1024u);
    BOOST_CHECK_EQUAL( (*p), 0x1234u );
    (*p) = 0x2345u;
    BOOST_CHECK_EQUAL( vp.data()[0], 0x23u );
}

///////////////////////////////cc.e////////////////////////////////////////
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
