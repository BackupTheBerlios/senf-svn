// $Id$
//
// Copyright (C) 2013
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on,
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V.
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Thorsten Horstmann <tho@berlios.de>

#ifndef HH_SENF_Utils_Cpp11Support_
#define HH_SENF_Utils_Cpp11Support_ 1

// Custom includes
#include <memory>
#include <utility>
#include <boost/config.hpp>
#include <senf/autoconf.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __clang__
#  if __has_feature(cxx_decltype)
#    define SENF_DECLTYPE decltype
#  else
#    define SENF_DECLTYPE typeof
#  endif
#else
#  ifdef BOOST_HAS_DECLTYPE
#    define SENF_DECLTYPE decltype
#  else
#    define SENF_DECLTYPE typeof
#  endif
#endif

#ifdef HAVE_STD__UNIQUE_PTR_VOID_
#  define unique_or_auto_ptr std::unique_ptr
#  define move_or_noop(t)    std::move(t)
#else
#  define unique_or_auto_ptr std::auto_ptr
#  define move_or_noop(t)    t
#endif

#ifdef SENF_CXX11_ENABLED
#  define SENF_NULLPTR nullptr
#  define SENF_CONSTEXPR constexpr
#else
#  define SENF_NULLPTR NULL
#  define SENF_CONSTEXPR
#endif

#ifdef SENF_CXX11_ENABLED

#include <type_traits>

namespace senf {

    /* stephantl_make_unique.h
       Based on code by Stephan T. Lavavej at
       http://channel9.msdn.com/Series/C9-Lectures-Stephan-T-Lavavej-Core-C-/STLCCSeries6
     */

    namespace detail {

        template<typename T, typename ... Args>
        std::unique_ptr<T> make_unique_helper(std::false_type, Args&&... args) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }

        template<typename T, typename ... Args>
        std::unique_ptr<T> make_unique_helper(std::true_type, Args&&... args) {
            static_assert(std::extent<T>::value == 0,
                    "make_unique<T[N]>() is forbidden, please use make_unique<T[]>(),");
            typedef typename std::remove_extent<T>::type U;
            return std::unique_ptr<T>(new U[sizeof...(Args)]{std::forward<Args>(args)...});
        }
    }

    template<typename T, typename ... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return detail::make_unique_helper<T>(
            std::is_array<T>(),std::forward<Args>(args)... );
    }
}
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Cpp11Support.cci"
//#include "Cpp11Support.ct"
//#include "Cpp11Support.cti"
#endif