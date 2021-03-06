// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief MIHPacket unit tests */

//#include "MIHPacket.test.hh"
//#include "MIHPacket.test.ih"

// Custom includes
#include "MIHPacket.hh"
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf;

SENF_AUTO_UNIT_TEST(MIHPacket_msgId)
{
    MIHPacket mihPacket (MIHPacket::create());
    mihPacket->sid() = 4;
    mihPacket->opcode() = 3;
    mihPacket->aid() = 42;
    BOOST_CHECK_EQUAL( mihPacket->messageId(), 0x4c2a );
}

SENF_AUTO_UNIT_TEST(MIHPacket_create_eth)
{
    EthernetPacket eth (EthernetPacket::create());
    MIHPacket mihPacket (MIHPacket::createAfter(eth));
    eth.finalizeAll();
    BOOST_CHECK_EQUAL( eth->type_length(), 0x8917);
    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( eth.dump( oss));
}

SENF_AUTO_UNIT_TEST(MIHPacket_create_string)
{
    MIHPacket mihPacket (MIHPacket::create());
    // set some fields
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().value( "senf@berlios.de");
    mihPacket->dst_mihfId().value( "test");
    mihPacket.finalizeThis();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x19,
            // source MIHF_ID TLV:
            0x01, 0x10, // type, length
            0x0f,  // value-length
            0x73, 0x65, 0x6e, 0x66, 0x40, 0x62, 0x65, 0x72, 0x6c,
            0x69, 0x6f, 0x73, 0x2e, 0x64, 0x65,  // value
            // destination MIHF_ID TLV:
            0x02, 0x05, 0x04, 0x74, 0x65, 0x73, 0x74
    };
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            mihPacket.data().begin(), mihPacket.data().end() );
    BOOST_CHECK_EQUAL( mihPacket->src_mihfId().valueAsString(), "senf@berlios.de");
    BOOST_CHECK_EQUAL( mihPacket->dst_mihfId().valueAsString(), "test");

    // the maximum length of a MIHF_ID is 253 octets
    BOOST_CHECK_THROW( mihPacket->dst_mihfId().value( std::string(254, 'x')), std::length_error);
    // now expand a MIHF_ID
    // first the tricky one: when setting the maximum id length to 128 the TLV length field
    // is set to 129 and therefore expanded to 2 bytes; the id-length field is still 1 byte long
    mihPacket->dst_mihfId().maxIdLength(128);
    mihPacket->dst_mihfId().value( std::string(128, 'x'));
    mihPacket.finalizeThis();
    // packet size is now MIH header (8 bytes) + src MIHIFId TLV (18 bytes) +
    // dst MIHIFId TLV (1 byte type + 2 byte TLV length + 1 byte id length + 128 id value)
    BOOST_CHECK_EQUAL( mihPacket.size(), unsigned(8 + 18 + 1+2+1+128));
    BOOST_CHECK_EQUAL( mihPacket->payloadLength(), 18 + 1+2+1+128);
    BOOST_CHECK_EQUAL( mihPacket->dst_mihfId().length(), 1+128);
    BOOST_CHECK_EQUAL( senf::bytes(mihPacket->dst_mihfId()), 1+2+1+128);
    // now we extend the dst id to 129 bytes; than we have 2 bytes tlv length and 2 bytes
    // id-length field
    mihPacket->dst_mihfId().maxIdLength(129);
    mihPacket->dst_mihfId().value( std::string(129, 'x'));
    mihPacket.finalizeThis();
    // packet size is now MIH header (8 bytes) + src MIHIFId TLV (18 bytes) +
    // dst MIHIFId TLV (1 byte type + 2 byte TLV length + 2 byte id length + 128 id value)
    BOOST_CHECK_EQUAL( mihPacket.size(), unsigned(8 + 18 + 1+2+2+129));
    BOOST_CHECK_EQUAL( mihPacket->payloadLength(), 18 + 1+2+2+129);
    BOOST_CHECK_EQUAL( mihPacket->dst_mihfId().length(), 2+129);
    BOOST_CHECK_EQUAL( senf::bytes(mihPacket->dst_mihfId()), 1+2+2+129);
    // finally we shrink to dst id field
    mihPacket->dst_mihfId().value( "test");
    mihPacket.finalizeThis();
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            mihPacket.data().begin(), mihPacket.data().end() );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( mihPacket.dump( oss));
}


