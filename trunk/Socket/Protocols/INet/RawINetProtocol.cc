// $Id: RawINetProtocol.cc 597 2008-01-15 09:16:20Z g0dil $
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


#include "RawINetProtocol.hh"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ
#include <net/if.h> // for if_nametoindex
#include "../../../Socket/SocketHandle.hh"

//#include "UDPProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ unsigned senf::RawINetProtocol::available()
    const
{
    int n;
    if (::ioctl(fd(),SIOCINQ,&n) < 0)
        throwErrno();
    return n;
}

prefix_ bool senf::RawINetProtocol::eof()
    const
{
    return false;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "UDPProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End: