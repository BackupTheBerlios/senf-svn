// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief Parameters public header */

#ifndef HH_Parameters_
#define HH_Parameters_ 1

// Custom includes

//#include "Parameters.mpp"
#include "Parameters.ih"
///////////////////////////////hh.p////////////////////////////////////////

/** \brief Set scope default log stream
    \hideinitializer
 */
#define SENF_LOG_DEFAULT_STREAM(stream) typedef stream SENFLogDefaultStream

/** \brief Set scope default log area
    \hideinitializer
 */
#define SENF_LOG_DEFAULT_AREA(area) typedef area SENFLogDefaultArea

/** \brief Set scope default log level
    \hideinitializer
 */
#define SENF_LOG_DEFAULT_LEVEL(level) typedef level SENFLogDefaultLevel

/** \brief Define log parameter alias

    Defines a new parameter alias named \a alias as an alias for the parameters in \a args. The
    alias is defined as a symbol in the current scope.

    \hideinitializer
 */
#define SENF_LOG_DEF_ALIAS(alias,args)                                                            \
    struct alias : public senf::log::detail::AliasBase                                            \
    {                                                                                             \
        template <class Base>                                                                     \
        struct apply                                                                              \
        {                                                                                         \
            typedef typename SENF_LOG_MERGE_PARAMETERS_I(Base,args) type;                         \
        };                                                                                        \
    }

///////////////////////////////hh.e////////////////////////////////////////
//#include "Parameters.cci"
//#include "Parameters.ct"
//#include "Parameters.cti"
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
