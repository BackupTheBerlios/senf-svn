// $Id: SNDUPacket.cc 423 2007-08-31 22:05:37Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
    \brief DSMCCSection non-inline non-template implementation */

#include "TLVPacket.hh"
//#include "TLVPacket.ih"

// Custom includes
#include <iomanip>
#include "../../Utils/hexdump.hh"
#include "../../Packets/DefaultBundle/EthernetPacket.hh"


#define prefix_
///////////////////////////////cc.p////////////////////////////////////////


prefix_ senf::PacketParserBase::size_type senf::Parse_TLVPacket::bytes()
    const
{
    return 4 + senf::bytes( length() );
}

//prefix_ void senf::TLVPacketType::init(packet p)
//{
//    p->init();
//}

prefix_ void senf::TLVPacketType::dump(packet p, std::ostream & os)
{
    os << "TLVPacket:\n"
       << std::dec
       << "  type: " <<  unsigned(p->type()) << "\n"
       << "  length: " << unsigned(p->length()) << "\n";
}

prefix_ senf::PacketParserBase::size_type senf::TLVPacketType::initSize()
{
    return 4 + 1;
}

prefix_ senf::PacketParserBase::size_type senf::TLVPacketType::initHeadSize()
{
    return 4 + 1;
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
