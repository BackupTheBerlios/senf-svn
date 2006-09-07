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
#include <boost/format.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    namespace pkf = satcom::pkf;
    pkf::PacketRegistry<pkf::EtherTypes>::RegistrationProxy<pkf::EthVLanPacket> 
        registerEthVLanPacket(0x8100);
}

prefix_ void satcom::pkf::EthernetPacket::v_nextInterpreter()
    const
{
    registerInterpreter(type(),begin()+bytes(),end());
}

prefix_ void satcom::pkf::EthernetPacket::v_dump(std::ostream & os)
    const
{
    os << "Ethernet 802.1:\n"
       << "  destination   : " << (boost::format("%02x:%02x:%02x:%02x:%02x:%02x") 
                                % unsigned(destination()[0])
                                % unsigned(destination()[1])
                                % unsigned(destination()[2])
                                % unsigned(destination()[3])
                                % unsigned(destination()[4])
                                % unsigned(destination()[5])) << "\n"
       << "  source        : " << (boost::format("%02x:%02x:%02x:%02x:%02x:%02x") 
                                % unsigned(source()[0])
                                % unsigned(source()[1])
                                % unsigned(source()[2])
                                % unsigned(source()[3])
                                % unsigned(source()[4])
                                % unsigned(source()[5])) << "\n"
       << "  ethertype     : " << boost::format("%04x") % type() << "\n";
}

prefix_ void satcom::pkf::EthernetPacket::v_finalize()
{}

prefix_ void satcom::pkf::EthVLanPacket::v_nextInterpreter()
    const
{
    registerInterpreter(type(),begin()+bytes(),end());
}

prefix_ void satcom::pkf::EthVLanPacket::v_finalize()
{}

prefix_ void satcom::pkf::EthVLanPacket::v_dump(std::ostream & os)
    const
{
    os << "Ethernet 802.1q (VLAN):\n"
       << "  priority      : " << priority() << "\n"
       << "  cfi           : " << cfi() << "\n"
       << "  vlan-ID       : " << vlanId() << "\n"
       << "  ethertype     : " << boost::format("%04x") % type() << "\n";
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
