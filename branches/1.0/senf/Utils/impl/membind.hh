// $Id$
//
// Copyright (C) 2006
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
//
// NEVER INCLUDE DIRECTLY !! INCLUDE senf/membind.hh

#if !BOOST_PP_IS_ITERATING

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/enum_shifted.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>

template <typename R, typename T1, typename T2>
boost::function<R()> membind(R (T1::* fn)(),T2 scOBTYPE ob)
{
    return boost::bind(fn,static_cast<T1 scOBTYPE>(ob));
}

template <typename R, typename T1, typename T2>
boost::function<R()> membind(R (T1::* fn)() const, T2 const scOBTYPE ob)
{
    return boost::bind(fn,static_cast<T1 const scOBTYPE>(ob));
}

template <typename R, typename T1, typename T2>
boost::function<R()> membind(R (T1::* fn)() const, T2 scOBTYPE ob)
{
    return boost::bind(fn,static_cast<T1 const scOBTYPE>(ob));
}

// for BOOST_PP_ITERATION() in 2..9 do
#define BOOST_PP_ITERATION_PARAMS_1 (4, (2, 9, SENF_ABSOLUTE_INCLUDE_PATH(Utils/impl/membind.hh), 1))
#include BOOST_PP_ITERATE()
#elif BOOST_PP_ITERATION_DEPTH()==1 && BOOST_PP_ITERATION_FLAGS()==1
// {{

#define scARG(z,n,d) BOOST_PP_CAT(d,n)
#define scPARAMS(d) BOOST_PP_ENUM_SHIFTED(BOOST_PP_ITERATION(),scARG,d)

template < typename R, typename T1, typename T2, scPARAMS(typename A) >
boost::function<R ( scPARAMS(A) )>
membind(R (T1::* fn)( scPARAMS(A) ), T2 scOBTYPE ob)
{
    return boost::bind(fn, static_cast<T1 scOBTYPE>(ob), scPARAMS(_) );
}

template < typename R, typename T1, typename T2, scPARAMS(typename A) >
boost::function<R ( scPARAMS(A) )>
membind(R (T1::* fn)( scPARAMS(A) ) const, T2 const scOBTYPE ob)
{
    return boost::bind(fn, static_cast<T1 const scOBTYPE>(ob), scPARAMS(_) );
}

template < typename R, typename T1, typename T2, scPARAMS(typename A) >
boost::function<R ( scPARAMS(A) )>
membind(R (T1::* fn)( scPARAMS(A) ) const, T2 scOBTYPE ob)
{
    return boost::bind(fn, static_cast<T1 const scOBTYPE>(ob), scPARAMS(_) );
}

#undef scPARAMS
#undef scARG

// }
#endif
// done


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
