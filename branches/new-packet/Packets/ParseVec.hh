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
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility.hpp>
#include <boost/range.hpp>
#include "PacketParser.hh"
#include "ParseArray.hh" // for Parse_Array_iterator

//#include "ParseVec.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    template <class ElementParser, class Sizer> class Parse_Vector_Container;

    template <class ElementParser, class Sizer>
    struct Parse_Vector : public PacketParserBase
    {
        Parse_Vector(data_iterator i, state_type s);
        Parse_Vector(Sizer sizer, data_iterator i, state_type s);

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef ElementParser value_type;
        typedef detail::Parse_Array_iterator<value_type> iterator;
        typedef Parse_Vector_Container<ElementParser,Sizer> container;

        size_type bytes() const;

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;

        value_type operator[](difference_type i) const;

     private:
        Sizer sizer_;

        friend class Parse_Vector_Container<ElementParser,Sizer>;
    };

    template <class SizeParser, unsigned offset=SizeParser::bytes>
    struct SimpleVectorSizer
    {
        typedef PacketParserBase::size_type size_type;
        typedef PacketParserBase::data_iterator iterator;
        typedef PacketParserBase::state_type state_type;

        size_type size(iterator i, state_type s) const;
        void size(iterator i, state_type s, size_type v) const;
        iterator begin(iterator i, state_type s) const;
        size_type bytes(iterator i, state_type s) const;
    };

    /** \brief

        Holds a reference to the container !
      */
    template <class ElementParser, class Sizer>
    class Parse_Vector_Container
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef Parse_Vector<ElementParser,Sizer> parser;
        typedef PacketParserBase::data_iterator data_iterator;
        typedef PacketParserBase::size_type size_type;
        typedef PacketParserBase::difference_type difference_type;
        typedef ElementParser value_type;
        typedef detail::Parse_Array_iterator<value_type> iterator;
        typedef boost::iterator_range<iterator> range_type;
        typedef PacketParserBase::state_type state_type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // no copy
        // default destructor
        // conversion constructors

        Parse_Vector_Container(parser const & vector);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Accessors
        ///@{

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;
        range_type range() const;

        value_type operator[](difference_type i) const;

        ///@}
        ///\name Mutators
        ///@{

        iterator shift(iterator pos, size_type n=1);
        template <class Value>
        void insert(iterator pos, Value const & t);
        template <class Value>
        void insert(iterator pos, size_type n, Value const & t);
        template <class ForwardIterator>
        void insert(iterator pos, ForwardIterator f, ForwardIterator l);

        void erase(iterator pos, size_type n=1);
        void erase(iterator f, iterator l);
        void clear();

        ///@}

        ///\name Parser interface
        ///@{

        data_iterator i() const;
        state_type state() const;
        PacketData & data() const;
        size_type bytes() const;
        
        ///@}

    protected:

    private:
        void setSize(size_type value);

        Sizer sizer_;
        PacketParserBase::state_type state_;
        size_type i_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ParseVec.cci"
#include "ParseVec.ct"
#include "ParseVec.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
