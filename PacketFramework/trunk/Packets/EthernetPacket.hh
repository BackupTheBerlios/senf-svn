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
#include "Packet.hh"
#include "ParseInt.hh"
#include "ParseArray.hh"

//#include "EthernetPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {
    
    template <class Iterator=nil, class IPacket=nil>
    struct Parse_Ethernet : protected ParserBase<Iterator,IPacket>
    {
        template <class I, class P=nil>
        struct rebind { typedef Parse_Ethernet<I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_Ethernet() {}
        Parse_Ethernet(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        static unsigned bytes() { return 14; }
        
        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_Array< 6, Parse_UInt8<>, Iterator > Parse_MAC;
        typedef Parse_UInt16<Iterator> Parse_Type;
        
        static unsigned bytes() { return 14; }
        
        Parse_MAC  destination() { return Parse_MAC  (i() ); }
        Parse_MAC  source()      { return Parse_MAC  (i() + Parse_MAC::size() ); }
        Parse_Type type()        { return Parse_Type (i() + 2*Parse_MAC::size() ); }

        void init() { 
            destination().init(); 
            source().init(); 
            type().init();
        }
    };

    /** \brief
      */
    class EthernetPacket
        : public Packet, public Parse_Ethernet<Packet::iterator, EthernetPacket>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<EthernetPacket>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no public constructors
        // no conversion constructors
        
        template <class InputIterator>
        static ptr create(InputIterator begin, InputIterator end);        

        ///@}

    protected:

    private:
        template <class InputIterator>
        EthernetPacket(InputIterator begin, InputIterator end);
        EthernetPacket(raw_container::iterator begin, raw_container::iterator end,
                       Packet const * parent);

        virtual void v_nextInterpreter() const;
        virtual void v_finalize();

        friend class Packet;
    };


}}


///////////////////////////////hh.e////////////////////////////////////////
//#include "EthernetPacket.cci"
//#include "EthernetPacket.ct"
#include "EthernetPacket.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
