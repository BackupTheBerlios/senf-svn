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
// (at your option) any later version.b
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

#ifndef HH_RTCPPacket_
#define HH_RTCPPacket_ 1

// Custom includes
#include "Packet.hh"
#include "ParseInt.hh"
#include "ParseArray.hh"
#include "ParseVec.hh"
#include "PacketRegistry.hh"

//#include "RTCPPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {

    template <class I=nil,class P=nil> struct Parse_RTCP_RR;
    template <class I=nil,class P=nil> struct Parse_RTCP_SR;
    template <class I=nil,class P=nil> struct Parse_RTCP_SDES;
    template <class I=nil,class P=nil> struct Parse_RTCP_BYE;
    template <class I=nil,class P=nil> struct Parse_RTCP_APP;
    
    template <class Iterator=nil, class IPacket=nil>
    struct Parse_RTCP : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTCP<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTCP() {}
        Parse_RTCP(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}
                     
        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UIntField  < 0, 2, Iterator >    Parse_Version;
        typedef Parse_Flag       < 2, Iterator >       Parse_P;
        typedef Parse_UIntField  < 3, 8, Iterator >    Parse_Count;
        typedef Parse_UInt8      < Iterator >          Parse_PT;     
        typedef Parse_UInt16     < Iterator >          Parse_Length;

        typedef Parse_RTCP_RR    < Iterator >          Parse_RTCP_RR;
        typedef Parse_RTCP_SR    < Iterator >          Parse_RTCP_SR;
        typedef Parse_RTCP_SDES  < Iterator >          Parse_RTCP_SDES;
        typedef Parse_RTCP_BYE   < Iterator >          Parse_RTCP_BYE;
        typedef Parse_RTCP_APP   < Iterator >          Parse_RTCP_APP;

        Parse_Version  version()      const { return Parse_Version  (this->i()      ); }
        Parse_P        padding()      const { return Parse_P        (this->i()      ); }
        Parse_Count    count()        const { return Parse_Count    (this->i()      ); }
        Parse_PT       payloadType()  const { return Parse_PT       (this->i() + 2  ); }
        Parse_Length   length()       const { return Parse_Length   (this->i() + 3  ); }

        Parse_RTCP_RR   rr()   { return Parse_RTCP_RR   (this->i()  ); }
        Parse_RTCP_SR   sr()   { return Parse_RTCP_SR   (this->i()  ); }
        Parse_RTCP_SDES sdes() { return Parse_RTCP_SDES (this->i()  ); }
        Parse_RTCP_BYE  bye()  { return Parse_RTCP_BYE  (this->i()  ); }      
        Parse_RTCP_APP  app()  { return Parse_RTCP_APP  (this->i()  ); }
 
        ///////////////////////////////////////////////////////////////////////////

        unsigned int bytes() const { return 32 + (4 * length()); }

        static bool check(Iterator const & b, Iterator const & e)
        { return e-b >= 4 and static_cast<unsigned>(e-b) >= bytes(); }
    };

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_RTCP_RB : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTCP_RB<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTCP_RB() {}
        Parse_RTCP_RB(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////
        
        typedef Parse_UInt32        < Iterator >                    Parse_32bit;
        typedef Parse_UInt8         < Iterator >                    Parse_8bit; 
        typedef Parse_Array         < 3, Parse_UInt8<>, Iterator >  Parse_24bit; 

        Parse_32bit   ssrc()        const { return Parse_32bit(this->i()   ); }
        Parse_8bit    fragLost()    const { return Parse_8bit(this->i()+4  ); }
        Parse_24bit   cnpl()        const { return Parse_24bit(this->i()+5 ); }
        Parse_32bit   ehsnr()       const { return Parse_32bit(this->i()+8 ); }
        Parse_32bit   LSR()         const { return Parse_32bit(this->i()+12); }
        Parse_32bit   DLSR()        const { return Parse_32bit(this->i()+16); }
    };

    template <class Iterator, class IPacket>
    struct Parse_RTCP_RR : public Parse_RTCP<Iterator, IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTCP_RR<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTCP_RR() {}
        Parse_RTCP_RR(Iterator const & i) : Parse_RTCP<Iterator,IPacket>(i) {}
       
        ///////////////////////////////////////////////////////////////////////////
        
        typedef Parse_UInt32        < Iterator > Parse_32bit;
        typedef Parse_Vector        < Parse_RTCP_RB<>, typename Parse_RTCP<Iterator,IPacket>::Parse_Count, Iterator > Parse_rbVec;

        Parse_32bit   ssrc()        const { return Parse_32bit(this->i()+ 4  ); }
      
        Parse_32bit   ntp_msb()     const { return Parse_32bit(this->i()+ 8  ); }
        Parse_32bit   ntp_lsb()     const { return Parse_32bit(this->i()+ 12 ); }
        Parse_32bit   timestamp()   const { return Parse_32bit(this->i()+ 16 ); }
        Parse_32bit   spcount()     const { return Parse_32bit(this->i()+ 20 ); }
        Parse_32bit   socount()     const { return Parse_32bit(this->i()+ 24 ); }

        Parse_rbVec   rbList()      const { return Parse_rbVec(this->count(), this->i() + 28 ); } 
    };


