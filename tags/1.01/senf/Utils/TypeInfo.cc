// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief TypeInfo non-inline non-template implementation */

#include "TypeInfo.hh"
//#include "TypeInfo.ih"

// Custom includes
#include <cxxabi.h>
#include <malloc.h>

//#include "TypeInfo.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ std::string senf::prettyName(std::type_info const & type)
{
    char const * mangled = type.name();
    int status (0);
    char * demangled ( abi::__cxa_demangle(mangled, 0, 0, &status) );
    std::string name (demangled ? demangled : mangled);
    if (demangled)
        ::free(demangled);
    return name;
}

prefix_ std::string senf::prettyBaseName(std::type_info const & type)
{
    std::string name (prettyName(type));
    std::string::size_type e (name.find('<'));
    if (e == std::string::npos) e = name.size();
    std::string::size_type b (name.rfind(':', e));
    if (b == std::string::npos) b = 0; else ++b;
    return name.substr(b,e-b);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "TypeInfo.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
