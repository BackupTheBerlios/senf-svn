// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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

// Definition of 802.11 WLAN Beacon Packet non-inline non-template functions

// Custom includes
#include "WLANBeaconPacket.hh"

#include <senf/Packets/Packets.hh>
#include <boost/io/ios_state.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::WLANBeaconPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);    
    os << "WLAN beacon frame:\n"
       << senf::fieldName("timestamp")      << unsigned( p->timestamp())      << "\n"
       << senf::fieldName("beaconInterval") << unsigned( p->beaconInterval()) << "\n";
    p->ssidIE().dump( os);
    p->supportedRatesIE().dump( os);
    os << "  Optional Information Elements:\n";
    typedef parser::ieList_t::container ieListContainer_t;
    ieListContainer_t ieListContainer (p->ieList());
    for (ieListContainer_t::const_iterator i = ieListContainer.begin(); i != ieListContainer.end(); ++i)
        i->dump( os);
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
