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
    \brief PacketInterpreter public header */

#ifndef HH_PacketInterpreter_
#define HH_PacketInterpreter_ 1

// Custom includes
#include <boost/intrusive/ilist.hpp>
#include <boost/optional.hpp>
#include <boost/range.hpp>
#include "Utils/intrusive_refcount.hh"
#include "Utils/pool_alloc_mixin.hh"
#include "PacketData.hh"

//#include "PacketInterpreter.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief
      */
    class PacketInterpreterBase
        : protected PacketData, 
          public detail::packet::interpreter_list_base,
          public intrusive_refcount_t<PacketInterpreterBase>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::detail::packet::smart_pointer<
            PacketInterpreterBase>::ptr_t ptr;

        typedef senf::detail::packet::iterator iterator;
        typedef senf::detail::packet::const_iterator const_iterator;
        typedef senf::detail::packet::size_type size_type;
        typedef senf::detail::packet::difference_type difference_type;
        typedef senf::detail::packet::byte byte;

        typedef boost::iterator_range<iterator> range;
        typedef boost::optional< boost::iterator_range<iterator> > optional_range;
        typedef optional_range no_range;

        enum Append_t { Append };
        enum Prepend_t { Prepend };

        typedef ptr (*factory_t)(detail::PacketImpl *, iterator, iterator, Append_t);

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // protected constructors
        // no copy
        // no conversion constructors

        virtual ~PacketInterpreterBase();

        static                             factory_t no_factory();
        template <class PacketType> static factory_t factory();
        
        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Interpreter chain access
        ///@{

        ptr next();
        ptr prev();
        ptr first();
        ptr last();

        ///@}

        ///\name Data access
        ///@{

        PacketData & data();
        
        ///@}

        ///\name Interpreter chain information
        ///@{

        optional_range nextPacketRange();

        ///@}

    protected:
        PacketInterpreterBase(detail::PacketImpl * impl, iterator b, iterator e, Append_t);
        PacketInterpreterBase(detail::PacketImpl * impl, iterator b, iterator e, Prepend_t);

    private:
        // abstract packet type interface
        virtual optional_range v_nextPacketRange() = 0;

        // reference/memory management. Only to be called by intrusive_refcount_t.
        void add_ref();
        bool release();

        // containment management. Only to be called by PacketImpl.
        void assignImpl(detail::PacketImpl *);
        void releaseImpl();

        friend class detail::PacketImpl;
        friend class intrusive_refcount_t<PacketInterpreterBase>;
        template <class PacketType> friend class PacketInterpreter;
        friend class detail::packet::test::TestDriver;
    };

    /** \brief Concrete packet interpreter

        \see PacketTypeBase for the \a PacketType interface
      */
    template <class PacketType>
    class PacketInterpreter
        : public PacketInterpreterBase,
          public pool_alloc_mixin< PacketInterpreter<PacketType> >
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef typename senf::detail::packet::smart_pointer<
            PacketInterpreter>::ptr_t ptr;
        typedef PacketType type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // private constructors
        // no copy
        // no conversion constructors

        // Create completely new packet
        static ptr create();

        // Create packet from given data
        template <class ForwardReadableRange>
        static ptr create(ForwardReadableRange const & range);

        // Create packet as new (empty) packet after a given packet
        static ptr createAfter(PacketInterpreterBase::ptr packet);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

    protected:

    private:
        PacketInterpreter(detail::PacketImpl * impl, iterator b, iterator e, Append_t);
        PacketInterpreter(detail::PacketImpl * impl, iterator b, iterator e, Prepend_t);

        static ptr create(detail::PacketImpl * impl, iterator b, iterator e, Append_t);
        static ptr create(detail::PacketImpl * impl, iterator b, iterator e, Prepend_t);

        static size_type initSize();

        void init();

        virtual optional_range v_nextPacketRange();

        friend class detail::packet::test::TestDriver;
    };

    struct InvalidPacketChainException : public std::exception
    { virtual char const * what() const throw() { return "invalid packet chain"; } };
    
}

///////////////////////////////hh.e////////////////////////////////////////
#include "PacketInterpreter.cci"
#include "PacketInterpreter.ct"
#include "PacketInterpreter.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
