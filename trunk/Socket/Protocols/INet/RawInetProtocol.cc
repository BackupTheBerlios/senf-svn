// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum NETwork research (NET)
//     David Wagner <david.wagner@fokus.fraunhofer.de>
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


#include "RawInetProtocol.hh"

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

prefix_ unsigned senf::RawInetProtocol::available()
    const
{
    int n;
    if (::ioctl(body().fd(),SIOCINQ,&n) < 0)
        throwErrno();
    return n;
}

prefix_ bool senf::RawInetProtocol::eof()
    const
{
    return false;
}
#undef prefix_