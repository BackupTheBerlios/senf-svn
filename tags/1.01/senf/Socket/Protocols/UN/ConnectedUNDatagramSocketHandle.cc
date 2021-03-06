// $Id$
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

/** \file
    \brief ConnectedUNDatagramSocketHandle non-inline non-template implementation */

#include "ConnectedUNDatagramSocketHandle.hh"

// Custom includes
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <senf/Utils/Exception.hh>

//#include "UNDatagramSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::ConnectedUNDatagramSocketProtocol::init_client() const
{
    int sock = ::socket(PF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not create socket(PF_UNIX,SOCK_DGRAM,0).");
    fd(sock);
}

prefix_ void senf::ConnectedUNDatagramSocketProtocol::init_client(UNSocketAddress const & address) const
{
    init_client();
    clientHandle().connect(address);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "UNDatagramSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
