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
    \brief SafePacketParser public header */

#ifndef HH_SafePacketParser_
#define HH_SafePacketParser_ 1

// Custom includes
#include "Utils/SafeBool.hh"
#include "safe_data_iterator.hh"

//#include "SafePacketParser.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief
      */
    template <class Parser>
    class SafePacketParser
        : public SafeBool< SafePacketParser<Parser> >
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor
        SafePacketParser();

        // conversion constructors
        SafePacketParser(Parser parser);

        SafePacketParser & operator=(Parser parser);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Parser operator*() const;
        bool boolean_test() const;

    protected:

    private:
        senf::safe_data_iterator i_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "SafePacketParser.cci"
//#include "SafePacketParser.ct"
#include "SafePacketParser.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
