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

//#include "ParseVec.test.hh"
//#include "ParseVec.test.ih"

// Custom includes
#include "ParseVec.hh"
#include "ParseInt.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace satcom::pkf;

BOOST_AUTO_UNIT_TEST(parseVec_test)
{
    unsigned char data[] = { 0x03,                                   // size
                             0x10, 0x11,  0x12, 0x13,  0x14, 0x15,   // data
                             0x20, 0x21,  0x22, 0x23,  0x24, 0x25 };
    typedef unsigned char * iterator;
    typedef Parse_Vector<Parse_UInt16<>,Parse_UInt8<>,iterator> Parse_UInt16Vec;

    Parse_UInt8<iterator> sizeParser (data);
    Parse_UInt16Vec v (sizeParser, data+1);
    
    BOOST_CHECK_EQUAL( v[0], 0x1011 );
    BOOST_CHECK_EQUAL( v[2], 0x1415 );
    BOOST_CHECK_EQUAL( v.size(), 3u );
    BOOST_CHECK_EQUAL( v.bytes(), 6u );
    data[0] = 0x06;
    BOOST_CHECK_EQUAL( v.size(), 6u );
    BOOST_CHECK_EQUAL( v.bytes(), 12u );
    
    iterator i (data+1);
    Parse_UInt16Vec::iterator j (v.begin());
    Parse_UInt16Vec::iterator e (v.end());
    for (;j!=e;++j, i+=2)
        BOOST_CHECK_EQUAL( Parse_UInt16<iterator>(i), *j );
    BOOST_CHECK_EQUAL(i, data+13);
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
