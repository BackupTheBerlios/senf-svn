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
    \brief type_traits unit tests */

//#include "type_traits.test.hh"
//#include "type_traits.test.ih"

// Custom includes
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include "type_traits.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct Class;
}

SENF_AUTO_UNIT_TEST(typeTraits)
{
    typedef boost::function_traits<void (int, double)> traits;

    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::function_traits_remove_arg< traits >::type, 
        boost::function_traits<void (double)> 
    >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::function_traits_arg_type< traits, 0 >::type,
        int
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::function_traits_arg_type< traits, 2 >::type,
        void
    >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_member_pointer< Class (Class::*) >::type, 
        Class 
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_member_pointer< Class const (Class::*) >::type, 
        Class const
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_member_pointer< Class (Class::*)(int) >::type, 
        Class (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_member_pointer< Class const (Class::*)(int) >::type, 
        Class const (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_member_pointer< void (Class::*)(int) const>::type, 
        void (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_member_pointer< Class const (Class::*)(int) const>::type, 
        Class const (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_member_pointer< void (Class::* const)(int)>::type, 
        void (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_member_pointer< void (Class::* const)(int) const>::type, 
        void (int)
    >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::member_class< int (Class::*) >::type,
        Class
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::member_class< int * >::type,
        void
    >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_any_pointer< int (Class::*) >::type, 
        int
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same< 
        senf::remove_any_pointer< void (Class::*)(int) >::type, 
        void (int)
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_any_pointer < int (*)() >::type,
        int ()
    >::value ));
    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_any_pointer < int >::type,
        int
    >::value ));

    BOOST_STATIC_ASSERT((   senf::is_any_function< void () >::value ));
    BOOST_STATIC_ASSERT((   senf::is_any_function< void (*)(int) >::value ));
    BOOST_STATIC_ASSERT((   senf::is_any_function< void (Class::*)() >::value ));
    BOOST_STATIC_ASSERT((   senf::is_any_function< void (Class::*)() const >::value ));
    BOOST_STATIC_ASSERT(( ! senf::is_any_function< int * >::value ));

    BOOST_STATIC_ASSERT(( boost::is_same<
        senf::remove_cvref<int const &>::type,
        int
    >::value ));

    BOOST_STATIC_ASSERT(( senf::function_arity<void ()>::value == 0 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (int,int)>::value == 2 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (*)()>::value == 0 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (*)(int,int)>::value == 2 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (Class::*)()>::value == 0 ));
    BOOST_STATIC_ASSERT(( senf::function_arity<void (Class::*)(int,int)>::value == 2 ));

    BOOST_STATIC_ASSERT(( senf::function_arity<void (Class::*)(int)>::value == 1 ));

    BOOST_STATIC_ASSERT((   senf::is_pair< std::pair<int,void*> >::value ));
    BOOST_STATIC_ASSERT(( ! senf::is_pair< void () >::value ));
    
    BOOST_CHECK( true );
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
