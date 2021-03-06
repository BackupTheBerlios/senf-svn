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
    \brief Config unit tests */

//#include "Config.test.hh"
//#include "Config.test.ih"

// Custom includes
#include "Console.hh"
#include <boost/filesystem/operations.hpp>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    std::string val1;
    bool val2 (false);

    void fun1(std::string p)
    { val1 = p; }

    void fun2()
    { val2 = true; }

    class TempFile
    {
    public:
        TempFile(std::string const & name) : name_ (name), file_ (name_.c_str()) {}
        ~TempFile() { file_.close(); boost::filesystem::remove(name_); }

        template <class T> TempFile & operator<<(T const & v) { file_ << v; return *this; }
        enum Closer { close }; void operator<<(Closer) { file_.close(); }
        std::string const & name() { return name_; }

    private:
        std::string name_;
        std::ofstream file_;
    };

}

SENF_AUTO_UNIT_TEST(configBundle_empty)
{
    TempFile cfg ("test.cfg");
    cfg << TempFile::close;

    senf::console::ScopedDirectory<> root;
    root.add("fun2", senf::console::factory::Command(&fun2));

    senf::console::ConfigBundle bundle(root);
    bundle.add( senf::console::FileConfig(cfg.name()));

    SENF_CHECK_NO_THROW( bundle.parse() );
}

SENF_AUTO_UNIT_TEST(configBundle)
{
    namespace fty = senf::console::factory;

    senf::console::ScopedDirectory<> root;
    senf::console::root().add("root", root);

    senf::console::ScopedDirectory<> chroot;
    senf::console::root().add("chroot", chroot);

    root.add("dir1", fty::Directory()).add("fun1", fty::Command(&fun1));
    root.add("fun2", fty::Command(&fun2));
    chroot.add("dir1", fty::Directory()).add("fun1", fty::Command(&fun1));
    chroot.add("fun2", fty::Command(&fun2));

    TempFile cfg ("test.cfg");
    cfg << "dir1/fun1 foo; fun2;" << TempFile::close;

    char const * argv[] = { "", "--dir1-fun1=bar" };

    senf::console::ConfigBundle bundle(root);
    bundle.add( senf::console::FileConfig(cfg.name()) );
    bundle.add( senf::console::OptionsConfig(sizeof(argv)/sizeof(argv[0]), argv) );

    SENF_CHECK_NO_THROW( bundle.parse() );
    BOOST_CHECK_EQUAL( val1, "bar" );
    BOOST_CHECK_EQUAL( val2, true );

    bundle.chroot( chroot);
    SENF_CHECK_NO_THROW( bundle.parse() );
    BOOST_CHECK_EQUAL( val1, "bar" );
    BOOST_CHECK_EQUAL( val2, true );
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
