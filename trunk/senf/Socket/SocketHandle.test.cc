// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief SocketHandle unit tests */

//#include "SocketHandle.test.hh"
//#include "SocketHandle.test.ih"

// Custom includes
#include "SocketHandle.hh"
#include "SocketProtocol.test.hh"
#include "AddressingPolicy.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class MySocketHandle
        : public senf::SocketHandle<senf::test::SomeSocketProtocol::Policy>
    {
    public:
        MySocketHandle()
            : senf::SocketHandle<senf::test::SomeSocketProtocol::Policy>(
                std::auto_ptr<senf::SocketBody>(
                    new senf::ProtocolSocketBody<senf::test::SomeSocketProtocol>(false, 0)))
            {}
    };

    class FDHandle
        : public senf::FileHandle
    {
    public:
        FDHandle() : senf::FileHandle(std::auto_ptr<senf::FileBody>(new senf::FileBody())) {}
    };

}

SENF_AUTO_UNIT_TEST(socketHandle)
{
    typedef senf::MakeSocketPolicy<
        senf::test::SomeCommunicationPolicy,
        senf::test::SomeReadPolicy
        >::policy OtherSocketPolicy;
    typedef senf::SocketHandle<OtherSocketPolicy> OtherSocketHandle;

    typedef senf::MakeSocketPolicy<
        senf::test::SomeCommunicationPolicy,
        senf::test::SomeAddressingPolicy
        >::policy AnotherSocketPolicy;
    typedef senf::SocketHandle<AnotherSocketPolicy> AnotherSocketHandle;

    {
        MySocketHandle myh;
        OtherSocketHandle osh (myh);
        osh = myh;

        typedef senf::SocketHandle<senf::test::SomeSocketProtocol::Policy> SomeSocketHandle;
        SomeSocketHandle ssh = senf::static_socket_cast<SomeSocketHandle>(osh);

        SENF_CHECK_NO_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(osh) );
        SENF_CHECK_NO_THROW( senf::dynamic_socket_cast<AnotherSocketHandle>(osh) );

        typedef senf::SocketHandle< senf::MakeSocketPolicy<
            OtherSocketPolicy,
            senf::NoAddressingPolicy
            >::policy> SomeOtherSocketHandle;

        BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeOtherSocketHandle>(osh),
                           std::bad_cast );
        BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(
                               senf::FileHandle(FDHandle())),
                           std::bad_cast );

        BOOST_CHECK_EQUAL( myh.dumpState(),
                "file.handle: 0\n"
                "file.refcount: 3\n"
                "handle: senf::SocketHandle<senf::SocketPolicy<senf::test::SomeAddressingPolicy, senf::test::SomeFramingPolicy, senf::test::SomeCommunicationPolicy, senf::test::SomeReadPolicy, senf::test::SomeWritePolicy> >\n"
                "socket.protocol: senf::test::SomeSocketProtocol\n"
                "socket.protocol.policy: senf::SocketPolicy<senf::test::SomeAddressingPolicy, senf::test::SomeFramingPolicy, senf::test::SomeCommunicationPolicy, senf::test::SomeReadPolicy, senf::test::SomeWritePolicy>\n"
                "socket.server: false\n"
                "valid: true\n" );

        SENF_CHECK_NO_THROW( myh.facet<senf::test::SomeSocketProtocol>() );
    }

    // Ensure, the destructor is called and calls the correct close() implementation
    BOOST_CHECK( senf::test::SomeSocketProtocol::closeCount() >= 1u );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
