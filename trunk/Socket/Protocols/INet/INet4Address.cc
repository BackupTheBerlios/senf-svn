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
    \brief INet4Address non-inline non-template implementation */

#include "INet4Address.hh"
//#include "INet4Address.ih"

// Custom includes
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#ifdef _REENTRANT
#include <boost/thread/mutex.hpp>
#endif

//#include "INet4Address.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::INet4Address::INet4Address

prefix_ senf::INet4Address::INet4Address(address_type value)
    : addr_(htonl(value))
{}

prefix_ senf::INet4Address senf::INet4Address::from_string(std::string const & s)
{
    ::in_addr ina;
    if (::inet_pton(AF_INET,s.c_str(),&ina) > 0)
        return senf::INet4Address::from_inaddr(ina.s_addr);
#   ifdef _REENTRANT
    static boost::mutex mutex;
    boost::mutex::scoped_lock lock(mutex);
#   endif
    ::hostent * ent (::gethostbyname(s.c_str()));
    if (!ent)
        ///\fixme Need to give better exception here
        throw SyntaxException(); 
    if (ent->h_addrtype != AF_INET)
        throw SyntaxException();    
    // We are only interested in the first address ...
    return senf::INet4Address::from_inaddr(
        reinterpret_cast<in_addr*>(*(ent->h_addr_list))->s_addr);
}

prefix_ bool senf::INet4Address::local()
    const
{
    address_type l (ntohl(addr_));
    return 
        (l & 0xFF000000u) == 0x0A000000u ||
        (l & 0xFFF00000u) == 0xAC100000u ||
        (l & 0xFFFF0000u) == 0xA9FE0000u ||
        (l & 0xFFFF0000u) == 0xC0A80000u;
}

prefix_ bool senf::INet4Address::loopback()
    const
{
    return (ntohl(addr_) & 0xFF000000u) == 0x7F000000u;
}

prefix_ bool senf::INet4Address::multicast()
    const
{
    return (ntohl(addr_) & 0xF0000000u) == 0xE0000000u;
}

senf::INet4Address const senf::INet4Address::None;
senf::INet4Address const senf::INet4Address::Loopback = senf::INet4Address(0x7F000001u);
senf::INet4Address const senf::INet4Address::Broadcast = senf::INet4Address(0xFFFFFFFFu);


///////////////////////////////////////////////////////////////////////////
// namespace members

prefix_ std::ostream & senf::operator<<(std::ostream & os, INet4Address const & addr)
{
    ::in_addr ina;
    char buffer[16];
    ina.s_addr = addr.raw();
    ::inet_ntop(AF_INET,&ina,buffer,16);
    buffer[15] = 0; 
    os << buffer;
    return os;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "INet4Address.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
