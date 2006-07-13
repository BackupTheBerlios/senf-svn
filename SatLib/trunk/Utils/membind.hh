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

#ifndef HH_membind_
#define HH_membind_ 1

// Custom includes
#include <boost/bind.hpp>
#include <boost/function.hpp>

///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

#define scOBTYPE T *
#include "Utils/impl/membind.hh"
#undef scOBTYPE

#define scOBTYPE T &
#include "Utils/impl/membind.hh"
#undef scOBTYPE

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "membind.cci"
//#include "membind.ct"
//#include "membind.cti"
#endif


// Local Variables:
// mode: c++
// End:
