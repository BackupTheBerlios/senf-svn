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
    \brief ThrottleBarrier non-inline non-template implementation */

#include "ThrottleBarrier.hh"
//#include "ThrottleBarrier.ih"

// Custom includes

//#include "ThrottleBarrier.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::ppi::module::ThrottleBarrier::ThrottleBarrier()
{
    route( input, output ).autoThrottling( false );
    input.onRequest( &ThrottleBarrier::request );
}

prefix_ void senf::ppi::module::ThrottleBarrier::request()
{
    Packet p (input());
    if (output)
        output(p);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "ThrottleBarrier.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
