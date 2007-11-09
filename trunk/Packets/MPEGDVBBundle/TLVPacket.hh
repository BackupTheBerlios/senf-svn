// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
    \brief TLVPacket public header */

#ifndef HH_TLVPacket_
#define HH_TLVPacket_ 1

// Custom includes
#include <algorithm>
#include "../../Packets/Packets.hh"

//#include "TLVPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief xxx
        
        \todo document me
        \todo add usefull exceptions strings
         
    */
    struct UnsuportedTLVPacketException : public std::exception
    { 
        virtual char const * what() const throw() { 
            return "length of length can be max. 4 bytes. Sorry.";
        }
    };

    /** \brief xxx
        \todo document me
    */
    class Parse_TLVPacketLength 
        : public detail::packet::ParseIntOps<Parse_TLVPacketLength, boost::uint32_t>,
          public PacketParserBase
    {
    public:
        Parse_TLVPacketLength(data_iterator i, state_type s) : PacketParserBase(i,s) {}

        typedef boost::uint32_t value_type;
        static const size_type init_bytes = 1;
        static value_type const min_value = 0;
        static value_type const max_value = 4294967295u;

        value_type value() const;
        void value(value_type const & v);
        
        Parse_TLVPacketLength const & operator= (value_type other);
    
        size_type bytes() const;
            
        void init() const;

    private:
        typedef Parse_Flag      <    0 > Parse_extended_length_flag;
        typedef Parse_UIntField < 1, 8 > Parse_fixed_length;

        Parse_extended_length_flag extended_length_flag() const {
            return parse<Parse_extended_length_flag>( 0 );
        }

        Parse_fixed_length fixed_length_field() const {
            return parse<Parse_fixed_length>( 0 );
        }
        
        void resize(size_type size);
    };  
        
    /** \brief parse TLVPacket Packet
    
        \todo document me
     
        \see TLVPacketType
     */
    template <class LengthParser>
    struct Parse_TLVPacket : public PacketParserBase
    {
#       include SENF_PARSER()
        
        SENF_PARSER_FIELD( type,   Parse_UInt32 );
        SENF_PARSER_FIELD( length, LengthParser );
        
        SENF_PARSER_FINALIZE(Parse_TLVPacket);
    };
    
    /** \brief generic TLV Packet type
        
        \todo document me
        
        \ingroup protocolbundle_mpegdvb
     */
    template <class LengthParser>
    struct TLVPacketType
        : public PacketTypeBase
    {
        typedef ConcretePacket<TLVPacketType<LengthParser> > packet;
        typedef Parse_TLVPacket<LengthParser> parser;

        static optional_range nextPacketRange(packet p);
        static size_type initSize();
        
        static void finalize(packet p);
        
        static void dump(packet p, std::ostream & os);
    };

    typedef TLVPacketType<Parse_TLVPacketLength>::packet TLVPacket;
    
    typedef TLVPacketType<Parse_UInt8>::packet  TLVFix8Packet;
    typedef TLVPacketType<Parse_UInt16>::packet TLVFix16Packet;
    typedef TLVPacketType<Parse_UInt24>::packet TLVFix24Packet;
    typedef TLVPacketType<Parse_UInt32>::packet TLVFix32Packet;
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "TLVPacket.cci"
#include "TLVPacket.ct"
//#include "TLVPacket.cti"
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
