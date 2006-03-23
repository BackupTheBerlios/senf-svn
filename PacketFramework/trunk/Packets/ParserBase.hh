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

#ifndef HH_ParserBase_
#define HH_ParserBase_ 1

// Custom includes

//#include <Packet.mpp>
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {
    
    struct nil {};

    /** \brief Baseclass providing iterator access for packet parsers

        ParserBase provides the parser classes with access to the
        packet iterator. This class is templatized on the Iterator
        type and an optional baseclass type.

        If the baseclass is given, it is used to access the iterator
        directly using 'begin'. If it is not given, the instance has
        to be constructed with an iterator.

        This implementation ensures, that a parser can either be
        inherited into a Packet class or be used as a temporary.
      */
    template <class Iterator, class IPacket=nil>
    class ParserBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{
        
        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

    protected:

        Iterator i() const;

    private:
        
    };

    template <class Iterator>
    class ParserBase<Iterator,nil>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit ParserBase(Iterator const & i);

        // no default constructor
        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

    protected:

        Iterator i() const;

    private:

        Iterator i_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ParserBase.cci"
//#include "ParserBase.ct"
#include "ParserBase.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
