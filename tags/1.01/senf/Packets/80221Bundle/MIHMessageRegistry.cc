// $Id$
//
// Copyright (C) 2011
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief MIH Message-Registry non-inline non-template implementation */

#include "MIHMessageRegistry.hh"
//#include "MIHMessageRegistry.ih"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::MIHMessageRegistry

#if BOOST_VERSION < 103400
#   define PTRMAP_GET_CONTENTS(v) (v)
#else
#   define PTRMAP_GET_CONTENTS(v) (*(v).second)
#endif

prefix_ void senf::MIHMessageRegistry::validate(key_t messageId, senf::Packet message)
{
    Map::const_iterator i (map_.find( messageId));
    if (i != map_.end())
        PTRMAP_GET_CONTENTS(*i).validate( message);
}

#undef PTRMAP_GET_CONTENTS

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
