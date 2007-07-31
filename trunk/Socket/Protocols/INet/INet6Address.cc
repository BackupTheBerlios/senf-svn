// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief INet6Address non-inline non-template implementation */

#include "INet6Address.hh"
#include "INet6Address.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//#include "INet6Address.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::INet6Address

prefix_ senf::INet6Address senf::INet6Address::from_string(std::string const & s,
                                                           Resolve_t resolve)
{
    struct in6_addr ina;
    if (::inet_pton(AF_INET6,s.c_str(),&ina) > 0)
        return senf::INet6Address::from_data(&ina.s6_addr[0]);
    int herr (0);

    // If available, we use the reentrant GNU variant. This has the additional advantage, that we
    // can explicitly ask for IpV4 addresses

#   ifdef __GLIBC__

    struct hostent entbuf;
    char buffer[4096];
    struct hostent * ent (0);
    ::gethostbyname2_r(s.c_str(), AF_INET6, &entbuf, buffer, sizeof(buffer), &ent, &herr);

#   else // ! __GLIBC__

#   ifdef _REENTRANT
    static boost::mutex mutex;
    boost::mutex::scoped_lock lock(mutex);
#   endif
    struct hostent * ent (::gethostbyname(s.c_str()));
    herr = h_errno;

#   endif // __GLIBC__

    if (ent && ent->h_addrtype == AF_INET6)
        // We are only interested in the first address ...
        return senf::INet6Address::from_data(
            &reinterpret_cast<in6_addr*>(*(ent->h_addr_list))->s6_addr[0]);

    ///\todo Throw better exceptions here ?

    if (resolve == ResolveINet4)
        try {
            return from_inet4address(INet4Address::from_string(s));
        } catch (INet4Address::SyntaxException const & ex) {
            throw SyntaxException();
        }
    else
        throw SyntaxException();
}

prefix_ std::ostream & senf::operator<<(std::ostream & os, INet6Address const & addr)
{
    ::in6_addr ina;
    char buffer[5*8];
    std::copy(addr.begin(),addr.end(),&ina.s6_addr[0]);
    ::inet_ntop(AF_INET6,&ina,buffer,sizeof(buffer));
    buffer[5*8] = 0;
    os << buffer;
    return os;
}

senf::INet6Address const senf::INet6Address::None;
senf::INet6Address const senf::INet6Address::Loopback   (0u,0u,0u,0u,0u,0u,0u,1u);
senf::INet6Address const senf::INet6Address::AllNodes   (0xFF02u,0u,0u,0u,0u,0u,0u,1u);
senf::INet6Address const senf::INet6Address::AllRouters (0xFF02u,0u,0u,0u,0u,0u,0u,2u);

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "INet6Address.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
