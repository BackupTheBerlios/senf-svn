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
    \brief PacketType public header */

#ifndef HH_PacketType_
#define HH_PacketType_ 1

// Custom includes
#include "PacketTypes.hh"
#include "PacketData.hh"
#include "PacketParser.hh"

//#include "PacketType.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Helper baseclass implementing the PacketType interface

        This helper class maybe used when building a new PacketType. It provides a complete default
        implementations of this interface. To define a new PacketType, derive from this class and
        implement the members you want to change.

        \code
        struct SomePacketType : public senf::PacketTypeBase
        {
            typedef senf::PacketIterpreter<SomePacketType> interpreter;
            
            static interpreter::size_type initSize()
            {
                return constant_initial_size_of_empty_packet;
            }

            static void init(interpreter & i)
            {
                // Initialize a new packet by e.g. setting some fields
                i.fields().some_field = 1;
            }

            static interpreter::optional_range nextPacketRange(interpreter & i)
            {
                if (have_next_packet)
                    // return iterator range delimiting the packet, e.g.:
                    return interpreter::range(i.data()+begin_offset, i.data()+end_offset);
                else
                    return interpreter::no_range();
            }

            static interpreter::factory_t nextPacketType(interpreter &)
            {
                if (have_next_packet && know_next_packet_type)
                    // \a TypeOfNextPacket is the \c PacketType class of that packet
                    return interpreter::factory<TypeOfNextPacket>();
                else
                    return interpreter::no_factory();
            }
        }
        \endcode

        You may leave out any one of the members (you should however always define the \c
        interpreter typedef member)
      */
    struct PacketTypeBase
    {
        typedef PacketInterpreterBase interpreter;

        typedef VoidPacketParser parser;
                                        ///< Parser to parser packet fields
                                        /**< This typedef has to be set to the parser of the packet
                                             
                                             The default is a VoidPacketParser which does not parser
                                             any field. */

        static interpreter::size_type initSize();
                                        ///< Get size of new (empty) packet
                                        /**< The default implementation returns 0.
                                             \returns size that needs to be allocated to a newly
                                             constructed packet */

        static interpreter::size_type initHeadSize();
                                        ///< Get size of new (empty) packet header
                                        /**< This function gives the index within a newly created,
                                             empty packet where a subpacket is to be placed.

                                             The default implementation returns initSize(). 
                                             
                                             \implementation Ok, it does not really return
                                                 initSize(), it returns size_type(-1) which is
                                                 interpreted to mean initSize(). It can't return
                                                 initSize since initSize() is not (and can't be
                                                 since it is static) virtual. */

        static void init(interpreter &);
                                        ///< Initialize new packet
                                        /**< This member is called to initialize a just created new
                                             packet. The new packet will have a size of at least
                                             initSize() but the size may well be larger. It is also
                                             possible for the packet to already have further
                                             subpackets.

                                             The default implementation does nothing. */

        

        static interpreter::optional_range nextPacketRange(interpreter &);
                                        ///< Get next packet placement
                                        /**< nextPacketRange returns the iterator range where the
                                             next packet (header) is placed within the current
                                             packet.
                                            
                                             The default implementation always returns
                                             <tt>no_range()</tt>.

                                             \returns nextPacketRange must return
                                             \li <tt>\ref interpreter::range(b,e)</tt> (where \a b
                                                 and \a e are the iterators delimiting the next
                                                 packet range) if the packet allows a next
                                                 header. The returned range is allowed to be empty.
                                             \li <tt>\ref interpreter::no_range()</tt> (a
                                                 default constructed \c no_range instance) if
                                                 no next header is allowed */

        static interpreter::factory_t nextPacketType(interpreter &);
                                        ///< Get type of next packet
                                        /**< nextPacketType retrieves the type of the next packet
                                             returning a factory to create that packet.
                                             
                                             The default implementation always returns
                                             <tt>no_factory()</tt>.

                                             \returns factory to create next packet
                                             \li <tt>interpreter::factory<OtherPacketType>()</tt> if
                                                 the type of the packet can be determined
                                             \li <tt>interpreter::no_factory()</tt> if the type of
                                                 the packet cannot be determined or no next packet
                                                 is supported */
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "PacketType.cci"
//#include "PacketType.ct"
//#include "PacketType.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
