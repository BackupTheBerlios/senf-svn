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
    \brief pool_alloc_mixin public header */

#ifndef HH_pool_alloc_mixin_
#define HH_pool_alloc_mixin_ 1

// Custom includes
#include <memory>

//#include "pool_alloc_mixin.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    struct pool_alloc_mixin_tag;

    template <class Self>
    class pool_alloc_mixin
    {
    public:
        static void * operator new (size_t size);
        static void operator delete (void *p, size_t size);

#ifndef NDEBUG
        static unsigned long allocCounter();
    private:
        static unsigned long allocCounter(long delta);
#endif
    };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "pool_alloc_mixin.cci"
//#include "pool_alloc_mixin.ct"
#include "pool_alloc_mixin.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
