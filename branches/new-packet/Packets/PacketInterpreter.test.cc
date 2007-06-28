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
    \brief PacketInterpreter.test unit tests */

//#include "PacketInterpreter.test.hh"
//#include "PacketInterpreter.test.ih"

// Custom includes
#include "PacketImpl.hh"
#include "PacketInterpreter.hh"
#include "PacketType.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {
        typedef senf::PacketInterpreter<VoidPacket> interpreter;
        static interpreter::optional_range nextPacketRange(interpreter & i)
            { return interpreter::range(i.data().end(), i.data().end()); }
    };
}

BOOST_AUTO_UNIT_TEST(packetInterpreterBase)
{
    {
        senf::PacketInterpreter<VoidPacket>::ptr pi2 (senf::PacketInterpreter<VoidPacket>::create());
        senf::PacketInterpreter<VoidPacket>::ptr pi3 (senf::PacketInterpreter<VoidPacket>::createAfter(pi2)); 
        senf::PacketInterpreter<VoidPacket>::ptr pi1 (senf::PacketInterpreter<VoidPacket>::createBefore(pi2));
   
        pi2->data().insert(pi2->data().begin(),0x02);
        BOOST_CHECK_EQUAL( pi1->data().size(), 1u );
        BOOST_CHECK_EQUAL( pi2->data().size(), 1u );
        
        BOOST_CHECK( pi2 == pi1->next() );
        BOOST_CHECK( pi3 == pi2->next() );
        BOOST_CHECK( ! pi3->next() );
        BOOST_CHECK( pi2 == pi3->prev() );
        BOOST_CHECK( pi1 == pi2->prev() );
        BOOST_CHECK( ! pi1->prev() );

        pi1->data().insert(pi1->data().begin(),2,0x01u);
        BOOST_CHECK_EQUAL( pi1->data().size(), 3u );
        BOOST_CHECK_EQUAL( pi2->data().size(), 1u );
        BOOST_CHECK_EQUAL( pi3->data().size(), 0u );

        senf::PacketInterpreter<VoidPacket>::ptr pi2b (pi2->clone());
        BOOST_REQUIRE( pi2b->next() );
        BOOST_CHECK( ! pi2b->next()->next() );
        BOOST_CHECK( ! pi2b->prev() );

        pi2b->data().insert(pi2b->data().begin(),0x03u);
        BOOST_CHECK_EQUAL( pi2->data().size(), 1u );
        BOOST_CHECK_EQUAL( pi2b->data().size(), 2u );
    }

    {
        senf::PacketInterpreter<VoidPacket>::ptr p
            (senf::PacketInterpreter<VoidPacket>::create(4u));
        
        BOOST_CHECK_EQUAL( p->data().size(), 4u );
        BOOST_CHECK_EQUAL( std::distance(p->data().begin(),p->data().end()), 4 );
    }

    {
        senf::PacketInterpreter<VoidPacket>::byte data[] = { 0x01, 0x02, 0x03, 0x04 };
        senf::PacketInterpreter<VoidPacket>::ptr p
            (senf::PacketInterpreter<VoidPacket>::create(data));

        BOOST_CHECK_EQUAL( p->data().size(), 4u );
        BOOST_CHECK_EQUAL( std::distance(p->data().begin(),p->data().end()), 4 );
        BOOST_CHECK( std::equal(p->data().begin(), p->data().end(), data) );
    }
            
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
