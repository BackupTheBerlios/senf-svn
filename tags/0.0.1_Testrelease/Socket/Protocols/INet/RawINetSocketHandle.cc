// $Id: RawINetSocketHandle.cc 597 2008-01-15 09:16:20Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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

#include "RawINetSocketHandle.hh"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "../../../Utils/Exception.hh"

//#include "UDPSocketHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::RawV4SocketProtocol

prefix_ void senf::RawV4SocketProtocol::init_client()
    const
{
    init_client(IPPROTO_RAW);
}

prefix_ void
senf::RawV4SocketProtocol::init_client(int const & protocol)
    const
{
    int sock = ::socket(PF_INET, SOCK_RAW, protocol);
    if (sock < 0)
        throw SystemException();
    fd(sock);
}

prefix_ void
senf::RawV4SocketProtocol::init_client(int const & protocol, INet4SocketAddress const & address)
    const
{
    init_client(protocol);
    clientHandle().bind(address);
}

///////////////////////////////////////////////////////////////////////////
// senf::RawV6SocketProtocol::

prefix_ void senf::RawV6SocketProtocol::init_client()
    const
{
    init_client(IPPROTO_RAW);
}

prefix_ void senf::RawV6SocketProtocol::init_client(int const & protocol)
    const
{
    int sock = ::socket(PF_INET6,SOCK_RAW,protocol);
    if (sock < 0)
        throw SystemException();
    fd(sock);
}

prefix_ void
senf::RawV6SocketProtocol::init_client(int const & protocol, INet6SocketAddress const & address)
    const
{
    init_client(protocol);
    clientHandle().bind(address);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "RawINetSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
