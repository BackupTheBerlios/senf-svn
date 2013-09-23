// $Id$
//
// Copyright (C) 2013
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
    \brief MMapSocketProtocol public header */

#ifndef HH_SENF_senf_Socket_Protocols_Raw_MMapSocketProtocol_
#define HH_SENF_senf_Socket_Protocols_Raw_MMapSocketProtocol_ 1

// Custom includes
#include <senf/Socket/SocketProtocol.hh>
#include <senf/Socket/QueueReadWritePolicy.hh>

//#include "MMapSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class MMapSocketProtocol
        : public virtual SocketProtocol,
          private FileHandleAccess
    {
    protected:
        void init_mmap(unsigned frameSize, unsigned rxqlen, unsigned txqlen) const;
        void close_mmap() const;
        void terminate_mmap() const;

        mutable detail::QueueInfo qi_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MMapSocketProtocol.cci"
//#include "MMapSocketProtocol.ct"
//#include "MMapSocketProtocol.cti"
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
