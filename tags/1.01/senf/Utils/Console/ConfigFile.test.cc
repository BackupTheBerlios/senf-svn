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
    \brief ConfigFile unit tests */

//#include "ConfigFile.test.hh"
//#include "ConfigFile.test.ih"

// Custom includes
#include "Console.hh"
#include <fstream>
#include <boost/filesystem/operations.hpp>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    int var1 (0);
    bool var2 (false);

    void fun1(int v) { var1 = v; }
    void fun2() { var2 = true; }

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

#define SENF_CHECK_THROW_SYSTEMEXCEPTION( expr, errorNumber, msg)   \
    try {                                                           \
        BOOST_TEST_PASSPOINT();                                     \
        expr;                                                       \
        BOOST_ERROR( "senf::SystemException is expected");          \
    } catch( senf::SystemException const & ex ) {                   \
        BOOST_CHECK( ex.anyOf( errorNumber));                       \
        BOOST_CHECK( ex.message().find(msg) != std::string::npos);  \
    }                                                               \


SENF_AUTO_UNIT_TEST(configFile)
{
    namespace fty = senf::console::factory;

    TempFile cfgf ("test.cfg");
    cfgf << "dir1/fun1 10;\n"
         << TempFile::close;

    senf::console::ScopedDirectory<> dir1;
    senf::console::root().add("dir1", dir1);
    dir1.add("fun1",fty::Command(&fun1));

    {
        senf::console::ConfigFile cfg (cfgf.name());

        var1 = 0;
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 10 );

        var1 = 0;
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 0 );

        var1 = 0;
        cfg.reset();
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 10 );
    }

    {
        senf::console::ConfigFile cfg ("i don't exist");
        SENF_CHECK_THROW_SYSTEMEXCEPTION(cfg.parse(), ENOENT, "i don't exist");
        cfg.ignoreMissing();
        SENF_CHECK_NO_THROW( cfg.parse() );
    }
    {
        std::string etc_shaddow ("/etc/shadow");
        if (getuid() != 0 && boost::filesystem::exists(etc_shaddow)) {
            senf::console::ConfigFile cfg (etc_shaddow);
            SENF_CHECK_THROW_SYSTEMEXCEPTION( cfg.parse(), EACCES, etc_shaddow);
        }
    }
}

SENF_AUTO_UNIT_TEST(configFileRestrict)
{
    namespace fty = senf::console::factory;

    TempFile cfgf ("test.cfg");
    cfgf << "dir1/fun1 10;\n"
         << "dir2/fun2;\n"
         << TempFile::close;

    senf::console::ScopedDirectory<> dir1;
    senf::console::root().add("dir1", dir1);
    dir1.add("fun1",fty::Command(&fun1));

    {
        var1 = 0;
        var2 = false;
        senf::console::ConfigFile cfg (cfgf.name());
        SENF_CHECK_NO_THROW( cfg.parse(dir1) );
        BOOST_CHECK_EQUAL( var1, 10 );
        BOOST_CHECK_EQUAL( var2, false );
        BOOST_CHECK( cfg.parsed(dir1) );
        BOOST_CHECK( ! cfg.complete() );

        senf::console::ScopedDirectory<> dir2;
        senf::console::root().add("dir2", dir2);
        dir2.add("fun2",fty::Command(&fun2));

        var1 = 0;
        var2 = false;
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 0 );
        BOOST_CHECK_EQUAL( var2, true );
        BOOST_CHECK( cfg.complete() );
    }
}

SENF_AUTO_UNIT_TEST(configFileSkipGroup)
{
    namespace fty = senf::console::factory;

    TempFile cfgf ("test.cfg");
    cfgf << "dir1/fun1 10;\n"
         << "dir2 { dir3 { fun2; } fun1 5; }"
         << TempFile::close;

    senf::console::ScopedDirectory<> dir1;
    senf::console::root().add("dir1", dir1);
    dir1.add("fun1",fty::Command(&fun1));

    senf::console::ScopedDirectory<> dir2;
    senf::console::root().add("dir2", dir2);

    dir2.add("dir3",fty::Directory()).add("fun2", fty::Command(&fun2));
    dir2.add("fun1", fty::Command(&fun1));

    {
        var1 = 0;
        var2 = false;
        senf::console::ConfigFile cfg (cfgf.name());
        SENF_CHECK_NO_THROW( cfg.parse(dir1) );
        BOOST_CHECK_EQUAL( var1, 10 );
        BOOST_CHECK_EQUAL( var2, false );
        BOOST_CHECK( cfg.parsed(dir1) );

        var1 = 0;
        var2 = false;
        SENF_CHECK_NO_THROW( cfg.parse(dir2["dir3"]) );
        BOOST_CHECK_EQUAL( var1, 0 );
        BOOST_CHECK_EQUAL( var2, true );
        BOOST_CHECK( ! cfg.parsed(dir2) );

        var1 = 0;
        var2 = false;
        SENF_CHECK_NO_THROW( cfg.parse() );
        BOOST_CHECK_EQUAL( var1, 5 );
        BOOST_CHECK_EQUAL( var2, false );
    }
}

SENF_AUTO_UNIT_TEST(configRestrictAndLink)
{
    namespace fty = senf::console::factory;

    TempFile cfgf ("test.cfg");
    cfgf << "dir1/fun1 10;\n"
         << "link1 { dir3 { fun2; } fun1 5; }"
         << TempFile::close;

    senf::console::ScopedDirectory<> dir1;
    senf::console::root().add("dir1", dir1);
    dir1.add("fun1",fty::Command(&fun1));

    senf::console::ScopedDirectory<> dir2;
    dir1.add("dir2", dir2);

    dir2.add("dir3",fty::Directory()).add("fun2", fty::Command(&fun2));
    dir2.add("fun1", fty::Command(&fun1));

    senf::console::ScopedDirectory<> dir4;
    senf::console::root().add("dir4", dir4);
    dir4.add("link1", fty::Link(dir2));

    {
        senf::console::ConfigFile cfg (cfgf.name(), dir4);

        var1 = 0;
        var2 = false;
        SENF_CHECK_NO_THROW( cfg.parse(dir2["dir3"]) );
        BOOST_CHECK_EQUAL( var1, 0 );
        BOOST_CHECK_EQUAL( var2, true );

        BOOST_CHECK_THROW( cfg.parse(), senf::console::SyntaxErrorException );
    }
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
