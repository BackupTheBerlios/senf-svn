// $Id$
//
// Copyright (C) 2007
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
//   David Wagner <dw6@berlios.de>
/** \file
    \brief UNDatagramSocketHandle non-inline non-template implementation */

#include "UNDatagramSocketHandle.hh"
//#include "UNDatagramSocketHandle.ih"

// Custom includes
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <senf/Utils/Exception.hh>

//#include "UNDatagramSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::UNDatagramSocketProtocol::init_client() const
{
    int sock = ::socket(PF_UNIX,SOCK_DGRAM,0);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not create socket(PF_UNIX,SOCK_DGRAM,0).");
    fd(sock);
}

prefix_ void senf::UNDatagramSocketProtocol::init_client(UNSocketAddress const & address) const
{
    init_client();
    clientHandle().bind(address);
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
