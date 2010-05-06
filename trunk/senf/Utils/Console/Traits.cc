// $Id$
//
// Copyright (C) 2008
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
    \brief Traits non-inline non-template implementation */

#include "Traits.hh"
#include "Traits.ih"

// Custom includes
#include <senf/Utils/senfassert.hh>

//#include "Traits.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ long senf::console::detail::parseEnum(EnumTable const & table,
                                              ParseCommandInfo::TokensRange const & tokens)
{
    if (tokens.size() != 1)
        throw SyntaxErrorException("parameter syntax error");

    std::string sym (tokens.begin()[0].value());
    boost::algorithm::to_lower(sym);
    EnumTable::left_map::const_iterator i1 (table.left.lower_bound(sym));
    EnumTable::left_map::const_iterator i2 (table.left.lower_bound(sym+"\xff"));
    if (i1 == i2)
        throw SyntaxErrorException("parameter syntax error: invalid enum value: ")
            << tokens.begin()[0].value();
    long v (i1->second);
    if (boost::algorithm::to_lower_copy(i1->first) == sym)
        return v;
    ++i1;
    if (i1 != i2)
        throw SyntaxErrorException("parameter syntax error: ambiguous enum value: ")
            << tokens.begin()[0].value();
    return v;
}

prefix_ std::string senf::console::detail::formatEnum(EnumTable const & table, long value)
{
    EnumTable::right_map::const_iterator i (table.right.find(value));
    SENF_ASSERT( i != table.right.end(), "Invalid enum value" );
    return i->second;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Traits.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
