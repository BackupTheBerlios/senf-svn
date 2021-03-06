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
    \brief StringParser.test unit tests */

//#include "StringParser.test.hh"
//#include "StringParser.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};

    typedef senf::StringParser<senf::UInt16Parser> MyStringParser;
}

SENF_AUTO_UNIT_TEST(stringParser)
{
    senf::PacketInterpreterBase::byte data[] = { 0x00, 0x04, 'T', 'E', 'S', 'T' };
    senf::PacketInterpreterBase::ptr p (senf::PacketInterpreter<VoidPacket>::create(data));

    BOOST_CHECK_EQUAL( p->data().size(), 6u );
    BOOST_CHECK_EQUAL( MyStringParser(p->data().begin(), &p->data()).value(), "TEST" );

    MyStringParser(p->data().begin(), &p->data()).value("Another Test");
    BOOST_CHECK_EQUAL( p->data().size(), 14u );
    BOOST_CHECK_EQUAL( p->data()[0], 0u );
    BOOST_CHECK_EQUAL( p->data()[1], 12u );
    BOOST_CHECK_EQUAL( MyStringParser(p->data().begin(), &p->data()).value(), "Another Test" );
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
