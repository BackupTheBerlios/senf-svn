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
    
    /** \brief Parser Baseclass

        To implement a new parser, derive from PacketParserBase and implement the necessary parser
        members:

        \code
          struct FooParser : public PacketParserBase
          {
              // This line will add the required standard members (constuctors)
              SENF_PACKET_PARSER_INIT(FooParser);

              // If this parser has a fixed size, you may optionally define this size here
              // This definition allows the parser to be used within the list, vector and array
              // parsers
              static size_type const bytes = some_constant_size;

              ///////////////////////////////////////////////////////////////////////////

              // Add here members returning (sub-)parsers for the fields. The 'parse' member is 
              // used to construct the sub-parsers. This member either takes an iterator to the
              // data to be parsed or just an offset in bytes.

              senf::Parse_UInt16 type() { return parse<Parse_UInt16>( 0 ); }
              senf::Parse_UInt16 size() { return parse<Parse_UInt16>( 2 ); }
          };

        \endcode
      */
    class PacketParserBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef detail::packet::iterator data_iterator;
        typedef detail::packet::size_type size_type;
        typedef detail::packet::difference_type difference_type;
        typedef detail::packet::byte byte;
        typedef PacketParserBase const & state;
        typedef PacketData * container;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        data_iterator i() const;

    protected:
        explicit PacketParserBase(container data);
        PacketParserBase(data_iterator i, state s);
        PacketParserBase(data_iterator i, state s, size_type size);

        bool check(size_type size);
        void validate(size_type size);

        template <class Parser> Parser parse(data_iterator i) const;
        template <class Parser> Parser parse(size_type n) const;

    private:
        data_iterator end();

        data_iterator i_;
        PacketData * data_;
    };

#   define SENF_PACKET_PARSER_INIT(name)                                \
        explicit name(container data) : senf::PacketParserBase(data) {} \
        name(data_iterator i, state s) : senf::PacketParserBase(i,s) {}

    struct VoidPacketParser 
        : public PacketParserBase
    {
        VoidPacketParser(container data);
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "PacketParser.cci"
//#include "PacketParser.ct"
#include "PacketParser.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
