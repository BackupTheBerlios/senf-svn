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
    \brief Boost.Test auto unit test compatibility across Boost V 1.33 and 1.34

    This header file will allows to consistently use the Boost Version 1.33 syntax for defining auto
    unit tests. If the available Boost version is 1.34, this file will automatically take care of
    any necessary workarounds.

    So, instead of <tt>\#include <boost/test/auto_unit_test.hpp></tt>, you should always write
    <pre>
    \#include "../Utils/auto_unit_test.hh"
    </pre> (with possibliy adjusted path).
 */

#ifndef HH_SENF_Utils_auto_unit_test_
#define HH_SENF_Utils_auto_unit_test_ 1

// Custom includes
#include <iostream>
#include <boost/version.hpp>

//#include "auto_unit_test.mpp"
#include "auto_unit_test.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#if BOOST_VERSION >= 103400

#   define BOOST_AUTO_UNIT_TEST BOOST_AUTO_TEST_CASE

#endif

#include <boost/test/auto_unit_test.hpp>

/** \defgroup unittest Boost.Test extensions

    This module defines some additional extensions to Boost.Test
 */

/** \brief Check for compile failure

    \c COMPILE_FAIL() is used to check, that a certain piece of code will produce a compile
    time failure.

    \code
    #ifdef COMPILE_CHECK

    COMPILE_FAIL(foo)
    {
        // fails to compile ....
        int x = "foo";
    }

    COMPILE_FAIL(bar)
    { ... }

    #endif
    \endcode

    This check is performed by the extended unit-test builder in \c senfscons.

    \ingroup unittest
 */
#define COMPILE_FAIL(n) void n()

/** \brief Show exception information

    \c SENF_CHECK_NO_THROW() is an extension to \c BOOST_CHECK_NO_THROW() which will write out the
    unabridged exception information when an exception is thrown in addition to signaling an error.

    \ingroup unittest
 */
#define SENF_CHECK_NO_THROW(expr)                                                               \
    BOOST_CHECK_NO_THROW(                                                                       \
        try { expr ; }                                                                          \
        catch (std::exception & e) { std::cerr << e.what() << std::endl; throw; } )

namespace senf {
namespace test {

    template <class Iterator>
    typename detail::NoCharIterator<Iterator>::type nocharIterator(Iterator i);

}}

/** \brief Check ranges for equality, writing char's as int's

    \c SENF_CHECK_EQUAL_COLLECTIONS() is like \c BOOST_CHECK_EQUAL_COLLECTIONS(). The only
    difference is, that \c SENF_CHECK_EQUAL_COLLECTIONS() will write out collection values in
    numeric form (and not as single characters) if the elements are of any char type. Other types
    are not affected.

    \hideinitializer
    \ingroup unittest
 */
#define SENF_CHECK_EQUAL_COLLECTIONS(left_begin, left_end, right_begin, right_end)              \
    BOOST_CHECK_EQUAL_COLLECTIONS(                                                              \
        senf::test::nocharIterator(left_begin), senf::test::nocharIterator(left_end),           \
        senf::test::nocharIterator(right_begin), senf::test::nocharIterator(right_end))

/** \brief Define new test case

    \c SENF_AUTO_UNIT_TEST() is like \c BOOST_AUTO_UNIT_TEST(). The only difference is, that
    if an exception is thrown in the test case \c SENF_AUTO_UNIT_TEST() will write out the
    complete what-message of the exception.

    \hideinitializer
    \ingroup unittest
 */
#define SENF_AUTO_UNIT_TEST(name)                                                               \
    void senf_test_body_##name();                                                               \
    BOOST_AUTO_UNIT_TEST(name)                                                                  \
    {                                                                                           \
        try {                                                                                   \
            senf_test_body_##name();                                                            \
        }                                                                                       \
        catch (std::exception & e) {                                                            \
            std::cerr << e.what() << std::endl;                                                 \
            throw;                                                                              \
        }                                                                                       \
    }                                                                                           \
    void senf_test_body_##name()


#ifdef DOXYGEN
    /** \brief Check for non-equality

        \c SENF_CHECK_NOT_EQUAL() is the opposite of \c BOOST_CHECK_EQUAL.
        \hideinitializer
        \ingroup unittest
     */
    #define SENF_CHECK_NOT_EQUAL( Left, Rigth)
#else
#ifdef BOOST_CHECK_NE
    #define SENF_CHECK_NOT_EQUAL( Left, Right) BOOST_CHECK_NE( Left, Right)
#else
    #define SENF_CHECK_NOT_EQUAL( Left, Right) BOOST_CHECK( Left != Right)
#endif
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "auto_unit_test.cci"
//#include "auto_unit_test.ct"
#include "auto_unit_test.cti"
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
