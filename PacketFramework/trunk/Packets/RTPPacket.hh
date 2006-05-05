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
 
        ///////////////////////////////////////////////////////////////////////////

        unsigned int bytes() const { return 12 + ( 4 * csrcCount()); }
        bool check(Iterator const & e) const { return e-this->i()>= 12 and e-this->i() >= int(bytes()); }
        
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
    struct Parse_RTPExtensionBase : public ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTPExtensionBase<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTPExtensionBase() {}
        Parse_RTPExtensionBase(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        
        
        ///////////////////////////////////////////////////////////////////////////
                
        typedef Parse_UInt16    < Iterator >        Parse_16bit;

        Parse_16bit         proDef()   const { return Parse_16bit(this->i()); };
        Parse_16bit         length()   const { return Parse_16bit(this->i()+2); };

        unsigned int bytes() const { return 4 + length(); }        
        bool check(Iterator e) const { return e-this->i()>=4 and e-this->i() >= bytes(); }

    }; 

    class RTPExtensionBasePacket 
        : public Packet
    {

    public:
         ///////////////////////////////////////////////////////////////////////////       
         typedef ptr_t<RTPExtensionBasePacket>::ptr ptr;

         template <class InputIterator>
         static ptr create(InputIterator begin, InputIterator end); 

    private:
        template <class InputIterator>
        RTPExtensionBasePacket(InputIterator begin, InputIterator end);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize() = 0;

        friend class Packet;

    };


    template <class Iterator=nil, class IPacket=nil>
    struct Parse_RTPUnknownExtension : public Parse_RTPExtensionBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_RTPUnknownExtension<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_RTPUnknownExtension() {}
        Parse_RTPUnknownExtension(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}
     
        ///////////////////////////////////////////////////////////////////////////
           
        typedef Parse_UInt16    < Iterator >        Parse_16bit;
        typedef Parse_UInt8     < Iterator >        Parse_8bit;
        typedef Parse_Vector    < Parse_8bit, Parse_16bit, Iterator > Parse_ext;
       
        Parse_ext      ext()      const { return Parse_ext (Parse_RTPExtensionBase<Iterator,IPacket>::length(), this->i() + 8 ); }    

    };

    class RTPUnknownExtensionPacket
        : public RTPExtensionBasePacket,
          public Parse_RTPUnknownExtension<Packet::iterator, RTPUnknownExtensionPacket>
          //public PacketRegistryMixin<RTPTypes, RTPUnknownExtensionPacket>
    {
        //using PacketRegistryMixin<RTPTypes, RTPExtensionPacket>::registerInterpreter;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<RTPUnknownExtensionPacket>::ptr ptr;

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
        RTPUnknownExtensionPacket(InputIterator begin, InputIterator end);

        //virtual void v_nextInterpreter() const;
        virtual void v_finalize();

        friend class RTPExtensionBasePacket;
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
