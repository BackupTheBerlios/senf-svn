// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Definition of non-inline non-template functions

#include "TypeInfo.hh"
//#include "TypeInfo.ih"

// Custom includes
#include "malloc.h"
#define HAVE_DECL_BASENAME 1
#define HAVE_DECL_ASPRINTF 1
#define HAVE_DECL_VASPRINTF 1
#include "impl/demangle.h"

//#include "TypeInfo.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////


// WARNING: This is completely g++ and libiberty dependent. The demangling 
// interface isn't even explicitly exportet from libiberty. However, it is 
// *EXTREMELY* helpful for debugging ...

prefix_ std::string satcom::lib::prettyName(std::type_info const & type)
{
    char const * mangled = type.name();
    char * demangled = ::cplus_demangle(mangled,DMGL_TYPES|DMGL_AUTO);
    std::string name (demangled ? demangled : mangled);
    if (demangled) 
        ::free(demangled);
    return name;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "TypeInfo.mpp"


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End: