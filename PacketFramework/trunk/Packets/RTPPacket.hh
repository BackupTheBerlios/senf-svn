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

#ifndef HH_RTPPacket_
#define HH_RTPPacket_ 1

// Custom includes
#include "Packet.hh"
#include "ParseInt.hh"
#include "ParseArray.hh"
#include "ParseVec.hh"
#include "PacketRegistry.hh"

//#include "RTPPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {
    
    template <class Iterator=nil, class IPacket=nil>
    struct Parse_RTP : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTP<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTP() {}
        Parse_RTP(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        static unsigned bytes() { return 12; }
        bool check(Iterator const & e) const { return e-this->i() >= static_cast<int>(bytes()); }
        
        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UIntField  < 0, 2, Iterator >    Parse_Version;
        typedef Parse_Flag       < 2, Iterator >       Parse_P;
        typedef Parse_Flag       < 3, Iterator >       Parse_X;
        typedef Parse_UIntField  < 4, 8, Iterator >    Parse_CC;
        typedef Parse_Flag       < 0, Iterator >       Parse_M;
        typedef Parse_UIntField  < 1, 8, Iterator >    Parse_PT;
        typedef Parse_UInt16     < Iterator >          Parse_Seq;
        typedef Parse_UInt32     < Iterator >          Parse_32bit;
        typedef Parse_Vector     < Parse_32bit, Parse_CC, Iterator > Parse_CCVec;
      
        Parse_Version  version()     const { return Parse_Version  (this->i()      ); }
        Parse_P        padding()     const { return Parse_P        (this->i()      ); }
        Parse_X        extension()   const { return Parse_X        (this->i()      ); }
        Parse_CC       csrcCount()   const { return Parse_CC       (this->i()      ); }
        Parse_M        marker()      const { return Parse_M        (this->i() + 1  ); }
        Parse_PT       payloadType() const { return Parse_PT       (this->i() + 1  ); }
        Parse_Seq      seqNumber()   const { return Parse_Seq      (this->i() + 2  ); }
        Parse_32bit    timestamp()   const { return Parse_32bit    (this->i() + 4  ); }
        Parse_32bit    ssrc()        const { return Parse_32bit    (this->i() + 8  ); }
        Parse_CCVec    csrcList()    const { return Parse_CCVec (csrcCount(), this->i() + 12 ); }

    };

    struct RTPTypes {
        typedef boost::uint16_t key_t;
    };

    class RTPPacket
        : public Packet, 
          public Parse_RTP<Packet::iterator, RTPPacket>, 
          public PacketRegistryMixin<RTPTypes,RTPPacket>
    {
        using PacketRegistryMixin<RTPTypes,RTPPacket>::registerInterpreter;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<RTPPacket>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no public constructors
        // no conversion constructors
        
        template <class InputIterator>
        static ptr create(InputIterator begin, InputIterator end);        

        ///@}

    private:
        template <class InputIterator>
        RTPPacket(InputIterator begin, InputIterator end);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();

        friend class Packet;
    };

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_RTPExtension : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTPExtension<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTPExtension() {}
        Parse_RTPExtension(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        static unsigned bytes() { return 4; }
        bool check(Iterator const & e) { return e-this->i() >= bytes(); }
        
        ///////////////////////////////////////////////////////////////////////////
        
        typedef Parse_UIntField < 0,  3, Iterator > Parse_Priority;
        typedef Parse_UInt16    < Iterator >        Parse_16bit;
        typedef Parse_UInt8     < Iterator >        Parse_8bit;
        typedef Parse_Vector    < Parse_8bit, Parse_16bit, Iterator > Parse_ext;

        Parse_16bit proDef()      const { return Parse_16bit(this->i()); }
        Parse_16bit length()      const { return Parse_16bit(this->i() +2); }
        Parse_ext      ext()      const { return Parse_ext (length(), this->i() + 8 ); }


    };

    class RTPExtensionPacket
        : public Packet,
          public Parse_RTPExtension<Packet::iterator, RTPExtensionPacket>,
          public PacketRegistryMixin<RTPTypes, RTPExtensionPacket>
    {
        using PacketRegistryMixin<RTPTypes, RTPExtensionPacket>::registerInterpreter;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<RTPExtensionPacket>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no public constructors
        // no conversion constructors
        
        template <class InputIterator>
        static ptr create(InputIterator begin, InputIterator end);        

        ///@}

    private:
        template <class InputIterator>
        RTPExtensionPacket(InputIterator begin, InputIterator end);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();

        friend class Packet;
    };

}}


///////////////////////////////hh.e////////////////////////////////////////
//#include RTPPacket.cci"
//#include "RTPPacket.ct"
#include "RTPPacket.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
