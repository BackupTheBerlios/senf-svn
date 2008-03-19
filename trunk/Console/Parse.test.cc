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
    \brief Parse.test unit tests */

//#include "Parse.test.hh"
//#include "Parse.test.ih"

// Custom includes
#include <sstream>
#include "Parse.hh"
#include "Parse.ih"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace 
{
    struct TestParseDispatcher 
    {
        TestParseDispatcher(std::ostream & os) : os_ (os) {}

        std::ostream & os_;

        void beginCommand(std::string const & command) 
            { os_ << "beginCommand( " << command << " )\n"; }
        void endCommand() 
            { os_ << "endCommand()\n"; }
        
        void pushArgument(std::string const & argument)
            { os_ << "pushArgument( " << argument << " )\n"; }
        void openGroup()
            { os_ << "openGroup()\n"; }
        void closeGroup()
            { os_ << "closeGroup()\n"; }
        void pushPunctuation(std::string const & token)
            { os_ << "pushPunctuation( " << token << " )\n"; }
        void pushWord(std::string const & token)
            { os_ << "pushWord( " << token << " )\n"; }
    };
}

BOOST_AUTO_UNIT_TEST(commandParser)
{
    senf::console::detail::CommandGrammar<TestParseDispatcher>::Context context;
    std::stringstream ss;
    TestParseDispatcher dispatcher (ss);
    senf::console::detail::CommandGrammar<TestParseDispatcher> grammar (dispatcher, context);
    senf::console::detail::SkipGrammar skipGrammar;

    char text[] = 
        "# Comment\n"
        "doo / bii / doo arg/../path"
        "                flab::blub"
        "                123.434>a"
        "                (a,b,c (huhu))"
        "                \"foo\\\"bar\" #\n"
        "                x\"01 02 # Inner comment\n"
        "                   0304\"";

    BOOST_CHECK( boost::spirit::parse( 
                     text, 
                     grammar, 
                     skipGrammar ) . full );
    BOOST_CHECK_EQUAL( ss.str(), 
                       "beginCommand( doo/bii/doo )\n"
                       "pushArgument( arg/../path )\n"
                       "pushArgument( flab::blub )\n"
                       "pushArgument( 123.434>a )\n"
                       "openGroup()\n"
                       "pushWord( a )\n"
                       "pushPunctuation( , )\n"
                       "pushWord( b )\n"
                       "pushPunctuation( , )\n"
                       "pushWord( c )\n"
                       "pushPunctuation( ( )\n"
                       "pushWord( huhu )\n"
                       "pushPunctuation( ) )\n"
                       "closeGroup()\n"
                       "pushArgument( foo\"bar )\n"
                       "pushArgument( \x01\x02\x03\x04 )\n"
                       "endCommand()\n" );
}

BOOST_AUTO_UNIT_TEST(singleCommandParser)
{
    senf::console::SingleCommandParser parser;

    char const text[] = 
        "# Comment\n"
        "doo / bii / doo arg/../path"
        "                flab::blub"
        "                123.434>a"
        "                (a,b,c (huhu))"
        "                \"foo\\\"bar\" #\n"
        "                x\"01 02 # Inner comment\n"
        "                   0304\"";

    senf::console::ParseCommandInfo info;
    BOOST_CHECK( parser.parseCommand(text, info) );

    BOOST_CHECK_EQUAL( info.commandPath(), "doo/bii/doo" );
    BOOST_REQUIRE_EQUAL( info.arguments(), 6u );
    BOOST_REQUIRE_EQUAL( info.tokens(), 13u );

    char const * tokens[] = { "arg/../path", 
                              "flab::blub", 
                              "123.434>a", 
                              "a", ",", "b", ",", "c", "(", "huhu", ")",
                              "foo\"bar",
                              "\x01\x02\x03\x04" };

    BOOST_REQUIRE_EQUAL( info.begin_arguments()[0].size(), 1u );
    BOOST_CHECK_EQUAL( info.begin_arguments()[0].begin()->value(), tokens[0] );

    BOOST_REQUIRE_EQUAL( info.begin_arguments()[1].size(), 1u );
    BOOST_CHECK_EQUAL( info.begin_arguments()[1].begin()->value(), tokens[1] );

    BOOST_REQUIRE_EQUAL( info.begin_arguments()[2].size(), 1u );
    BOOST_CHECK_EQUAL( info.begin_arguments()[2].begin()->value(), tokens[2] );

    BOOST_REQUIRE_EQUAL( info.begin_arguments()[3].size(), 8u );
    for (unsigned i (0); i<8; ++i)
        BOOST_CHECK_EQUAL( info.begin_arguments()[3].begin()[i].value(), tokens[3+i] );

    BOOST_REQUIRE_EQUAL( info.begin_arguments()[4].size(), 1u );
    BOOST_CHECK_EQUAL( info.begin_arguments()[4].begin()->value(), tokens[11] );

    BOOST_REQUIRE_EQUAL( info.begin_arguments()[5].size(), 1u );
    BOOST_CHECK_EQUAL( info.begin_arguments()[5].begin()->value(), tokens[12] );
}

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
