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

/** \file
    \brief Parser framework
 */

#ifndef HH_ParserBase_
#define HH_ParserBase_ 1

// Custom includes
#include <boost/type_traits/is_member_function_pointer.hpp>

#include "ParserBase.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {
    
    struct nil { 
	// This is a fix for gcc-3.3 which checks out the implementation
	// even when only taking the address of an object ... sigh ...
	int operator[](int) const;
    };

    /** \brief Parser framework

        This class is the baseclass for all parser classes of the
        parser framework. The parser framework is used to interpret
        byte-oriented data from an arbitrary random access
        iterator. The framework is hierarchical in the sense, that
        parsers can be arbitrarily nested.

        All parser framework classes are as lightweight as
        possible. Most parser classes only have a single iterator as
        data member and (depending on the container) therefore have
        the same size as a single pointer. Parsers are therefore
        simple pointers decorated with type information.

        It is very important for parser classes to be lightweight and
        to have only simple constructors since parsers are passed
        around by value. Parser instances most of the time are
        temporaries. However, since they are only 'decorated'
        pointers, this should not have any performance impact.

        To implement a new parser, write a template implementing the
        following members:
        
        \code
            template <class Iterator=nil, class IPacket=nil>
            struct Parser_Example
                : protected satcom::pkf::ParserBase<Iterator,IPacket>
            {
                template <class I=nil, class P=nil>
                struct rebind { typedef Parse_Example<I,P> parser; }
                typedef Iterator byte_iterator;

                Parse_Example() {}
                Parse_Example(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}
        
                [static] unsigned bytes()
                {
                    // return the size of the parsed header. This
                    // method must be declared static if the size is
                    // constant, otherwise it must be a non-static
                    // member
                    return 14;
                }
                bool check(Iterator const & e)
                {
                    // return true, if the data in the range i() <-> e
                    // can be safely interpreted by the parser without
                    // causing invalid memory access. This means,
                    // check, wether the data is truncated
                    return e-this->i() >= static_cast<int>(bytes());
                }

                // methods to parse fields

                typedef Parse_UInt16 < Iterator >                    Parse_Field1;
                typedef Parse_Array  < 3, Parse_UInt32<>, Iterator > Parser_Field2

                Parse_Field1 field1() const { return Parse_Field1 (this->i()); }
                Parse_Field2 field2() const { return Parse_Field2 (this->i()+2); }
            };
        \endcode
        
        Every parser must have some mandatory fixed members which are:
        
        - struct rebind: This structure allows the parser to be
          converted to a parser of the same type but with a different
          iterator. Parser may have more than the two standard
          template parameters. These parameters must then be added in
          front of the standard parameters. The rebind structure must
          however always have only two parameters. Additional
          parameters must be provided from the outside template

        - byte_iterator: A typedef for the Iterator class used
        
        - default constructor: Every parser must have a default
          constructor. However, a standalone default-constructed
          parser is \e not dereferenceable. This error is \e not
          checked by the parser framework
        
        - Iterator constructor: This constructor must call the
          corresponding ParserBase constructor

        - unsigned bytes() member: This member must return the number
          of bytes the parser interprets. This will be the size of the
          implemented header. If the header has a fixed size, this
          member must be static, if it is dynamic the member must be
          non-static

        - bool check(Iterator e) member: This method must return true
          \e only if the range this->i() <-> e contains a \e complete
          packet, that is, e-this->i() >= bytes(). However, the call
          to bytes() might involve accessing data bytes which might
          not exist. The bytes() call cannot check this (it has no
          access to the \e end of the valid range). To keep the
          performance up, the validity check is performed once. The
          parser has to ensure, that validity is maintained even when
          changing the values. Validity in this context does not
          imply, that the packet is semantically correct, it only
          implies, that the packet can be parsed without risking
          invalid memory access.

        - The parser then contains any additional methods to parse the
          header constituents.

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

        Iterator i() const;

    private:

        Iterator i_;
    };

    template <class Parser>
    struct Parser_traits {
        typedef Parser parser;
        typedef typename Parser::byte_iterator byte_iterator;
        static const bool fixed_size = impl::Parser_traits_fixed_size<Parser>::fixed_size;

        template <class I=nil, class P=nil>
        struct rebind {
            typedef typename Parser::template rebind<I,P>::parser parser;
        };
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
