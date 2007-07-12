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
    \brief safe_data_iterator public header */

#ifndef HH_safe_data_iterator_
#define HH_safe_data_iterator_ 1

// Custom includes
#include "Utils/SafeBool.hh"
#include "PacketData.hh"
#include "PacketParser.hh"

//#include "safe_data_iterator.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class safe_data_iterator
        : public boost::iterator_facade< safe_data_iterator,
                                         PacketData::value_type,
                                         boost::random_access_traversal_tag >,
          public ComparableSafeBool<safe_data_iterator>
    {
    public:
        typedef PacketData::size_type size_type;

        safe_data_iterator();
        explicit safe_data_iterator(PacketData & data);
        safe_data_iterator(PacketData & data, PacketData::iterator i);
        explicit safe_data_iterator(PacketParserBase const & parser);

        safe_data_iterator & operator=(PacketData::iterator i);
        safe_data_iterator & operator=(PacketParserBase const & parser);
        operator PacketData::iterator() const;

        bool boolean_test() const;

        PacketData & data() const;

    private:
        friend class boost::iterator_core_access;

        // iterator_facade interface

        value_type & dereference() const;
        bool equal(safe_data_iterator const & other) const;
        difference_type distance_to(safe_data_iterator const & other) const;
        void increment();
        void decrement();
        void advance(difference_type n);

        PacketData::iterator i() const;

        PacketData * data_;
        size_type i_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "safe_data_iterator.cci"
//#include "safe_data_iterator.ct"
//#include "safe_data_iterator.cti"
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
