// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_SocketHandle_
#define HH_SocketHandle_ 1

// Custom includes
#include <memory> // std::auto_ptr
#include "FileHandle.hh"
#include "SocketPolicy.hh"

//#include "SocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////
#include "SocketHandle.ih"

namespace satcom {
namespace lib {

    /** \brief
      */
    template <class SocketPolicy>
    class SocketHandle
        : public FileHandle
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef SocketPolicy Policy;

        template <class OtherPolicy>
        struct IsCompatible
            : public boost::enable_if< SocketPolicyIsBaseOf<SocketPolicy,OtherPolicy>,
                                       SocketHandle >
        {};

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // default copy constructor
        // default copy assignment
        // default destructor

        // conversion constructors
        template <class OtherPolicy>
        SocketHandle(SocketHandle<OtherPolicy> other, 
                     typename IsCompatible<OtherPolicy>::type * = 0);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        template <class OtherPolicy>
        typename IsCompatible<OtherPolicy>::type const & operator=(SocketHandle<OtherPolicy> other);

    protected:
        explicit SocketHandle(std::auto_ptr<SocketProtocol> protocol);
        SocketHandle(FileHandle other, bool isChecked);
        void assign(FileHandle other);

    private:

    };


}}

///////////////////////////////hh.e////////////////////////////////////////
#include "SocketHandle.cci"
//#include "SocketHandle.ct"
#include "SocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
