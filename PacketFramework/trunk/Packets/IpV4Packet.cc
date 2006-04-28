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

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    namespace pkf = satcom::pkf;
    pkf::PacketRegistry<pkf::EtherTypes>::RegistrationProxy<pkf::IpV4Packet> 
        registerIpV4Packet(0x0800);
}

prefix_ void satcom::pkf::IpV4Packet::v_nextInterpreter()
    const
{
    registerInterpreter(protocol(),begin()+bytes(),end());
}

prefix_ void satcom::pkf::IpV4Packet::v_finalize()
{}



///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
