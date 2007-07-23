// $Id:DatagramSection.hh 327 2007-07-20 10:03:44Z tho $
//
// Copyright (C) 2007
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

#ifndef HH_TransportPacket_
#define HH_TransportPacket_ 1

// Custom includes
#include <algorithm>
#include "Packets/PacketType.hh"
#include "Packets/ParseInt.hh"
#include "Packets/PacketRegistry.hh"
#include "Packets/PacketParser.hh"

//#include "TransportPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
    
    ///\addtogroup protocolbundle_mpegdvb
    ///@{
    
    struct Parse_TransportPacket : public PacketParserBase
    {
        SENF_PACKET_PARSER_INIT(Parse_TransportPacket);
        
        typedef Parse_Flag      <     0 > Parse_tei; // transport_error_indicator
        typedef Parse_Flag      <     1 > Parse_pusi; // payload_unit_start_indicator
        typedef Parse_Flag      <     2 > Parse_transportPrio;  // transport_priority
        typedef Parse_UIntField < 2, 16 > Parse_pid;
        typedef Parse_UIntField < 0,  2 > Parse_tsc; // transport_scrambling_control
        typedef Parse_UIntField < 2,  4 > Parse_adaptCtrl; // adaptation_field_control
        typedef Parse_UIntField < 4,  8 > Parse_contCounter; // continuity_counter
    
        ///////////////////////////////////////////////////////////////////////////
                
        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((Field       ) ( syncByte,                  Parse_UInt8         )) 
            ((OverlayField) ( transport_error_indicator, Parse_tei           ))
            ((OverlayField) ( pusi,                      Parse_pusi          ))
            ((OverlayField) ( transport_priority,        Parse_transportPrio ))
            ((Field       ) ( pid,                       Parse_pid           ))
            ((OverlayField) ( transport_scrmbl_ctrl,     Parse_tsc           ))
            ((OverlayField) ( adaptation_field_ctrl,     Parse_adaptCtrl     ))
            ((Field       ) ( continuity_counter,        Parse_contCounter   ))      
        );
        
    };
    
    struct TransportPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<TransportPacketType>
    {
        typedef PacketTypeMixin<TransportPacketType> mixin;
        typedef ConcretePacket<TransportPacketType> packet;
        typedef Parse_TransportPacket parser;
    
        using mixin::nextPacketRange;
    //          using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;
        
        static void dump(packet p, std::ostream & os);
    };
        
    typedef TransportPacketType::packet TransportPacket;
    
    ///@}
  
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "TransportPacket.cci"
//#include "TransportPacket.ct"
//#include "TransportPacket.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
