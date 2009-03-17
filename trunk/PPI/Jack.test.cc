// $Id$
//
// Copyright (C) 2009 
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
    \brief Jack unit tests */

//#include "Jack.test.hh"
//#include "Jack.test.ih"

// Custom includes
#include "PPI.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    class ActiveDummyForward 
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE(ActiveDummyForward);
    public:
        senf::ppi::connector::ActiveInput<> input;
        senf::ppi::connector::PassiveOutput<> output;

        ActiveDummyForward()
            { route(input, output); output.onRequest(&ActiveDummyForward::request); }

    private:
        void request()
            { output(input()); }
    };

    class PassiveDummyForward 
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE(PassiveDummyForward);
    public:
        senf::ppi::connector::PassiveInput<> input;
        senf::ppi::connector::ActiveOutput<> output;

        PassiveDummyForward()
            { route(input, output); input.onRequest(&PassiveDummyForward::request); }

    private:
        void request()
            { output(input()); }
    };

    class ActiveGroup
    {
    public:
        senf::ppi::connector::ActiveInputJack<> input;
        senf::ppi::connector::PassiveOutputJack<> output;

        ActiveGroup()
            : input (forward.input), output (forward.output) {}

    private:
        ActiveDummyForward forward;
    };

    class PassiveGroup
    {
    public:
        senf::ppi::connector::PassiveInputJack<> input;
        senf::ppi::connector::ActiveOutputJack<> output;

        PassiveGroup()
            : input (forward.input), output (forward.output) {}
        
    private:
        PassiveDummyForward forward;
    };

}

BOOST_AUTO_UNIT_TEST(jacks)
{
    {
        ActiveGroup group;
        senf::ppi::module::debug::PassiveSource source;
        senf::ppi::module::debug::ActiveSink sink;

        senf::ppi::connect(source, group);
        senf::ppi::connect(group, sink);
        
        senf::ppi::init();
        
        senf::Packet p (senf::DataPacket::create());
        source.submit(p);
        
        BOOST_CHECK(p == sink.request());
    }
     
    {
        PassiveGroup group;
        senf::ppi::module::debug::ActiveSource source;
        senf::ppi::module::debug::PassiveSink sink;

        senf::ppi::connect(source, group);
        senf::ppi::connect(group, sink);

        senf::ppi::init();

        senf::Packet p (senf::DataPacket::create());
        source.submit(p);

        BOOST_CHECK(p == sink.front());
    }
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
