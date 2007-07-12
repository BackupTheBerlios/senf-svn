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
    \brief PacketData public header */

#ifndef HH_PacketData_
#define HH_PacketData_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include "PacketTypes.hh"

//#include "PacketData.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief

        PacketData only exists to separate out the container interface from PacketInterpreter.
      */
    class PacketData
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::detail::packet::smart_pointer<PacketData>::ptr_t ptr;
        
        typedef senf::detail::packet::iterator iterator;
        typedef senf::detail::packet::const_iterator const_iterator;
        typedef senf::detail::packet::size_type size_type;
        typedef senf::detail::packet::difference_type difference_type;
        typedef senf::detail::packet::byte byte;
        typedef byte value_type;
        typedef byte & reference;
        typedef byte const & const_reference;
        typedef byte * pointer;
        typedef byte const * const_pointer;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no public constructors
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////
    
        ///\name Sequence interface to raw data
        ///@{

        iterator begin() const;
        iterator end() const;
        size_type size() const;
        bool empty() const;
        byte operator[](size_type n) const;
        byte & operator[](size_type n);

        // Modifying the raw packet data

        // IMPORTANT NOTE: It is not possible to insert data AFTER an empty packet
        // since for an empty packet begin() == end(). However, I hope this problem is
        // only academic since what should an empty packet be good for ?
        void insert(iterator pos, byte v);
        void insert(iterator pos, size_type n, byte v);
        template <class InputIterator>
        void insert(iterator pos, InputIterator f, InputIterator l,
                    typename boost::disable_if< boost::is_convertible<InputIterator,size_type> >::type * = 0);

        void erase(iterator pos);
        void erase(iterator first, iterator last);
        void clear();
        
        void resize(size_type n, byte v=0);

        ///@}

        bool valid();

    protected:
        PacketData(size_type b, size_type e);

        detail::PacketImpl * impl_;

        detail::PacketImpl & impl() const;

    private:
        size_type begin_;
        size_type end_;

        friend class detail::PacketImpl;
    };

    struct TruncatedPacketException : public std::exception
    { virtual char const * what() const throw() { return "truncated packet"; } };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "PacketData.cci"
//#include "PacketData.ct"
#include "PacketData.cti"
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
