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
    \brief PacketInterpreter public header */

#ifndef HH_PacketInterpreter_
#define HH_PacketInterpreter_ 1

// Custom includes
#include <boost/intrusive/ilist.hpp>
#include "Utils/intrusive_refcount.hh"
#include "Utils/pool_alloc_mixin.hh"
#include "PacketData.hh"

//#include "PacketInterpreter.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief
      */
    class PacketInterpreterBase
        : public PacketData, 
          public detail::packet::interpreter_list_base,
          public intrusive_refcount_t<PacketInterpreterBase>,
          public pool_alloc_mixin<PacketInterpreterBase>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::detail::packet::smart_pointer<
            PacketInterpreterBase>::ptr_t ptr;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // protected constructors
        // no conversion constructors
        
        static ptr create(size_type b, size_type e);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Interpreter chain access
        ///@{

        ptr next();
        ptr prev();
        ptr first();
        ptr last();

        ///@}

    protected:
        PacketInterpreterBase(size_type b, size_type e);

    private:
        // reference/memory management
        void add_ref();
        bool release();

        // containment management
        void assignImpl(detail::PacketImpl *);
        void releaseImpl();

        friend class detail::PacketImpl;
        friend class intrusive_refcount_t<PacketInterpreterBase>;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "PacketInterpreter.cci"
//#include "PacketInterpreter.ct"
//#include "PacketInterpreter.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
