// $Id$
//
// Copyright (C) 2007
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
    \brief VariantParser public header */

#ifndef HH_VariantParser_
#define HH_VariantParser_ 1

#ifndef HH_Packets_
#error "Don't include 'VariantParser.hh' directly, include 'Packets.hh'"
#endif

// Custom includes
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include "PacketParser.hh"

//#include "VariantParser.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

#   ifndef SENF_LIMIT_PARSE_VARIANT
        /** \brief Maximum number of senf::VariantParser sub-parsers.

            This number defines the maximum number of parser arguments senf::VariantParser takes.
         */
#       define SENF_LIMIT_PARSE_VARIANT 6
#   endif

    /** \brief Variant parser

        This is not really a collection parser (it does not provide a collection
        interface). However, it is not a composite parser or value parser either.

        A variant parser will parse any number of sub-parsers discriminated by an arbitrary, policy
        defined condition. This is the parser to use, if the type and/or number of fields of a
        packet change depending on some condition.
        \code
        typedef senf::VariantParser< 
            MyAuxPolicy, 
            senf::mpl::vector<senf::VoidPacketParser, TypeAParser, TypeBParser> > MyVariantParser;
        \endcode
        This typedef defines a variant parser choosing one of three sub
        parsers. senf::VoidPacketParser is an empty parser, it effectively makes this parser
        optional.

        When creating a new packet containing a variant parser, the variant parser will always be
        initialized to the first sub-parser.

        \see 
            ExampleAuxPolicy on how to implement the \a AuxPolicy \n
            \ref SENF_PARSER_VARIANT() on how to integrate the parser into another parser
        \ingroup parsecollection
     */
    template <class AuxPolicy, class Parsers>
    class VariantParser 
        : public PacketParserBase, private AuxPolicy
    {
        typedef Parsers parsers;

    public:
        ///\name Parser interface
        ///\{

        VariantParser(data_iterator i, state_type s);
        VariantParser(AuxPolicy policy, data_iterator i, state_type s);

        size_type bytes() const;
        void init();
        
        static const size_type init_bytes = senf::init_bytes< 
            typename boost::mpl::at<parsers, boost::mpl::int_<0> >::type>::value 
                + AuxPolicy::aux_bytes;

        ///\}
        ///////////////////////////////////////////////////////////////////////////

        unsigned variant() const;       ///< Get current variant
                                        /**< Get the currently selected variant index. The returned
                                             number directly indexes the list of sub parsers.
                                             \returns Index of currently selected variant. Integer
                                                 in the range from 0 to (number-of-sub-parsers - 1)
                                          */
        
        template <unsigned N>
        typename boost::mpl::at< parsers, boost::mpl::int_<N> >::type get() const;
                                        ///< Access sub-parser
                                        /**< This call will return the sub-parser at index \a
                                             N. This call will fail, if the currently active
                                             variant() is not \a N.
                                             \pre variant() == \a N
                                             \returns sub-parser at index \a N */

        template <unsigned N>
        void init();                    ///< Re-initialize field
                                        /**< This will reinitialize the field to the variant
                                             sub-parser at index \a N changing the currently
                                             selected variant.
                                             \post variant() == \a N */
    };

    /** \brief Define VariantParser field

        This macro is a special helper to define a senf::DirectVariantParser type field. This is a
        variant field which chooses the sub-type by directly taking the value of some other field.

        \code
        struct SomeParser : public PacketParserBase
        {
        #   include SENF_PARSER()
        
            SENF_PARSER_PRIVATE_FIELD( type, senf::UInt8Parser );
            SENF_PARSER_PRIVATE_VARIANT( content, type,
                                            (senf::VoidPacketParser)
                                            (senf::UInt8Parser)
                                            (senf::UInt16Parser)
                                            (senf::UInt24Parser)
                                            (senf::UInt32Parser) );

            senf::UInt8Parser  uint8()  const { return content().get<1>(); }
            senf::UInt16Parser uint16() const { return content().get<2>(); }
            senf::UInt24Parser uint24() const { return content().get<3>(); }
            senf::UInt32Parser uint32() const { return content().get<4>(); }

            void disable()    const { content().init<0>(); }
            void set_uint8()  const { content().init<1>(); }
            void set_uint16() const { content().init<2>(); }
            void set_uint24)  const { content().init<3>(); }
            void set_uint23() const { content().init<4>(); }

            SENF_PARSER_FINALIZE(SomeParser);
        };
        \endcode

        The variant \c content chooses one of the sub parsers depending on the \c type field. If \c
        type is 0, senf::VoidPacketParser is selected, if it is 1, senf::UInt8Parser and so on. 

        It is customary, to hide the variant parser (by defining it private) and provide more
        conveniently named accessors.

        Further additional tags are supported which modify the way, the \a chooser field is
        interpreted:

        <table class="senf fixedcolumn">
        <tr><td>\c transform(\a transform, \a size)</td><td>The \a transform is applied to the \a
        chooser value, the value is not used directly</td>
        </table>

        The optional \a transform is a class with the following layout

        \code
        struct MyTransform
        {
            typedef ... value_type;
            static value_type get(other_type v);
            static other_type set(value_type v);
        };
        \endcode \c other_type is the \a chooser ::\c value_type where as the \c value_type typedef
        is the arbitrary return type of the transform.

        The tags are applied to the \a chooser parameter:
        \code
        SENF_PARSER_VARIANT ( content, transform(MyTransform, type_),
                                            (senf::VoidPacketParser)
                                            (senf::UInt8Parser)
                                            (senf::UInt16Parser)
                                            (senf::UInt24Parser)
                                            (senf::UInt32Parser) );
        \endcode
        
        \param[in] name name of the field
        \param[in] chooser name of the field choosing the variant to use
        \param[in] types a Boost.Preprocessor style sequence of sub-parser types

        \see 
            senf::VariantParser \n 
            \ref SENF_PARSER_PRIVATE_VARIANT()
        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_VARIANT(name, chooser, types) \
        SENF_PARSER_VARIANT_I(public, name, chooser, types)

    /** \brief Define private VariantParser field
        
        \see \ref SENF_PARSER_VARIANT()
        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_PRIVATE_VARIANT(name, chooser, types) \
        SENF_PARSER_VARIANT_I(private, name, chooser, types)
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_VariantParser_i_)
#define HH_VariantParser_i_
//#include "VariantParser.cci"
#include "VariantParser.ct"
#include "VariantParser.cti"
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
