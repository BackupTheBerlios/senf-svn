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
    \brief MIH protocol non-inline non-template implementation */

#include "MIHPacket.hh"
//#include "MIHPacket.ih"

// Custom includes
#include <boost/io/ios_state.hpp>
#include <senf/Utils/String.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::EtherTypes, senf::MIHPacketType::etherType, senf::MIHPacket);


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// MIHPacketType

prefix_ void senf::MIHPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "MIH Packet:\n"
       << "  protocol header:\n"
       << senf::fieldName("  version")           << unsigned( p->version()) << "\n"
       << senf::fieldName("  ack request")       << p->ackRequest() << "\n"
       << senf::fieldName("  ack response")      << p->ackResponse() << "\n"
       << senf::fieldName("  UIR")               << p->uir() << "\n"
       << senf::fieldName("  more fragments")    << p->moreFragment() << "\n"
       << senf::fieldName("  fragment number")   << p->fragmentNr() << "\n"
       << senf::fieldName("  message ID (MID)")  << unsigned( p->messageId()) << "\n"
       << senf::fieldName("    sid")             << unsigned( p->sid()) << "\n"
       << senf::fieldName("    opcode")          << unsigned( p->opcode()) << "\n"
       << senf::fieldName("    aid")             << unsigned( p->aid()) << "\n"
       << senf::fieldName("  transaction id")    << unsigned( p->transactionId()) << "\n"
       << senf::fieldName("  payload length")    << unsigned( p->payloadLength()) << "\n";
    p->src_mihfId().dump( os);
    p->dst_mihfId().dump( os);
}

prefix_ void senf::MIHPacketType::finalize(packet p)
{
    p->src_mihfId().finalize();
    p->dst_mihfId().finalize();
    p->payloadLength_() << p.size() - 8;
    p->messageId() << key(p.next(nothrow));
}

prefix_ senf::PacketInterpreterBase::factory_t senf::MIHPacketType::nextPacketType(packet p)
{
    if (p.data().size() < initSize())
        return no_factory();
    PacketRegistry<MIHMessageRegistry>::Entry const * e (
        PacketRegistry<MIHMessageRegistry>::lookup( p->messageId(), nothrow ));
    return e ? e->factory() : MIHGenericPayloadPacket::factory();
}

prefix_ void senf::MIHPacketType::validate(packet p)
{
    try {
        if (p.data().size() < initSize())
            throw InvalidMIHPacketException("truncated MIH message");
        if (p->version() != 1)
            throw InvalidMIHPacketException("invalid MIH version: ") << senf::str(p->version());
        if (p->payloadLength() != p.size()-8)
            throw InvalidMIHPacketException("wrong MIH length: ") << senf::str(p->payloadLength());
        if (p.next(senf::nothrow))
            MIHMessageRegistry::instance().validate( p->messageId(), p.next());
    } catch (TruncatedPacketException & e) {
        throw InvalidMIHPacketException("truncated MIH message");
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// MIHGenericPayloadPacketType

prefix_ void senf::MIHGenericPayloadPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "MIH Payload (service specific TLVs):\n";
    typedef parser::tlvList_t::container tlvListContainer_t;
    tlvListContainer_t tlvListContainer (p->tlvList());
    for (tlvListContainer_t::const_iterator i = tlvListContainer.begin(); i != tlvListContainer.end(); ++i)
        i->dump( os);
}

prefix_ void senf::MIHGenericPayloadPacketType::finalize(packet p)
{
    typedef parser::tlvList_t::container tlvContainer_t;
    tlvContainer_t tlvs (p->tlvList() );
    for (tlvContainer_t::iterator i (tlvs.begin()); i != tlvs.end(); ++i) {
        MIHGenericTLVParser p (*i);
        p.finalize();
    }
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
