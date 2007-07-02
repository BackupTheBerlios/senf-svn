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

#ifndef HH_EthernetPacket_
#define HH_EthernetPacket_ 1

// Custom includes
#include <algorithm>
#include "Packets/PacketType.hh"
#include "Packets/ParseInt.hh"
#include "Packets/PacketRegistry.hh"

//#include "EthernetPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    struct MACAddress
    {
        MACAddress() { std::fill(address,address+6,0u); }
        template <class ForwardIterator>
        MACAddress(ForwardIterator i) { std::copy(i,boost::next(i,6),address); }

        PacketInterpreterBase::byte operator[](unsigned i) const { return address[i]; }
        PacketInterpreterBase::byte & operator[](unsigned i) { return address[i]; }

        PacketInterpreterBase::byte address[6];
    };

    struct Parse_MAC : public PacketParserBase
    {
        Parse_MAC(iterator i, state const & s) : PacketParserBase(i,s,6) {}
       
        ///////////////////////////////////////////////////////////////////////////

        typedef MACAddress value_type;

        value_type value() const { return MACAddress(i()); }
        void value(value_type v) { std::copy(v.address, v.address+6, i()); }
        operator value_type () { return value(); }
        Parse_MAC const & operator= (value_type other) { value(other); return *this; }
    };

    struct Parse_Ethernet : public PacketParserBase
    {
        Parse_Ethernet(container c) : PacketParserBase(c) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UInt16                      Parse_Type;

        Parse_MAC  destination() const { return parse<Parse_MAC>  (i()    ); }
        Parse_MAC  source()      const { return parse<Parse_MAC>  (i() + 2); }
        Parse_Type type()        const { return parse<Parse_Type> (i() + 4); }
    };

    struct EtherTypes {
        // See http://www.iana.org/assignments/ethernet-numbers
        typedef boost::uint16_t key_t;
    };

    struct EthernetPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<EthernetPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<EthernetPacketType, EtherTypes> mixin;
        typedef PacketInterpreter<EthernetPacketType> interpreter;
        typedef Parse_Ethernet parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;

        static interpreter::size_type initSize()
            { return 14; }
        
        /** \todo Add LLC/SNAP support -> only use the registry
            for type() values >=1536, otherwise expect an LLC header */
        static registry_key_t nextPacketKey(interpreter & i) 
            { return i.fields().type(); }

        static void dump(interpreter & i, std::ostream & os);
    };

    struct Parse_EthVLan : public PacketParserBase
    {
        Parse_EthVLan(container c) : PacketParserBase(c) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UIntField < 0,  3 > Parse_Priority;
        typedef Parse_Flag          < 3 > Parse_CFI;
        typedef Parse_UIntField < 4, 16 > Parse_VLanId;
        typedef Parse_UInt16              Parse_Type;

        Parse_Priority priority() const { return parse<Parse_Priority> (i()    ); }
        Parse_CFI      cfi()      const { return parse<Parse_CFI>      (i()    ); }
        Parse_VLanId   vlanId()   const { return parse<Parse_VLanId>   (i()    ); }
        Parse_Type     type()     const { return parse<Parse_Type>     (i() + 2); }
    };

    struct EthVLanPacketType
        : public PacketTypeBase, 
          public PacketTypeMixin<EthVLanPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<EthVLanPacketType, EtherTypes> mixin;
        typedef PacketInterpreter<EthVLanPacketType> interpreter;
        typedef Parse_EthVLan parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;

        static interpreter::size_type initSize()
            { return 4; }

        /** \todo Add LLC/SNAP support -> only use the registry
            for type() values >=1536, otherwise expect an LLC header */
        static registry_key_t nextPacketKey(interpreter & i) 
            { return i.fields().type(); }

        static void dump(interpreter & i, std::ostream & os);
    };

}


///////////////////////////////hh.e////////////////////////////////////////
//#include "EthernetPacket.cci"
//#include "EthernetPacket.ct"
//#include "EthernetPacket.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
