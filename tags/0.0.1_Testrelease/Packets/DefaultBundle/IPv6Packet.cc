// $Id$
//
// Copyright (C) 2007
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
    \brief IPv6Packet non-inline non-template implementation */

#include "IPv6Packet.hh"
//#include "IPv6Packet.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include "EthernetPacket.hh"
#include "../../Socket/Protocols/INet/INetAddressing.hh"

//#include "IPv6Packet.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::IPv6Packet>
        registerIPv6Packet (0x86dd);

    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv6Packet>
        registerIPv6Packet2 (41); // IP6-in-IP(6) encapsulation
}

prefix_ void senf::IPv6PacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Internet protocol Version 6:\n"
       << "  version        : " << unsigned(p->version()) << "\n"
       << "  traffic class  : 0x" 
         << std::hex << std::setw(2) << std::setfill('0') << unsigned(p->trafficClass()) << "\n"
       << "  flow label     : 0x" 
         << std::hex << std::setw(5) << std::setfill('0') << unsigned(p->flowLabel()) << "\n"
       << "  payload length : " << std::dec << unsigned(p->length()) << "\n"
       << "  next header    : " << unsigned(p->nextHeader()) << "\n"
       << "  hop limit      : " << unsigned(p->hopLimit()) << "\n"
       << "  source         : " << p->source() << "\n"
       << "  destination    : " << p->destination() << "\n";
}

prefix_ void senf::IPv6PacketType::finalize(packet p)
{
    p->length() << (p.size() - IPv6PacketParser::fixed_bytes);
    try {
        p->nextHeader() << key(p.next());
    }
    catch (InvalidPacketChainException & ex) {
        p->nextHeader() << 59; // No next header
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IPv6Packet.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