SENF_AUTO_UNIT_TEST(MIHPacket_create_mac)
{
    MACAddress srcMac ( MACAddress::from_string("01:02:03:04:05:06"));
    MACAddress dstMac ( MACAddress::from_string("07:08:09:0a:0b:0c"));
    MIHPacket mihPacket (MIHPacket::create());
    // set some fields
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().value( srcMac);
    mihPacket->dst_mihfId().value( dstMac);
    mihPacket.finalizeThis();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x1e,
            // source MIHF_ID TLV:
            0x01, 0x0d, // type, length
            0x0c,       // value-length
            0x5c, 0x01, 0x5c, 0x02, 0x5c, 0x03, 0x5c, 0x04, 0x5c, 0x05, 0x5c, 0x06, // value (nai-encoded)
            // destination MIHF_ID TLV:
            0x02, 0x0d, // type, length
            0x0c,       // value-length
            0x5c, 0x07, 0x5c, 0x08, 0x5c, 0x09, 0x5c, 0x0a, 0x5c, 0x0b, 0x5c, 0x0c  // value (nai-encoded)
    };
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            mihPacket.data().begin(), mihPacket.data().end() );
    BOOST_CHECK_EQUAL( mihPacket->src_mihfId().valueAsMACAddress(), srcMac);
    BOOST_CHECK_EQUAL( mihPacket->dst_mihfId().valueAsMACAddress(), dstMac);
    BOOST_CHECK_EQUAL( mihPacket->dst_mihfId().valueAs( MIHFId::MACAddress), MIHFId(dstMac) );
    BOOST_CHECK( mihPacket->src_mihfId().valueEquals(srcMac));
}


SENF_AUTO_UNIT_TEST(MIHPacket_create_inet4)
{
    MIHPacket mihPacket (MIHPacket::create());
    // set some fields
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().value( INet4Address::from_string("128.129.130.131"));
    mihPacket->dst_mihfId().value( INet4Address::from_string("132.133.134.135"));
    mihPacket.finalizeThis();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x16,
            // source MIHF_ID TLV:
            0x01, 0x09, // type, length
            0x08,       // value-length
            0x5c, 0x80, 0x5c, 0x81, 0x5c, 0x82, 0x5c, 0x83, // value (nai-encoded)
            // destination MIHF_ID TLV:
            0x02, 0x09, // type, length
            0x08,       // value-length
            0x5c, 0x84, 0x5c, 0x85, 0x5c, 0x86, 0x5c, 0x87  // value (nai-encoded)
    };
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            mihPacket.data().begin(), mihPacket.data().end() );
    BOOST_CHECK_EQUAL(
            mihPacket->src_mihfId().valueAsINet4Address(),
            INet4Address::from_string("128.129.130.131"));
    BOOST_CHECK_EQUAL(
            mihPacket->dst_mihfId().valueAsINet4Address(),
            INet4Address::from_string("132.133.134.135"));
}


SENF_AUTO_UNIT_TEST(MIHPacket_create_inet6)
{
    MIHPacket mihPacket (MIHPacket::create());
    // set some fields
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().value( INet6Address::from_string("::ffff:1.2.3.4"));
    mihPacket->dst_mihfId().value( INet6Address::from_string("::ffff:5.6.7.8"));
    mihPacket.finalizeThis();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x46,
            // source MIHF_ID TLV:
            0x01, 0x21, // type, length
            0x20,       // value-length
            // value (nai-encoded):
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00,
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00,
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0xff, 0x5c, 0xff,
            0x5c, 0x01, 0x5c, 0x02, 0x5c, 0x03, 0x5c, 0x04,
            // destination MIHF_ID TLV:
            0x02, 0x21, // type, length
            0x20,       // value-length
            // value (nai-encoded):
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00,
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x5c, 0x00,
            0x5c, 0x00, 0x5c, 0x00, 0x5c, 0xff, 0x5c, 0xff,
            0x5c, 0x05, 0x5c, 0x06, 0x5c, 0x07, 0x5c, 0x08
    };
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            mihPacket.data().begin(), mihPacket.data().end() );
    BOOST_CHECK_EQUAL(
            mihPacket->src_mihfId().valueAsINet6Address(),
            INet6Address::from_string("::ffff:1.2.3.4"));
    BOOST_CHECK_EQUAL(
            mihPacket->dst_mihfId().valueAsINet6Address(),
            INet6Address::from_string("::ffff:5.6.7.8") );
}


