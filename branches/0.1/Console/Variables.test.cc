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
    \brief Variables.test unit tests */

//#include "Variables.test.hh"
//#include "Variables.test.ih"

// Custom includes
#include <iostream>
#include <sstream>
#include "Variables.hh"
#include "Executor.hh"
#include "Parse.hh"
#include "ScopedDirectory.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    void testParser(senf::console::ParseCommandInfo::TokensRange const &, int & value)
    { value = 0; }

    void testFormatter(int value, std::ostream & os)
    { os << '[' << value << ']'; }

    static bool changed_ (false);
    void testCallback(int oldValue)
    { changed_ = true; }
}

BOOST_AUTO_UNIT_TEST(variables)
{
    senf::console::Executor executor;
    senf::console::CommandParser parser;
    senf::console::ScopedDirectory<> dir;
    senf::console::root().add("test", dir);

    int var (5);

    std::stringstream ss;
    dir.add("var", var)
        .doc("Current blorg limit")
        .formatter(&testFormatter)
        .parser(&testParser)
        .typeName("number")
        .onChange(&testCallback);
    parser.parse("test/var; test/var 10; test/var",
                 boost::bind<void>( boost::ref(executor), boost::ref(ss), _1 ));
    BOOST_CHECK_EQUAL( ss.str(), "[5]\n[0]\n" );
    BOOST_CHECK( changed_ );

    ss.str("");
    dir("var").help(ss);
    BOOST_CHECK_EQUAL(ss.str(), 
                      "Usage:\n"
                      "    1- var new_value:number\n"
                      "    2- var\n"
                      "\n"
                      "Current blorg limit\n");

    dir.add("refvar", boost::ref(var))
        .doc("Current blorg limit")
        .formatter(&testFormatter)
        .parser(&testParser)
        .typeName("number");

    dir.add("crefvar", boost::cref(var))
        .doc("Current blorg limit")
        .formatter(&testFormatter);
}

#ifdef COMPILE_CHECK

COMPILE_FAIL(constVariable)
{
    senf::console::ScopedDirectory<> dir;
    int var;
    dir.add("var", boost::cref(var))
        .parser(&testParser);
}

#endif

///////////////////////////////cc.e////////////////////////////////////////
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
