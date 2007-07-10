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

#ifndef HH_ParseInt_
#define HH_ParseInt_ 1

// Custom includes
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>
#include <boost/integer/integer_mask.hpp>
#include "PacketParser.hh"

//#include "ParseInt.mpp"
///////////////////////////////hh.p////////////////////////////////////////
#include "ParseInt.ih"

namespace senf {

    struct Parse_Int8
        : public detail::packet::ParseIntOps<Parse_Int8,boost::int8_t>,
          public PacketParserBase
    {
        Parse_Int8(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int8_t value_type;
        static size_type const fixed_bytes = 1;

        value_type value() const { return i()[0]; }
        void value(value_type v) { i()[0] = v; }
        Parse_Int8 const & operator= (value_type other) { value(other); return *this; }
    };
    inline std::ostream & operator<<(std::ostream & os, Parse_Int8 const & i)
    { os << i.value(); return os; }

    struct Parse_UInt8
        : public detail::packet::ParseIntOps<Parse_UInt8,boost::uint8_t>,
          public PacketParserBase
    {
        Parse_UInt8(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint8_t value_type;
        static size_type const fixed_bytes = 1;

        value_type value() const { return i()[0]; }
        void value(value_type v) { i()[0] = v; }
        Parse_UInt8 const & operator= (value_type other) { value(other); return *this; }
    };
    inline std::ostream & operator<<(std::ostream & os, Parse_UInt8 const & i)
    { os << i.value(); return os; }

    struct Parse_Int16
        : public detail::packet::ParseIntOps<Parse_Int16,boost::int16_t>,
          public PacketParserBase
    {
        Parse_Int16(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int16_t value_type;
        static size_type const fixed_bytes = 2;

        value_type value() const { return detail::packet::parse_uint16(i()); }
        void value(value_type v) { detail::packet::write_uint16(i(),v); }
        Parse_Int16 const & operator= (value_type other) { value(other); return *this; }
    };
    inline std::ostream & operator<<(std::ostream & os, Parse_Int16 const & i)
    { os << i.value(); return os; }

    struct Parse_UInt16
        : public detail::packet::ParseIntOps<Parse_UInt16,boost::uint16_t>,
          public PacketParserBase
    {
        Parse_UInt16(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint16_t value_type;
        static size_type const fixed_bytes = 2;

        value_type value() const { return detail::packet::parse_uint16(i()); }
        void value(value_type v) { detail::packet::write_uint16(i(),v); }
        Parse_UInt16 const & operator= (value_type other) { value(other); return *this; }
    };
    inline std::ostream & operator<<(std::ostream & os, Parse_UInt16 const & i)
    { os << i.value(); return os; }

    struct Parse_Int24
        : public detail::packet::ParseIntOps<Parse_Int24,boost::int32_t>,
          public PacketParserBase
    {
        Parse_Int24(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int32_t value_type;
        static size_type const fixed_bytes = 3;

        value_type value() const {
            value_type v (detail::packet::parse_uint24(i())); return v&0x800000 ? v|0xff000000 : v; }
        void value(value_type v) { detail::packet::write_uint24(i(),v); }
        Parse_Int24 const & operator= (value_type other) { value(other); return *this; }
    };
    inline std::ostream & operator<<(std::ostream & os, Parse_Int24 const & i)
    { os << i.value(); return os; }

    struct Parse_UInt24
        : public detail::packet::ParseIntOps<Parse_UInt24,boost::uint32_t>,
          public PacketParserBase
    {
        Parse_UInt24(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint32_t value_type;
        static size_type const fixed_bytes = 3;

        value_type value() const { return detail::packet::parse_uint24(i()); }
        void value(value_type v) { detail::packet::write_uint24(i(),v); }
        Parse_UInt24 const & operator= (value_type other) { value(other); return *this; }
    };
    inline std::ostream & operator<<(std::ostream & os, Parse_UInt24 const & i)
    { os << i.value(); return os; }

    struct Parse_Int32
        : public detail::packet::ParseIntOps<Parse_Int32,boost::int32_t>,
          public PacketParserBase
    {
        Parse_Int32(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int32_t value_type;
        static size_type const fixed_bytes = 4;

        value_type value() const { return detail::packet::parse_uint32(i()); }
        void value(value_type v) { detail::packet::write_uint32(i(),v); }
        Parse_Int32 const & operator= (value_type other) { value(other); return *this; }
    };
    inline std::ostream & operator<<(std::ostream & os, Parse_Int32 const & i)
    { os << i.value(); return os; }

    struct Parse_UInt32
        : public detail::packet::ParseIntOps<Parse_UInt32,boost::uint32_t>,
          public PacketParserBase
    {
        Parse_UInt32(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint32_t value_type;
        static size_type const fixed_bytes = 4;

        value_type value() const { return detail::packet::parse_uint32(i()); }
        void value(value_type v) { detail::packet::write_uint32(i(),v); }
        Parse_UInt32 const & operator= (value_type other) { value(other); return *this; }
    };
    inline std::ostream & operator<<(std::ostream & os, Parse_UInt32 const & i)
    { os << i.value(); return os; }

    template <unsigned Start, unsigned End>
    struct Parse_IntField
        : public detail::packet::ParseIntOps<Parse_IntField<Start,End>,boost::int32_t>,
          public PacketParserBase
    {
        Parse_IntField(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int32_t value_type;
        static size_type const fixed_bytes = (End-1)/8+1;

        value_type value() const {
            value_type v (detail::packet::parse_bitfield<Start,End>::parse(i()));
            return v&boost::high_bit_mask_t<End-Start-1>::high_bit ?
                v | ~boost::low_bits_mask_t<End-Start>::sig_bits : v;
        }
        void value(value_type v) { detail::packet::parse_bitfield<Start,End>::write(i(),v); }
        Parse_IntField const & operator= (value_type other) { value(other); return *this; }

    private:
        BOOST_STATIC_ASSERT( Start<End );
        BOOST_STATIC_ASSERT( End-Start<=32 );
    };
    template <unsigned Start, unsigned End>
    inline std::ostream & operator<<(std::ostream & os, Parse_IntField<Start,End> const & i)
    { os << i.value(); return os; }

    template <unsigned Start, unsigned End>
    struct Parse_UIntField
        : public detail::packet::ParseIntOps<Parse_UIntField<Start,End>,boost::uint32_t>,
          public PacketParserBase
    {
        Parse_UIntField(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint32_t value_type;
        static size_type const fixed_bytes = (End-1)/8+1;

        value_type value() const { return detail::packet::parse_bitfield<Start,End>::parse(i()); }
        void value(value_type v) { detail::packet::parse_bitfield<Start,End>::write(i(),v); }
        Parse_UIntField const & operator= (value_type other) { value(other); return *this; }

    private:
        BOOST_STATIC_ASSERT( Start<End );
        BOOST_STATIC_ASSERT( End-Start<=32 );
    };
    template <unsigned Start, unsigned End>
    inline std::ostream & operator<<(std::ostream & os, Parse_UIntField<Start,End> const & i)
    { os << i.value(); return os; }

    template <unsigned bit>
    struct Parse_Flag
        : public detail::packet::ParseIntOps<Parse_Flag<bit>,bool>,
          public PacketParserBase
    {
        Parse_Flag(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef bool value_type;
        static size_type const fixed_bytes = 1;

        value_type value() const { return i()[bit/8] & (1<<(7-(bit%8))); }
        void value(value_type v) {
            if (v) i()[0] |= 1<<(7-(bit%8));
            else   i()[0] &= ~(1<<(7-(bit%8)));
        }
        Parse_Flag const & operator= (value_type other) { value(other); return *this; }
    };
    template <unsigned bit>
    inline std::ostream & operator<<(std::ostream & os, Parse_Flag<bit> const & i)
    { os << i.value(); return os; }

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ParseInt.cci"
//#include "ParseInt.ct"
//#include "ParseInt.cti"
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
