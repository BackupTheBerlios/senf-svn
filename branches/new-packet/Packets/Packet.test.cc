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
    \brief Packet.test unit tests */

//#include "Packet.test.hh"
//#include "Packet.test.ih"

// Custom includes
#include <sstream>
#include "PacketType.hh"
#include "PacketRegistry.hh"
#include "Packet.hh"
#include "ParseInt.hh"
#include "PacketParser.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    struct RegTag {
        typedef unsigned key_t;
    };

    struct FooPacketType 
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<FooPacketType>
    {
        using senf::PacketTypeMixin<FooPacketType>::nextPacketRange;
        typedef senf::PacketInterpreter<FooPacketType> interpreter;
        static interpreter::size_type initSize()
            { return 4u; }
    };
    typedef senf::ConcretePacket<FooPacketType> FooPacket;

    struct BarPacketParser : public senf::PacketParserBase
    {
        BarPacketParser(container c) : PacketParserBase(c) {}
        
        typedef senf::Parse_UInt16 Parse_Type;
        typedef senf::Parse_Int32  Parse_Length;
        typedef senf::Parse_UInt16 Parse_Reserved;

        Parse_Type type() const { return parse<Parse_Type> (i()); }
        Parse_Length length() const { return parse<Parse_Length> (i()+2); }
        Parse_Reserved reserved() const { return parse<Parse_Reserved> (i()+6); }
    };

    struct BarPacketType 
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<BarPacketType>
    {
        using senf::PacketTypeMixin<BarPacketType>::nextPacketRange;
        typedef senf::ConcretePacket<BarPacketType> packet;
        typedef BarPacketParser parser;
        static size_type initSize() 
            { return 8u; }
        static void init(packet p) {
            p->reserved() = 0xA0A0u;
        }
        static void dump(packet p, std::ostream & os) {
            os << "BarPacket:\n"
               << "type: " << p->type() << "\n"
               << "length: " << p->length() << "\n";
        }
        static void finalize(packet p) {
            if (p.next())
                p->type() = senf::PacketRegistry<RegTag>::key(p.next());
            else
                p->type() = -1;
        }
    };
    typedef BarPacketType::packet BarPacket;

    namespace reg {
        senf::PacketRegistry<RegTag>::RegistrationProxy<FooPacketType> registerFoo(1u);
        senf::PacketRegistry<RegTag>::RegistrationProxy<BarPacketType> registerBar(2u);
    }

}

BOOST_AUTO_UNIT_TEST(packet)
{
    senf::Packet packet (FooPacket::create());
    BarPacket::createAfter(packet);

    BOOST_REQUIRE( packet );
    BOOST_CHECK( packet.next() );
    BOOST_CHECK( ! packet.next().next() );
    BOOST_CHECK( ! packet.prev() );
    BOOST_CHECK( packet.next().prev() == packet );
    BOOST_CHECK( packet.next() != packet );
    BOOST_CHECK_EQUAL( packet.data().size(), 12u );
    BOOST_CHECK_EQUAL( packet.next().data().size(), 8u );
    BOOST_CHECK( packet.is<FooPacket>() );
    BOOST_CHECK( packet.next().is<BarPacket>() );
    BOOST_CHECK( packet.first() == packet );
    BOOST_CHECK( packet.last() == packet.next() );
    
    senf::Packet p2 (packet.next());
    BOOST_CHECK( p2 );
    packet.parseNextAs<FooPacket>();
    BOOST_CHECK_EQUAL( packet.data().size(), 12u );
    BOOST_CHECK_EQUAL( packet.next().data().size(), 8u );
    BOOST_CHECK( packet.next().is<FooPacket>() );
    BOOST_CHECK( ! p2 );
    BOOST_CHECK( packet.next().as<FooPacket>() );
    
    p2 = packet.next().clone();
    BOOST_REQUIRE( p2 );
    packet.next().append( p2 );
    BOOST_REQUIRE( packet.next().next() );
    BOOST_CHECK( ! packet.next().next().next() );
    BOOST_CHECK_EQUAL( packet.data().size(), 16u );

    // This calls and checks typeId()
    BOOST_CHECK_EQUAL( senf::PacketRegistry<RegTag>::key(packet), 1u );
    packet.next().parseNextAs( senf::PacketRegistry<RegTag>::lookup(2u).factory() );
    BOOST_CHECK( packet.next().next().is<BarPacket>() );
    
    std::stringstream s;
    packet.dump(s);
    BOOST_CHECK_EQUAL( s.str(), "BarPacket:\ntype: 0\nlength: 0\n" );
    
    packet.finalize();
    BOOST_CHECK_EQUAL( packet.last().as<BarPacket>()->type(), 
                       BarPacket::type::parser::Parse_Type::value_type(-1) );
    packet.last().append(FooPacket::create());
    packet.finalize();
    BOOST_CHECK_EQUAL( packet.last().prev().as<BarPacket>()->type(), 1u );
}

BOOST_AUTO_UNIT_TEST(concretePacket)
{
    FooPacket::byte data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };

    BOOST_CHECK_EQUAL( FooPacket::create().data().size(), 4u );
    BOOST_CHECK_EQUAL( FooPacket::create(FooPacket::noinit).data().size(), 0u );
    BOOST_CHECK_THROW( FooPacket::create(2u), senf::TruncatedPacketException );
    BOOST_CHECK_EQUAL( FooPacket::create(10u).data().size(), 10u );
    BOOST_CHECK_EQUAL( FooPacket::create(2u,FooPacket::noinit).data().size(), 2u );
    BOOST_CHECK_EQUAL( FooPacket::create(data).data().size(), 6u );

    senf::Packet packet (FooPacket::create());

    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet).data().size(), 4u );
    BOOST_CHECK_EQUAL( packet.data().size(), 8u );

    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,FooPacket::noinit).data().size(), 0u );
    BOOST_CHECK_EQUAL( packet.data().size(), 4u );

    BOOST_CHECK_THROW( FooPacket::createAfter(packet,2u), senf::TruncatedPacketException );
    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,10u).data().size(), 10u );
    BOOST_CHECK_EQUAL( packet.data().size(), 14u );
    
    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,2u,FooPacket::noinit).data().size(), 2u );
    BOOST_CHECK_EQUAL( packet.data().size(), 6u );
    
    BOOST_CHECK_EQUAL( FooPacket::createAfter(packet,data).data().size(), 6u );
    BOOST_CHECK_EQUAL( packet.data().size(), 10u );
    
    BOOST_CHECK_EQUAL( FooPacket::createBefore(packet).data().size(), 14u );
    BOOST_CHECK_EQUAL( packet.data().size(), 10u );

    BOOST_CHECK_EQUAL( FooPacket::createBefore(packet,FooPacket::noinit).data().size(), 10u );
    BOOST_CHECK_EQUAL( packet.data().size(), 10u );

    BOOST_CHECK( packet.clone() != packet );
    BOOST_CHECK_EQUAL( BarPacket::create()->reserved(), 0xA0A0u );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
