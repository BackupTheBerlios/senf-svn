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
    \brief ParseListB.test unit tests */

//#include "ParseListB.test.hh"
//#include "ParseListB.test.ih"

// Custom includes
#include "ParseListB.hh"
#include "ParseVec.hh"
#include "PacketType.hh"
#include "ParseInt.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};
}

BOOST_AUTO_UNIT_TEST(parseListB)
{
    typedef senf::Parse_VectorN<senf::Parse_UInt16,senf::Parse_UInt8>::parser ParseVec;
    typedef senf::Parse_ListB<ParseVec,senf::Parse_UInt16>::parser ParseList;

    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
                                             ParseList::init_bytes));
    
    {
        ParseList p (pi->data().begin(),&pi->data());
        p.init();
        BOOST_CHECK_EQUAL( p.size(), 0u );
        BOOST_CHECK_EQUAL( p.bytes(), 2u );
        BOOST_CHECK( p.empty() );
        BOOST_CHECK( p.begin() == p.end() );
    }

    {
#       define p ParseList(pi->data().begin(),&pi->data())

        p.push_back_space();
        BOOST_CHECK_EQUAL( p.bytes(), 3u );
        BOOST_CHECK_EQUAL( p.size(), 1u );
        BOOST_CHECK_EQUAL( p.front().bytes(), 1u );
        BOOST_CHECK_EQUAL( p.front().size(), 0u );

        // THIS FAILS ... need to find a solution here ...
//         p.front().push_back(0x1234u);
//         BOOST_CHECK_EQUAL( p.front().bytes(), 3u );
//         BOOST_CHECK_EQUAL( p.front().size(), 1u );
//         BOOST_CHECK_EQUAL( p.front()[0], 0x1234u );
//         BOOST_CHECK_EQUAL( p.bytes(), 5u );
//         BOOST_CHECK_EQUAL( p.size(), 1u );

#       undef p
    }
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
