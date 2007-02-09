// $Id$
//
// Copyright (C) 2006 Stefan Bund <g0dil@senf.berlios.de>
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
    \brief MicroTime non-inline non-template implementation */

#include "MicroTime.hh"
//#include "MicroTime.ih"

// Custom includes
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "Utils/Exception.hh"

//#include "MicroTime.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::MicroTime senf::now()
{
    struct timeval tv;
    if (gettimeofday(&tv,0) < 0)
	throw SystemException(errno);
    return 1000000*MicroTime(tv.tv_sec) + tv.tv_usec;
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "MicroTime.mpp"


// Local Variables:
// mode: c++
// End:
