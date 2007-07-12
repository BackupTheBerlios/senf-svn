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
    \brief safe_data_iterator.test unit tests */

//#include "safe_data_iterator.test.hh"
//#include "safe_data_iterator.test.ih"

// Custom includes
#include "safe_data_iterator.hh"
#include "PacketType.hh"
#include "PacketInterpreter.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {};
}

BOOST_AUTO_UNIT_TEST(safePacketIterator)
{
    // We cannot simply allocate a packetData instance .. we must go through PacketInterpreterBase
    // and PacketImpl.

    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create());

    senf::PacketData & d (pi->data());

    senf::safe_data_iterator i;

    BOOST_CHECK( ! i );
    i = senf::safe_data_iterator(d);
    BOOST_CHECK( i );
    i = d.begin();
    BOOST_CHECK( i == senf::safe_data_iterator(d,d.begin()) );
    BOOST_CHECK( senf::PacketData::iterator(i) == d.begin() );

    senf::PacketData::byte data[] = 
        { 0xf0u, 0xf1u, 0xf2u, 0xf3u, 0xf4u, 0xf5u, 0xf6u, 0xf7u };
    d.resize(sizeof(data)/sizeof(data[0]));
    BOOST_CHECK( senf::PacketData::iterator(i) == d.begin() );
    std::copy(data,data+sizeof(data)/sizeof(data[0]),i);

    BOOST_CHECK_EQUAL( d.size(), sizeof(data)/sizeof(data[0]) );
    BOOST_CHECK_EQUAL( *(i+sizeof(data)/sizeof(data[0])-1), 0xf7u );
    BOOST_CHECK_EQUAL( std::distance(i,senf::safe_data_iterator(d,d.end())), 
                       senf::PacketData::difference_type(d.size()) );
    *(++i) = 0x01u;
    BOOST_CHECK_EQUAL( d[1], 0x01u );
    *(--i) = 0x02u;
    BOOST_CHECK_EQUAL( d[0], 0x02u );
    BOOST_CHECK( &d == &i.data() );
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
