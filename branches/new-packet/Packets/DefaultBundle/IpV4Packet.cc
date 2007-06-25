// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Definition of non-inline non-template functions

#include "IpV4Packet.hh"
//#include "IpV4Packet.ih"
#include "EthernetPacket.hh"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::IpV4Packet>
        registerIpV4Packet (0x0800);

    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IpV4Packet>
        regsiterIpV4Packet2 (4); // IP-in-IP encapsulation
}

prefix_ void senf::IpV4Packet::v_nextInterpreter()
    const
{
    registerInterpreter(protocol(),begin()+bytes(),end());
}

prefix_ void senf::IpV4Packet::v_finalize()
{}

prefix_ void senf::IpV4Packet::v_dump(std::ostream & os)
    const
{
    struct in_addr in;
    in.s_addr = htonl(source());
    char buffer[128];
    std::string src (inet_ntop(AF_INET,&in,buffer,128));
    in.s_addr = htonl(destination());
    std::string dst (inet_ntop(AF_INET,&in,buffer,128));
    os << "Internet protocol Version 4:\n"
       << "  version       : " << version() << "\n"
       << "  IHL           : " << ihl() << "\n"
       << "  TOS           : " << unsigned(tos()) << "\n"
       << "  length        : " << length() << "\n"
       << "  identifier    : " << identifier() << "\n"
       << "  DF            : " << df() << "\n"
       << "  MF            : " << mf() << "\n"
       << "  fragment      : " << frag() << "\n"
       << "  TTL           : " << unsigned(ttl()) << "\n"
       << "  protocol      : " << unsigned(protocol()) << "\n"
       << "  CRC           : " << std::hex << crc() << std::dec << "\n"
       << "  source        : " << src << "\n"
       << "  destination   : " << dst << "\n";
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