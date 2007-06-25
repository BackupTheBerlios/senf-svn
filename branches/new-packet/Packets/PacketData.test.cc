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
    \brief PacketData.test unit tests */

//#include "PacketData.test.hh"
//#include "PacketData.test.ih"

// Custom includes
#include "PacketData.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(packetData)
{
    // We cannot simply allocate a packetData instance .. we must go through PacketInterpreterBase
    // and PacketImpl.

    std::auto_ptr<senf::detail::PacketImpl> p (senf::detail::PacketImpl::create());
    p->add_ref();
    senf::PacketInterpreterBase::ptr pi (
        senf::PacketInterpreterBase::create(0,0));
    p->appendInterpreter(pi.get());

    senf::PacketData & d (*pi);
    
    BOOST_CHECK( d.begin() == d.end() );
    BOOST_CHECK_EQUAL( d.size(), 0u );
    BOOST_CHECK( d.empty() );
    
    d.insert(d.begin(), 0xabu);
    BOOST_CHECK_EQUAL( d.size(), 1u );
    BOOST_CHECK_EQUAL( d[0], 0xabu );
    BOOST_CHECK( !d.empty() );
    
    d.insert(d.begin(), 10, 0xcdu );
    BOOST_CHECK_EQUAL( d.size(), 11u );
    BOOST_CHECK_EQUAL( d[0], 0xcdu );
    BOOST_CHECK_EQUAL( d[9], 0xcdu );
    BOOST_CHECK_EQUAL( d[10], 0xabu );

    senf::PacketData::byte data[] = 
        { 0xf0u, 0xf1u, 0xf2u, 0xf3u, 0xf4u, 0xf5u, 0xf6u, 0xf7u };
    d.insert(d.begin()+5, data, data+sizeof(data)/sizeof(data[0]));
    BOOST_CHECK_EQUAL( d.size(), 19u );
    BOOST_CHECK_EQUAL( d[4], 0xcdu );
    BOOST_CHECK_EQUAL( d[5], 0xf0u );
    BOOST_CHECK_EQUAL( d[12], 0xf7u );

    d.erase(d.begin());
    BOOST_CHECK_EQUAL( d.size(), 18u );
    BOOST_CHECK_EQUAL( d[4], 0xf0u );
    
    d.erase(d.begin(), d.begin()+11);
    BOOST_CHECK_EQUAL( d.size(), 7u );
    BOOST_CHECK_EQUAL( d[0], 0xf7u );

    d.resize(16u, 0xefu);
    BOOST_CHECK_EQUAL( d.size(), 16u );
    BOOST_CHECK_EQUAL( d[15], 0xefu );
    BOOST_CHECK_EQUAL( d[6], 0xabu );

    d.resize(8u, 0xbcu);
    BOOST_CHECK_EQUAL( d.size(), 8u );
    BOOST_CHECK_EQUAL( d[7], 0xefu );

    d.clear();
    BOOST_CHECK_EQUAL( d.size(), 0u );
    BOOST_CHECK( d.empty() );
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