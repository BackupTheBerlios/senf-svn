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

/** \file
    \brief ServerSocketHandle unit tests */

//#include "ServerSocketHandle.test.hh"
//#include "ServerSocketHandle.test.ih"

// Custom includes
#include "ServerSocketHandle.hh"
#include "ClientSocketHandle.hh"
#include "SocketProtocol.test.hh"
#include "AddressingPolicy.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    class MySocketHandle
        : public senf::ServerSocketHandle<senf::test::SomeSocketProtocol::Policy>
    {
    public:
        MySocketHandle()
            : senf::ServerSocketHandle<senf::test::SomeSocketProtocol::Policy>(
                std::auto_ptr<senf::SocketBody>(
                    new senf::ProtocolSocketBody<senf::test::SomeSocketProtocol>(true)))
            {}
    };

}

SENF_AUTO_UNIT_TEST(serverSocketHandle)
{
    typedef senf::MakeSocketPolicy<
        senf::test::SomeFramingPolicy,
        senf::test::SomeReadPolicy,
        senf::test::SomeWritePolicy
        >::policy OtherSocketPolicy;
    typedef senf::SocketHandle<OtherSocketPolicy> OtherSocketHandle;

    MySocketHandle myh;
    OtherSocketHandle osh (myh);
    osh = myh;

    typedef senf::ServerSocketHandle<senf::test::SomeSocketProtocol::Policy> SomeSocketHandle;
    SomeSocketHandle ssh = senf::static_socket_cast<SomeSocketHandle>(osh);

    typedef senf::ServerSocketHandle<senf::MakeSocketPolicy<
        OtherSocketPolicy,
        senf::NoAddressingPolicy
        >::policy> SomeOtherSocketHandle;
    typedef senf::ClientSocketHandle<OtherSocketPolicy> OtherClientHandle;

    SENF_CHECK_NO_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(osh) );
    BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeOtherSocketHandle>(osh),
                       std::bad_cast );
    BOOST_CHECK_THROW( senf::dynamic_socket_cast<OtherClientHandle>(osh),
                       std::bad_cast );

    SENF_CHECK_NO_THROW( myh.bind(0) );
    BOOST_CHECK_EQUAL( myh.local(), 2u );

    {
        MySocketHandle::ClientHandle client = myh.accept();
        BOOST_CHECK_EQUAL( client.fd(), -1 );
    }

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
