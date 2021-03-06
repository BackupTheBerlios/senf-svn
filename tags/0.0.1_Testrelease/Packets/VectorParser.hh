// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief VectorParser public header */

#ifndef HH_VectorParser_
#define HH_VectorParser_ 1

// Custom includes
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility.hpp>
#include <boost/range.hpp>
#include <boost/type_traits.hpp>
#include "PacketParser.hh"
#include "ArrayParser.hh" // for ArrayParser_iterator

//#include "VectorParser.mpp"
#include "VectorParser.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    template <class ElementParser, class Sizer> class VectorParser_Container;

    /** \brief Collection of fixed-size elements

        A Vector is a collection of fixed-size elements of which the size of the collection can be
        determined directly (that is without traversing the collection). This allows very efficient
        random access to the elements of the collection.
        
        A vector is a model of an STL random-access sequence. The parser only provides a reduced
        interface, the container wrapper however completes this interface.

        VectorParser makes use of a policy template argument, \a Sizer, to customize the way the
        containers size is obtained. You will normally not instantiate Parser_Vector directly, you
        will use one of the 'template typedefs' (which are templated structures since C++ does not
        provide real template typedefs) provided with the policy implementations.
        
        \todo Make the sizer a private base-class to profit from the empty-base-class optimization

        \see ExampleVectorPolicy
        \ingroup parsecollection
     */
    template <class ElementParser, class Sizer>
    struct VectorParser : public PacketParserBase
    {
        VectorParser(data_iterator i, state_type s);
        VectorParser(Sizer sizer, data_iterator i, state_type s);
                                        ///< Additional sizer specific constructor
                                        /**< This constructor may be used, if the sizer needs
                                             additional parameters. */

        size_type bytes() const;
        void init() const;

        static const size_type init_bytes = Sizer::init_bytes;

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef ElementParser value_type;
        typedef detail::ArrayParser_iterator<value_type> iterator;
        typedef iterator const_iterator;
        typedef VectorParser_Container<ElementParser,Sizer> container;

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;

        value_type operator[](difference_type i) const;
        value_type front() const;
        value_type back() const;

        // Mutators
        
        // The mutators provided here are those which don't take an iterator argument.
        // If you need to pass an iterator it is much simpler and cleaner to use the
        // 'container' wrapper
                   
        template <class Value> void push_back        (Value value, size_type n=1) const;
                               void push_back_space  (size_type n=1) const;
        template <class Value> void push_front       (Value value, size_type n=1) const;
                               void push_front_space (size_type n=1) const;
                               void resize           (size_type n) const;
        template <class Value> void resize           (size_type n, Value value) const;

     private:
        Sizer sizer_;

        friend class VectorParser_Container<ElementParser,Sizer>;
    };

    /** \brief Vector with prefix sizing
        
        This is a 'template typedef'. It defines a vector with a <em>directly preceding</em> size
        field holding the number of vector elements. The size field is considered part of the
        vector.
        \code
        // Define MyVector as a vector of 16bit unsigned elements with a directly preceding
        // 8bit unsigned size field
        typedef senf::VectorNParser<senf::UInt16Parser, senf::UInt8Parser>::parser MyVector;
        \endcode

        \param ElementParser \e fixed-size parser for parsing the vector elements
        \param SizeParser parser for parsing the vector size (number of elements)

        \see VectorParser
        \ingroup parsecollection
     */
    template <class ElementParser, class SizeParser, unsigned Distance>
    struct VectorNParser
    {
        typedef VectorParser< ElementParser,
                              detail::VectorNParser_Sizer<SizeParser, Distance> > parser;
    };

    /** \brief Define VectorNParser field
        
        This macro is a special helper to define a senf::VectorNParser type field, a vector of
        elements of type \a elt_type (a parser) which size is given by the \a size field.

        \code
        // The size field should be declared private (size is accessible via the vector)
        SENF_PARSER_PRIVATE_FIELD ( vec_size_, senf::UInt16Parser );
        // Define the vector, here it has 32bit unsigned integer elements
        SENF_PARSER_VEC_N         ( vec,       _vec_size, senf::UInt32Parser );
        \endcode
        
        \param[in] name field name
        \param[in] size name of field giving the vector size
        \param[in] elt_type vector element type
        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_VEC_N(name, size, elt_type)                                                \
        SENF_PARSER_VEC_N_I(SENF_PARSER_FIELD, name, size, elt_type)

    /** \brief Define VectorNParser field

        \see \ref SENF_PARSER_VEC_N()
        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_PRIVATE_VEC_N(name, size, elt_type)                                        \
        SENF_PARSER_VEC_N_I(SENF_PARSER_PRIVATE_FIELD, name, size, elt_type)

    /** \brief VectorParser container wrapper

        This is the container wrapper used for vector parsers. The container wrapper will stay valid
        after changing the collection. However the container still depends on the packet and will be
        invalidated if the Packet is deallocated or if the packet size is changed from without the
        container wrapper (more precisely, it is invalided if the insertion/deletion happens before
        the vector in the packet data).

        The vector container wrapper provides a complete STL random-access sequence interface.
        
        \code
        SomePacket p (...);
        SomePacket::aVectorCollection_t::container c (p->aVectorCollection());
        c.insert(c.begin(), ... );
        \endcode

        \see VectorParser
      */
    template <class ElementParser, class Sizer>
    class VectorParser_Container
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef VectorParser<ElementParser,Sizer> parser_type;
        typedef PacketParserBase::data_iterator data_iterator;
        typedef PacketParserBase::size_type size_type;
        typedef PacketParserBase::difference_type difference_type;
        typedef ElementParser value_type;
        typedef detail::ArrayParser_iterator<value_type> iterator;
        typedef iterator const_iterator;
        typedef PacketParserBase::state_type state_type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy
        // default destructor
        // conversion constructors

        VectorParser_Container(parser_type const & vector);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Accessors
        ///@{

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;

        value_type operator[](difference_type i) const;
        value_type front() const;
        value_type back() const;

        ///@}
        ///\name Mutators
        ///@{

        iterator shift(iterator pos, size_type n=1);
        template <class Value>
        void insert(iterator pos, Value const & t);
        template <class Value>
        void insert(iterator pos, size_type n, Value const & t);
#       ifndef DOXYGEN
        template <class ForwardIterator>
        void insert(iterator pos, ForwardIterator f, ForwardIterator l,
                    typename boost::disable_if< boost::is_convertible<ForwardIterator,size_type> >::type * = 0);
#       else
        template <class ForwardIterator>
        void insert(iterator pos, ForwardIterator f, ForwardIterator l);
#       endif

        void erase(iterator pos, size_type n=1);
        void erase(iterator f, iterator l);
        void clear();

        template <class Value> void push_back        (Value value, size_type n=1);
                               void push_back_space  (size_type n=1);
        template <class Value> void push_front       (Value value, size_type n=1);
                               void push_front_space (size_type n=1);
                               void resize           (size_type n);
        template <class Value> void resize           (size_type n, Value value);

        ///@}

        ///\name Parser interface
        ///@{

        parser_type parser() const;
        data_iterator i() const;
        state_type state() const;
        PacketData & data() const;

        size_type bytes() const;
        void init() const;
        
        ///@}

    protected:

    private:
        void setSize(size_type value);

        Sizer sizer_;
        state_type state_;
        size_type i_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_VectorParser_i_)
#define HH_VectorParser_i_
//#include "VectorParser.cci"
#include "VectorParser.ct"
#include "VectorParser.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
