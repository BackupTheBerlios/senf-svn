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
    \brief PacketParser public header */

#ifndef HH_PacketParser_
#define HH_PacketParser_ 1

// Custom includes
#include "PacketTypes.hh"
#include "PacketData.hh"

//#include "PacketParser.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
    
    /** \brief
      */
    class PacketParserBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef detail::packet::iterator iterator;
        typedef detail::packet::size_type size_type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        iterator i() const;

    protected:
        PacketParserBase(iterator i, PacketData * data);
        PacketParserBase(iterator i, PacketData * data, size_type size);

        bool check(size_type size);
        void validate(size_type size);

    private:
        iterator end();

        iterator i_;
        PacketData * data_;
    };

    struct VoidPacketParser 
    {
        VoidPacketParser(detail::packet::iterator, PacketData *);
    };


}

///////////////////////////////hh.e////////////////////////////////////////
#include "PacketParser.cci"
//#include "PacketParser.ct"
//#include "PacketParser.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
