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
    \brief AreaRegistry unit tests */

//#include "AreaRegistry.test.hh"
//#include "AreaRegistry.test.ih"

// Custom includes
#include "main.test.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(areaRegistry)
{
    BOOST_CHECK( true );
#if 0
    // Need to rewrite this test ...
    char const * areas[] = { "GlobalTestArea",
                             "senf::log::DefaultArea",
                             "senf::log::test::Foo",
                             "senf::log::test::myArea" };

    BOOST_CHECK_EQUAL_COLLECTIONS( senf::log::AreaRegistry::instance().begin(),
                                   senf::log::AreaRegistry::instance().end(),
                                   areas, areas+sizeof(areas)/sizeof(areas[0]) );
#endif
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
