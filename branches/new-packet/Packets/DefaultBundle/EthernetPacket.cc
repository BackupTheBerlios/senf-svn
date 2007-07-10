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

#include "EthernetPacket.hh"
//#include "EthernetPacket.ih"

// Custom includes
#include <iomanip>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::EthVLanPacketType>
        registerEthVLanPacket(0x8100);
}

namespace {
    void dumpmac(std::ostream & os, senf::MACAddress mac)
    {
        for (unsigned i = 0; i < 6; ++i) {
            if (i > 0)
                os << ':';
            os << std::hex << std::setw(2) << std::setfill('0')
               << unsigned(mac[i]);
        }
    }
}

prefix_ void senf::EthernetPacketType::dump(packet p, std::ostream & os)
{
    if (p->type() <= 1500)
        os << "Ethernet 802.3";
    else if (p->type() >= 0x600)
        os << "Ethernet II (DIX)";
    else
        os << "Ethernet 802.3 (bad ethertype >1500 and <1536)";
    os << ": \n"
       << "  destination   : ";
    dumpmac(os,p->destination());
    os << "\n"
       << "  source        : ";
    dumpmac(os,p->source());
    os << "\n"
       << "  ethertype     : " << std::hex << std::setw(4) << std::setfill('0')
       << unsigned(p->type()) << "\n" << std::dec;
}

prefix_ void senf::EthVLanPacketType::dump(packet p, std::ostream & os)
{
    os << "Ethernet 802.1q (VLAN):\n"
       << "  priority      : " << p->priority() << "\n"
       << "  cfi           : " << p->cfi() << "\n"
       << "  vlan-ID       : " << p->vlanId() << "\n"
       << "  ethertype     : " << std::hex << std::setw(4) << std::setfill('0')
       << p->type() << "\n" << std::dec;
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
// comment-column: 40
// End:
