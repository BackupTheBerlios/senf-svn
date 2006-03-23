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

#ifndef HH_ParseVec_
#define HH_ParseVec_ 1

// Custom includes
#include <utility> // for std::pair
#include <boost/iterator/iterator_facade.hpp>
#include "ParserBase.hh"
#include "ParseArray.hh" // for Parser_Array_iterator

//#include "ParseVec.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {

    template <class Parser, class SizeParser, class Iterator=nil, class IPacket=nil>
    struct Parse_Vector : protected ParserBase<Iterator,IPacket>
    {
        ///////////////////////////////////////////////////////////////////////////
        // Parser interface

        template <class I=nil, class P=nil> 
        struct rebind { typedef Parse_Vector<Parser,I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_Vector(SizeParser const & size);
        explicit Parse_Vector(SizeParser const & size, Iterator const & i);
        
        unsigned bytes();
        void init() const;

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef typename Parser::template rebind<Iterator>::parser value_type;
        typedef impl::Parse_Array_iterator<value_type,Iterator> iterator;
        typedef unsigned size_type;
        typedef int difference_type;
        typedef std::pair<iterator,iterator> range_type;

        size_type size() const;

        iterator begin() const;
        iterator end() const;
        range_type range() const;
        range_type value() const;

        value_type operator[](difference_type i) const;

    private:
        SizeParser size_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ParseVec.cci"
//#include "ParseVec.ct"
#include "ParseVec.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
