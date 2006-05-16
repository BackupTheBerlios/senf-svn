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

#ifndef HH_IpV4Packet_
#define HH_IpV4Packet_ 1

// Custom includes
#include "Packet.hh"
#include "ParseInt.hh"
#include "ParseArray.hh"
#include "PacketRegistry.hh"

//#include "IpV4Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {
    
    template <class Iterator=nil, class IpV4Packet=nil>
    struct Parse_IpV4 : public ParserBase<Iterator,IpV4Packet>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_IpV4<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_IpV4() {}
        Parse_IpV4(Iterator const & i) : ParserBase<Iterator,IpV4Packet>(i) {}

        static unsigned bytes() { return 20; }
        static bool check(Iterator const & b, Iterator const & e) 
        { return static_cast<unsigned>(e-b) >= bytes(); }
        
        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UIntField <  0,  4, Iterator > Parse_Version;
        typedef Parse_UIntField <  4,  8, Iterator > Parse_IHL;
        typedef Parse_UInt8     <         Iterator > Parse_8bit;
        typedef Parse_UInt16    <         Iterator > Parse_16bit;
        typedef Parse_Flag      <  0,     Iterator > Parse_R;
        typedef Parse_Flag      <  1,     Iterator > Parse_DF;
        typedef Parse_Flag      <  2,     Iterator > Parse_MF;
        typedef Parse_UIntField <  3, 16, Iterator > Parse_Frag;
        typedef Parse_UInt32    <         Iterator > Parse_32bit;  
        
        Parse_Version  version()     const { return Parse_Version (this->i()      ); }
        Parse_IHL      ihl()         const { return Parse_IHL     (this->i()      ); }
        Parse_8bit     tos()         const { return Parse_8bit    (this->i() + 1  ); }
        Parse_16bit    length()      const { return Parse_16bit   (this->i() + 2  ); }
        Parse_16bit    identifier()  const { return Parse_16bit   (this->i() + 4  ); }
        Parse_R        reserved()    const { return Parse_R       (this->i() + 6  ); }
        Parse_DF       df()          const { return Parse_DF      (this->i() + 6  ); }
        Parse_MF       mf()          const { return Parse_MF      (this->i() + 6  ); }
        Parse_Frag     frag()        const { return Parse_Frag    (this->i() + 6  ); }
        Parse_8bit     ttl()         const { return Parse_8bit    (this->i() + 8  ); }
	Parse_8bit     protocol()    const { return Parse_8bit    (this->i() + 9  ); } 
	Parse_16bit    crc()         const { return Parse_16bit   (this->i() + 10 ); }
	Parse_32bit    source()      const { return Parse_32bit   (this->i() + 12 ); }
        Parse_32bit    destination() const { return Parse_32bit   (this->i() + 16 ); }
    };

    struct IpV4Types {
        typedef boost::uint16_t key_t;
    };

    class IpV4Packet
        : public Packet, 
          public Parse_IpV4<Packet::iterator,IpV4Packet>, 
          public PacketRegistryMixin<IpV4Types,IpV4Packet>
    {
        using PacketRegistryMixin<IpV4Types,IpV4Packet>::registerInterpreter;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<IpV4Packet>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////

    private:
        template <class Arg>
        IpV4Packet(Arg const & arg);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();

        friend class Packet;
    };
}}


///////////////////////////////hh.e////////////////////////////////////////
//#include IpV4Packet.cci"
//#include "IpV4Packet.ct"
#include "IpV4Packet.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