SENF_AUTO_UNIT_TEST(MIHPayload_parse)
{
    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15,
            // variable payload length:
            0x00, 0x29,
            // source MIHF_ID TLV:
            0x01, 0x10, // type, length
            0x0f,  // value-length
            0x73, 0x65, 0x6e, 0x66, 0x40, 0x62, 0x65, 0x72, 0x6c,
            0x69, 0x6f, 0x73, 0x2e, 0x64, 0x65,  // value
            // destination MIHF_ID TLV:
            0x02, 0x05, 0x04, 0x74, 0x65, 0x73, 0x74,
            // MIH Payload (two test tlvs)
            // first test tlv
            0x42, // type
            0x0a, // first bit not set, length=10
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, // value
            // second test tlv
            0x0c, // type
            0x04, // first bit not set, length=4
            0x1a, 0x2b, 0x3c, 0x4d // value
    };

    MIHPacket mihPacket (MIHPacket::create(data));
    BOOST_CHECK_EQUAL( mihPacket->payloadLength(), 41u);

    BOOST_REQUIRE( mihPacket.next().is<MIHGenericPayloadPacket>() );
    MIHGenericPayloadPacket mihPayload (mihPacket.next().as<MIHGenericPayloadPacket>());

    BOOST_CHECK_EQUAL( mihPayload->tlvList().size(), 2u);
    MIHGenericPayloadPacket::Parser::tlvList_t::container tlvListContainer (
            mihPayload->tlvList());

    MIHGenericTLVParser tlv1 = *tlvListContainer.begin();
    BOOST_CHECK_EQUAL( tlv1.type(), 0x42);
    BOOST_CHECK_EQUAL( tlv1.length(), 0x0au);
    BOOST_CHECK_EQUAL( tlv1.value().size(), 0x0a);

    MIHGenericTLVParser tlv2 = *boost::next(tlvListContainer.begin());
    BOOST_CHECK_EQUAL( tlv2.type(), 0x0c);
    BOOST_CHECK_EQUAL( tlv2.length(), 0x04u);
    BOOST_CHECK_EQUAL( tlv2.value().size(), 0x04);

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( mihPayload.dump( oss));
}


SENF_AUTO_UNIT_TEST(MIHPayload_create)
{
    MIHPacket mihPacket (MIHPacket::create());
    mihPacket->fragmentNr() = 42;
    mihPacket->transactionId() = 21;
    mihPacket->src_mihfId().value( "senf@berlios.de");
    mihPacket->dst_mihfId().value( "test");

    MIHGenericPayloadPacket mihPayload (MIHGenericPayloadPacket::createAfter(mihPacket));
    MIHGenericPayloadPacket::Parser::tlvList_t::container tlvListContainer (
            mihPayload->tlvList() );

    unsigned char tlv1_value[] = {
           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
    MIHGenericTLVParser tlv1 ( tlvListContainer.push_back_space());
    tlv1.type() = 0x42;
    tlv1.value( tlv1_value);

    unsigned char tlv2_value[] = {
            0x1a, 0x2b, 0x3c, 0x4d };
    MIHGenericTLVParser tlv2 ( tlvListContainer.push_back_space());
    tlv2.type() = 0x43;
    tlv2.value( tlv2_value);

    mihPacket.finalizeAll();

    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15,
            // variable payload length:
            0x00, 0x2b,
            // source MIHF_ID TLV:
            0x01, 0x10, // type, length
            0x0f,  // value-length
            0x73, 0x65, 0x6e, 0x66, 0x40, 0x62, 0x65, 0x72, 0x6c,
            0x69, 0x6f, 0x73, 0x2e, 0x64, 0x65,  // value
            // destination MIHF_ID TLV:
            0x02, 0x05, 0x04, 0x74, 0x65, 0x73, 0x74,
            // MIH Payload (two test tlvs)
            // first test tlv
            0x42, // type
            0x0a, // first bit not set, length=10
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, // value
            // second test tlv
            0x43, // type
            0x04, // first bit not set, length=4
            0x1a, 0x2b, 0x3c, 0x4d // value
    };
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            mihPacket.data().begin(), mihPacket.data().end() );
}

SENF_AUTO_UNIT_TEST(Test_MIHFIdTLV)
{
    unsigned char data[] = {
            // MIH header
            0x10, 0x54, 0x00, 0x00, 0x00, 0x15, 0x00, 0x06,
            // source MIHF_ID TLV:
            0x01, 0x01, // type, length
            0x00,       // value-length
            // destination MIHF_ID TLV:
            0x02, 0x01, // type, length
            0x00        // value-length
    };

    MIHPacket mihPacket (MIHPacket::create(data));
    BOOST_CHECK( mihPacket->src_mihfId().valueEquals( MIHFId::Multicast) );
    BOOST_CHECK( mihPacket->dst_mihfId().valueEquals( MIHFId::Multicast) );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
