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

#ifndef HH_DataPacket_
#define HH_DataPacket_ 1

// Custom includes
#include "Packet.hh"

///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {

    /** \brief Non-interpreted Packet

        A DataPacket is an uninterpreted blob of data. It is terminal
        in the sense, that no further packet header may follow after
        this packet. A DataPacket implements the abstract Packet
        interface and nothing more.
     */
    class DataPacket : public Packet
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef ptr_t<DataPacket>::ptr ptr;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no public constructors
        // no conversion constructors

        template <class InputIterator>
        static ptr create(InputIterator begin, InputIterator end);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

    private:
        template <class InputIterator>
        DataPacket(InputIterator begin, InputIterator end);
        
        virtual void v_nextInterpreter() const;
        virtual void v_finalize();

        friend class Packet;
    };


}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DataPacket.cci"
//#include "DataPacket.ct"
#include "DataPacket.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