/*
    // TODO SDES - chunk

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_RTCP_Chunk : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTCP_Chunk<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTCP_chunk() {}
        Parse_RTCP_chunk(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        static unsigned bytes() { return 4; }
        bool check(Iterator const & e) { return e-this->i() >= bytes(); }
        
        ///////////////////////////////////////////////////////////////////////////
        
        typedef Parse_UInt32        < Iterator >                    Parse_32bit; 

        Parse_32bit   ssrc()        const { return Parse_32bit(this->i()   ); }


    };

  */  
/*

    // TODO SDES - item -> 00

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_RTCP_item : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTCP_item<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTCP_item() {}
        Parse_RTCP_item(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        
        ///////////////////////////////////////////////////////////////////////////
        
        typedef Parse_UInt8         < Iterator >                    Parse_8bit;
        typedef Parse_UInt32        < Iterator >                    Parse_32bit;
        
        Parse_8bit   typeField()    const { return Parse_8bit(this->i()   ); }
        Parse_8bit   length()       const { return Parse_8bit(this->i()+1 ); }

        typedef Parse_Array         < length(), Parse_UInt8<>, Iterator >  Parse_desc;

        // Item-Description UTF-8
        Parse_desc   desc()       const { return Parse_desc(this->i()+2 ); }

        unsigned int bytes() const { return 2 + length(); }
        bool check(Iterator const & e) const { return e-this->i()>= 2 and e-this->i() >= int(bytes()); }

    };

  */ 
/*
 
    // TODO BYE

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_RTCP_BYE : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTCP_BYE<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTCP_BYE() {}
        Parse_RTCP_BYE(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        
        ///////////////////////////////////////////////////////////////////////////
        
        typedef Parse_UInt32        < Iterator >                    Parse_32bit;
        // negative offset to default RTCPHeader SC-Field
        typedef Parse_UIntField  < 3, 8, Iterator >    Parse_Count;
        typedef Parse_Vector     < Parse_32bit, Parse_Count, Iterator > Parse_ssrcList;
        
        // TODO reason for leaving
        
        Parse_Count      count()          const { return Parse_Count(i()-2); }
        Parse_ssrcList   ssrcList()       const { return Parse_ssrcList(count(), this->i()); }

        // Item-Description UTF-8
        unsigned int bytes() const { return 4 * length(); }
        bool check(Iterator const & e) const { return e-this->i() >= int(bytes()); }

    };

  */  
/*
    // TODO APP

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_RTCP_APP : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTCP_APP<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTCP_APP() {}
        Parse_RTCP_APP(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        
        ///////////////////////////////////////////////////////////////////////////
        
        typedef Parse_UInt32       < Iterator >       Parse_32bit;
        // TODO application dependent data
        
        Parse_32bit   ssrc()       const { return Parse_32bit(this->i()); }
        Parse_32bit   name()       const { return Parse_32bit(this->i()+4); }

        // Item-Description UTF-8
        unsigned int bytes() const { return 4 * length(); }
        bool check(Iterator const & e) const { return e-this->i() >= int(bytes()); }

    };
*/

    class RTCPPacket
        : public Packet, 
          public Parse_RTCP<Packet::iterator, RTCPPacket>, 
          public PacketRegistryMixin<RTCPTypes,RTCPPacket>
    {
        using Packet::registerInterpreter;
        using PacketRegistryMixin<RTCPTypes,RTCPPacket>::registerInterpreter;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<RTCPPacket>::ptr ptr;

    private:
        template <class Arg>
        RTCPPacket(Arg const & arg);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();

        friend class Packet;
    };
 
}}


///////////////////////////////hh.e////////////////////////////////////////
//#include "RTCPPacket.cci"
//#include "RTCPPacket.ct"
#include "RTCPPacket.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
