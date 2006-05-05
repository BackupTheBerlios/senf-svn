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

// Unit tests

//#include "RTPPacket.test.hh"
//#include "RTPPacket.test.ih"

// Custom includes
#include "RTPPacket.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace satcom::pkf;

BOOST_AUTO_UNIT_TEST(rtpPacket_parser)
{
    unsigned char data[] = { 0x13, 0x02, 0x03, 0x04, 
			     0x05, 0x06, 0x07, 0x08, 
			     0x09, 0x0A, 0x0B, 0x0C,

                             0x11, 0x12, 0x13, 0x14, // CSRC 1
                             0x15, 0x16, 0x17, 0x18, // CSRC 2
                             0x19, 0x1A, 0x1B, 0x1C,  // CSRC 3 

                             0x20, 0x02, 0x22, 0x23
  
                           };                       

    typedef unsigned char * iterator;
    Parse_RTP<iterator> p(data);

    BOOST_CHECK_EQUAL( p.version(),            0x00u       );
    BOOST_CHECK_EQUAL( p.padding(),            0           );
    BOOST_CHECK_EQUAL( p.extension(),          1           );
    BOOST_CHECK_EQUAL( p.csrcCount(),          0x03u       );    
    BOOST_CHECK_EQUAL( p.marker(),             0           );
    BOOST_CHECK_EQUAL( p.payloadType(),        0x02u       );
    // the static_cast is to silence gcc-3.3
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p.seqNumber()), 0x0304u );
    BOOST_CHECK_EQUAL( p.timestamp(),          0x05060708u );
    BOOST_CHECK_EQUAL( p.ssrc(),               0x090A0B0Cu );
 
    BOOST_CHECK_EQUAL( p.csrcList()[0],        0x11121314u ); 
    BOOST_CHECK_EQUAL( p.csrcList()[1],        0x15161718u );
    BOOST_CHECK_EQUAL( p.csrcList()[2],        0x191A1B1Cu );


    //p->last()->reinterpret<RTPUnknownExtensionPacket>();
    //BOOST_CHECK( p->next()->is<RTPUnknownExtensionPacket>() );
    //RTPUnknownExtensionPacket::ptr v (p->next()->reinterpret<RTPUnknownExtensionPacket>());
   /* BOOST_CHECK_EQUAL( v->proDef(), 0x20 );
    BOOST_CHECK_EQUAL( v->length(), 0x02 );
    BOOST_CHECK_EQUAL( v->ext(), 0x2223 );
    */
    
    


}

		      
BOOST_AUTO_UNIT_TEST(rtpPacket_packet)
{

    unsigned char data[] = { 0x00, 0x02, 0x03, 0x04, 
			     0x05, 0x06, 0x07, 0x08, 
			     0x09, 0x0A, 0x0B, 0x0C
                           };  

    RTPPacket::ptr p (RTPPacket::create(data, data+sizeof(data)));

    BOOST_CHECK_EQUAL( p->version(),            0x00u       );
    BOOST_CHECK_EQUAL( p->padding(),            0           );
    BOOST_CHECK_EQUAL( p->extension(),          0           );
    BOOST_CHECK_EQUAL( p->csrcCount(),          0x00u       );    
    BOOST_CHECK_EQUAL( p->marker(),             0           );
    BOOST_CHECK_EQUAL( p->payloadType(),        0x02u       );
    // the static_cast is to silence gcc-3.3
    BOOST_CHECK_EQUAL( static_cast<unsigned>(p->seqNumber()), 0x0304u );
    BOOST_CHECK_EQUAL( p->timestamp(),          0x05060708u );
    BOOST_CHECK_EQUAL( p->ssrc(),               0x090A0B0Cu );

}



///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
