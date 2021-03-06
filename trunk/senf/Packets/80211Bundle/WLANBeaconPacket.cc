// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Thorsten Horstmann <tho@berlios.de>
//   Christian Niephaus <cni@berlios.de>


// Definition of 802.11 WLAN Beacon Packet non-inline non-template functions

#include "WLANBeaconPacket.hh"

// Custom includes
#include <boost/io/ios_state.hpp>
#include "Registries.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::WLANManagementSubtypes, 8u, senf::WLANBeaconPacket );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::WLANBeaconPacket );

prefix_ void senf::WLANBeaconPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "WLAN beacon frame:\n"
       << senf::fieldName("timestamp")      << unsigned( p->timestamp())      << "\n"
       << senf::fieldName("beaconInterval") << unsigned( p->beaconInterval()) << "\n";
    p->ssidIE().dump( os);
    p->supportedRatesIE().dump( os);
    os << "  Optional Information Elements:\n";
    typedef parser::ieList_t::container_type ieListContainer_t;
    ieListContainer_t ieListContainer (p->ieList());
    for (ieListContainer_t::const_iterator i = ieListContainer.begin(); i != ieListContainer.end(); ++i)
        i->dump( os);
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
