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
    \brief ParseList public header */

#ifndef HH_ParseList_
#define HH_ParseList_ 1

// Custom includes
#include <boost/utility.hpp>
#include "PacketParser.hh"

//#include "ParseList.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    namespace detail { template <class ElementParser, class IteratorPolicy> 
                       class Parse_List_Iterator; }

    template <class ListPolicy>
    class Parse_List_Container;

    /** \brief Arbitrary sequential element collection

        A List is a sequential collection of elements. The element type is given as an arbitrary
        parser. The list is more flexible than a vector: It is not limited to fixed-size elements
        and it might not have direct access to the size of the collection. 

        The cost is however, that a List is only a model of an STL forward sequence. The parser
        provides a reduced interface to this sequence, the container wrapper provides the complete
        interface.

        Pare_List makes use of a policy template argument, \a ListPolicy, to customize the way the
        list is laid out. This policy is given quite some freedom in the list
        implementation. It is however important, that list elements <em>always follow each other
        without padding</em> (if padding is needed, it needs to be part of the element parser).

        \see ExampleListPolicy
        \ingroup parsecollection
      */
    template <class ListPolicy>
    class Parse_List 
        : public PacketParserBase,
          private ListPolicy
    {
    public:
        Parse_List(data_iterator i, state_type s);
        Parse_List(ListPolicy policy, data_iterator i, state_type s);
                                        ///< Additional policy specific constructor
                                        /**< This constructor may be used, if the policy needs
                                             additional parameters. */

        size_type bytes() const;
        void init() const;

        static const size_type init_bytes = ListPolicy::init_bytes;

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef typename ListPolicy::element_type value_type;
        typedef detail::Parse_List_Iterator< 
            value_type, typename ListPolicy::iterator_policy > iterator;
        typedef iterator const_iterator;
        typedef typename ListPolicy::container_type container;

        size_type size() const;
        bool empty() const;
        
        iterator begin() const;
        iterator end() const;

        value_type front() const;
        value_type back() const;
        
        template <class Value> void push_back        (Value value, size_type n=1) const;
                               void push_back_space  (size_type n=1) const;
        template <class Value> void push_front       (Value value, size_type n=1) const;
                               void push_front_space (size_type n=1) const;
                               void resize           (size_type n) const;
        template <class Value> void resize           (size_type n, Value value) const;

    private:
        template <class Policy> friend class Parse_List_Container;
    };

    /** \brief Example of a list policy. ONLY FOR EXPOSITION.
        
        This class shows the interface which must be implemented by a list policy. It is not a list
        policy only a declaration of the interface:
        \code
        struct ExampleListPolicy
        {
            // optional typedefs used to simplify all other declarations
            typedef PacketParserBase::data_iterator data_iterator;
            typedef PacketParserBase::state_type state_type;
            typedef PacketParserBase::size_type size_type;

            // mandatory typedefs in the parser and container policy
            typedef ElementParser element_type;
            typedef Parse_List< ExampleListPolicy > parser_type;
            typedef Parse_List_Container< ExampleListPolicy > container_type;

            // mandatory constant in parser and container policy
            static const size_type init_bytes = 0;

            // Members needed in the parser and the container policy
            size_type bytes  (data_iterator i, state_type s) const;
            size_type size   (data_iterator i, state_type s) const;
            void      init   (data_iterator i, state_type s) const;
        
            // Members needed only in the container policy
            void      erase  (data_iterator i, state_type s, iterator p) const;
            void      insert (data_iterator i, state_type s, iterator p) const;

            struct iterator_policy
            {
                iterator setBegin        (data_iterator i, state_type s);
                iterator setEnd          (data_iterator i, state_type s);
                void     setFromPosition (data_iterator i, state_type s, iterator p);
                iterator next            (data_iterator i, state_type s);
                iterator raw             (data_iterator i, state_type s) const;
            };
        };
        \endcode

        If necessary, you may use a different policy in the container_type. The ListPolicy must
        define the elements bytes(), size() and init(), the container policy needs all these and
        additionally needs erase() and insert(). The container policy will also need the
        element_type, parser_type and container_type typedefs.
        
        \see \ref Parse_List
     */
    struct ExampleListPolicy
    {
        typedef PacketParserBase::data_iterator iterator;
        typedef PacketParserBase::state_type state_type;
        typedef PacketParserBase::size_type size_type;

        typedef void element_type;      ///< Type of list elements
                                        /**< This is the parser used to parse the list elements. */
        typedef void parser_type;       ///< List parser type
                                        /**< parser_type is the list parser used to parse a list of
                                             this type,
                                             e.g. <tt>senf::Parse_List<ExampleListPolicy></tt>. */
        typedef void container_type;    ///< Type of container wrapper
                                        /**< This is the container wrapper of the list, e.g.
                                             <tt>Parse_List_Container<ExampleListPolicy></tt>. The
                                             container may however use a \e different policy, as
                                             long as that policy is constructible from the parser
                                             policy. */

        static const size_type init_bytes = 0; ///< Size of a new list of this type
                                        /**< Initial size which needs to be allocated to this type
                                             of list */

        size_type bytes(iterator i, state_type s) const; ///< Size of list in bytes
                                        /**< Return the complete size of the list in
                                             bytes. Depending on the type of list, this call may
                                             need to completely traverse the list ... */

        size_type size(iterator i, state_type s) const; ///< Number of elements in list
                                        /**< Return the number of elements in the list. This
                                             operation may be quite inefficient for some lists (the
                                             list must be traversed to find that number. */

        void init(iterator i, state_type s) const; ///< Initialize new list
                                        /**< Called after init_size bytes have been allocated to
                                             initialize the list. After init() is called, the list
                                             is traversed to initialize any members (probably
                                             none) */

        void erase(iterator i, state_type s, iterator p) const; ///< Erase element from list
                                        /**< Delete the list element at p from the List (i,s). When
                                             this operation is called, the element is still part of
                                             the list. This call must update the meta-data as
                                             needed. The data will be removed after this call
                                             returns. */

        void insert(iterator i, state_type s, iterator p) const; ///< Insert element into list
                                        /**< This is called after an element has been inserted at p
                                             into the List (i,s) to update the meta-data. */

        /** \brief Example of a list iterator policy. ONLY FOR EXPOSITION.

            \see \ref ExampleListPolicy \n
                \ref Parse_List
         */
        struct iterator_policy 
        {
            iterator setBegin(iterator i, state_type s); ///< Initialize iterator to begin()
                                        /**< Initialize the policy from the given List (i,s). Set
                                             the iterator to the beginning iterator. Return
                                             data_iterator to the first element.

                                             \warning if the list is empty, the returned iterator
                                             \e must be the same as the one returned by setEnd. */

            iterator setEnd(iterator i, state_type s); ///< Initialize iterator to end()
                                        /**< Initialize the policy from the given List (i,s). Set
                                             the iterator to the end iterator. Return data_iterator
                                             used to mark the end of the range. This may be a
                                             special sentinel value (e.g. data().end()) if
                                             needed. */

            void setFromPosition(iterator i, state_type s, iterator p); 
                                        ///< Initialize iterator from the given raw position
                                        /**< Set the iterator to the Element at raw position p. This
                                             operation can potentially be very inefficient if the
                                             list needs to be traversed from the beginning until the
                                             iterator is found. */
            
            iterator next(iterator i, state_type s); ///< Advance to next element
                                        /**< given an iterator to an element, go to the next
                                             element. */

            iterator raw(iterator i, state_type s); ///< Return raw position of element
                                        /**< Given the iterator state (i,s), return the raw iterator
                                             to the datum. This will be i in almost all cases EXCEPT
                                             if a special sentinel value is used as end() value. In
                                             this case, this member must return the real position
                                             after the last element. */
        };

        /** \brief Example of a list container policy. ONLY FOR EXPOSITION
            
            \see \ref ExampleListPolicy \n
                \ref Parse_List
         */
        struct container_policy
        {
            void init(iterator i, state_type s); ///< Initialize new container
            void update(iterator i, state_type s); ///< Called before every container access
        };
    };

    /** \brief Parse_List container wrapper

        This is the container wrapper used for list parsers. The container wrapper will stay valid
        after changing the collection. However the container still depends on the packet and will be
        invalidated if the Packet is deallocated or if the packet size is changed from without the
        container wrapper (more precisely, it is invalided if the insertion/deletion happens before
        the vector in the packet data).

        The vector container wrapper provides a complete STL random-access sequence interface.
        
        \code
        SomePacket p (...);
        SomePacket::aListCollection_t::container c (p->aListCollection());
        c.insert(c.begin(), ... );
        \endcode

        \see Parse_List
      */
    template <class ListPolicy>
    class Parse_List_Container
        : private ListPolicy
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef typename ListPolicy::parser_type parser_type;
        typedef PacketParserBase::data_iterator data_iterator;
        typedef PacketParserBase::size_type size_type;
        typedef PacketParserBase::difference_type difference_type;
        typedef typename ListPolicy::element_type value_type;
        typedef detail::Parse_List_Iterator<
            value_type, typename ListPolicy::iterator_policy> iterator;
        typedef iterator const_iterator;
        typedef PacketParserBase::state_type state_type;
        
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy
        // default destructor
        // conversion constructors

        Parse_List_Container(parser_type const & list);
        ~Parse_List_Container();
        
        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Accessors
        ///@{

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;

        value_type front() const;
        value_type back() const;

        ///@}
        ///\name Mutators
        ///@{

        // All these operations can be quite inefficient depending on the list type
        void shift(iterator pos, size_type n=1);
        template <class Value>
        void insert(iterator pos, Value const & t);
        template <class Value>
        void insert(iterator pos, size_type n, Value const & t);
        template <class ForwardIterator>
        void insert(iterator pos, ForwardIterator f, ForwardIterator l,
                    typename boost::disable_if< boost::is_convertible<ForwardIterator,size_type> >::type * = 0);

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

    private:
        state_type state_;
        size_type i_;
    };

        
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(SENF_PACKETS_DECL_ONLY) && !defined(HH_ParseList_i_)
#define HH_ParseList_i_
//#include "ParseList.cci"
#include "ParseList.ct"
#include "ParseList.cti"
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