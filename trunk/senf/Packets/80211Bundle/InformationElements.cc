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

/** \file
    \brief InformationElements non-inline non-template implementation */

#include "InformationElements.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANSSIDInfoElementParser            );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANPowerConstraintInfoElementParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANSupportedRatesInfoElementParser  );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANVendorSpecificInfoElementParser  );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANHTCapabilitiesInfoElementParser  );
//SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANHTOperationInfoElementParser     );

prefix_ void senf::WLANPowerConstraintInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN PowerConstraint Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  " << unsigned(value()) << "\n";
}

prefix_ void senf::WLANSSIDInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN SSID Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  " << value() << "\n";
}

prefix_ void senf::WLANSupportedRatesInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN SupportedRates Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  ToDo!\n";
}

prefix_ void senf::WLANVendorSpecificInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN Vendor Specific Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  ToDo!\n";
}

prefix_ void senf::WLANHTCapabilitiesInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN HT Capabilities Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  ToDo!\n";
}

prefix_ void senf::WLANHTOperationInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN HT Operation Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  ToDo!\n";
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
