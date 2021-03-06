// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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

//#include "ClientSocketHandle.test.hh"
//#include "ClientSocketHandle.test.ih"

// Custom includes
#include "SocketPolicy.test.hh"
#include "SocketProtocol.test.hh"
#include "ClientSocketHandle.hh"
#include "AddressingPolicy.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    class MySocketHandle
        : public senf::ClientSocketHandle<senf::test::SomeSocketProtocol::Policy>
    {
    public:
        MySocketHandle()
            : senf::ClientSocketHandle<senf::test::SomeSocketProtocol::Policy>(
                std::auto_ptr<senf::SocketBody>(
                    new senf::ProtocolSocketBody<senf::test::SomeSocketProtocol>(false)))
            {}
    };

}

BOOST_AUTO_UNIT_TEST(clientSocketHandle)
{
    BOOST_CHECKPOINT("Constructing socket handle");
    MySocketHandle myh;

    // conversion to other socket handles
    {
        typedef senf::MakeSocketPolicy<
            senf::test::SomeFramingPolicy,
            senf::test::SomeReadPolicy,
            senf::test::SomeWritePolicy
            >::policy OtherSocketPolicy;
        typedef senf::SocketHandle<OtherSocketPolicy> OtherSocketHandle;

        BOOST_CHECKPOINT("Copy-constructing socket handle");
        OtherSocketHandle osh (myh);
        BOOST_CHECKPOINT("Assigning socket handle");
        osh = myh;
        typedef senf::ClientSocketHandle<senf::test::SomeSocketProtocol::Policy> SomeSocketHandle;
        BOOST_CHECKPOINT("static_casting socket handle");
        SomeSocketHandle ssh =
            senf::static_socket_cast<SomeSocketHandle>(osh);
        BOOST_CHECK_NO_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(osh) );
        typedef senf::ClientSocketHandle<senf::MakeSocketPolicy<
            OtherSocketPolicy,
            senf::NoAddressingPolicy
            >::policy> SomeOtherSocketHandle;
        BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeOtherSocketHandle>(osh),
                           std::bad_cast );
    }

    // reading and writing
    BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.read(), "TEST-READ" ) );
    {
        std::string buf("FOO-BAR");
        BOOST_CHECK_NO_THROW( myh.read(buf,0) );
        BOOST_CHECK_EQUAL( buf, "TEST-READ" );
    }
    {
        char buf[11];
        ::strcpy(buf,"0123456789");
        BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.read(buf,buf+10), buf+9 ) );
        BOOST_CHECK_EQUAL( buf, "TEST-READ9" );
    }

    BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.readfrom().first, "TEST-READ" ) );
    {
        std::string buf("FOO-BAR");
        unsigned addr;
        BOOST_CHECK_NO_THROW( myh.readfrom(buf,addr,0) );
        BOOST_CHECK_EQUAL( buf, "TEST-READ" );
    }
    {
        char buf[11];
        unsigned addr;
        ::strcpy(buf,"0123456789");
        BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.readfrom(buf,buf+10,addr), buf+9 ) );
        BOOST_CHECK_EQUAL( buf, "TEST-READ9" );
    }

    {
        std::string s ("TEST-WRITE");
        BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.write(s)-s.begin(), 10 ) );
        s = "TEST";
        // This simulates a closed file in this test policy. However, we
        // have changed the semantics so this will not work anymore.
        // BOOST_CHECK_THROW( myh.write(s),senf::SystemException );
        char const * const s1 = "TEST-WRITE9";
        BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.write(s1,s1+10), s1+10u ) );
        s = "TEST-WRITE";
        BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.writeto(0,s)-s.begin(), 10 ) );
        BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.writeto(0,s1,s1+10), s1+10 ) );
    }

    BOOST_CHECK_NO_THROW( myh.connect(0) );
    BOOST_CHECK_NO_THROW( myh.bind(0) );
    BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.peer(), 1u ) );
    BOOST_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.local(), 2u ) );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
