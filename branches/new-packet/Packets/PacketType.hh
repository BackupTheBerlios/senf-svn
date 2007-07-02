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
#include <iostream>
#include "PacketTypes.hh"
#include "PacketData.hh"
#include "PacketParser.hh"
#include "PacketInterpreter.hh"

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

            static interpreter::size_type initHeadSize()
            {
                // optional member
                return constant_initial_size_of_header
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

            static interpreter::factory_t nextPacketType(interpreter & i)
            {
                if (have_next_packet && know_next_packet_type)
                    // \a TypeOfNextPacket is the \c PacketType class of that packet
                    return interpreter::factory<TypeOfNextPacket>();
                else
                    return interpreter::no_factory();
            }

            static void finalize(interpreter & i)
            {
                // optionally complete the packet by generating autogenerated information
                // (like checksums)
            }

            static void dump(interpreter & i, std::ostream & os)
            {
                // Write out a readable representation of the packet for debug purposes
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

        static void init(interpreter & i);
                                        ///< Initialize new packet
                                        /**< This member is called to initialize a just created new
                                             packet. The new packet will have a size of at least
                                             initSize() but the size may well be larger. It is also
                                             possible for the packet to already have further
                                             subpackets.

                                             The default implementation does nothing. */

        

        static interpreter::optional_range nextPacketRange(interpreter & i);
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

        static interpreter::factory_t nextPacketType(interpreter & i);
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

        static void finalize(interpreter & i);
                                        ///< Finalize packet
                                        /**< finalize() will be called to complete a packet after it
                                             has been modified. This function must calculate any
                                             checksums, set size fields from the interpreter chain
                                             etc. 
                                             
                                             The default implementation does nothing. */

        static void dump(interpreter & i, std::ostream & os);
                                        ///< Dump packet data in readable form
                                        /**< The dump() function writes out a complete
                                             representation of the packet. This is used formost for
                                             debugging purposes. 

                                             The default implementation does nothing. */
    };


    /** \brief Mixin to provide standard implementations for nextPacketRange and nextPacketType

        This mixin class simplifies the definition of simple packets with fixed-size headers and/or
        trailers. For this type of Packet, this mixin provides the nextPacketRange() and
        nextPacketType() members.
        \code
        struct SimplePacketType 
            : public senf::PacketTypeBase
              pyblic senf:PacketTypeMixin<SimplePacketType, SomeRegistryTag>
        {
            static interpreter::size_type initSize()
            {
                // The value returned is the length of the header if initHeadSize() is not defined.
                // If initHeadSize is defined, this value is the combined size of the header
                // and trailer while initHeadSize() will return the size of the header only.
                return packet_size;
            }
        
            static interpreter::size_type initHeadSize()
            {
                // This member is optional. It returns the header size if the packet has a
                // trailer.
                return header_size;
            }

            static registry_key_t nextPacketKey(interpreter & i)
            {
                // Return the key in the registry under which the next packet
                // header is to be found. This member must be given if a Registry argument is
                // passed to the PacketTypeMixin template.
                return i.fields().typeField();
            }
        };
        \endcode
     */
    template <class Self, class Registry=void>
    class PacketTypeMixin
    {
    public:
        typedef PacketInterpreterBase interpreter;
        typedef typename Registry::key_t registry_key_t;

        static interpreter::optional_range nextPacketRange (interpreter & i);
        static interpreter::factory_t      nextPacketType  (interpreter & i);
    };

    template <class Self>
    class PacketTypeMixin<Self,void>
    {
    public:
        typedef PacketInterpreterBase interpreter;

        static interpreter::optional_range nextPacketRange (interpreter & i);
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "PacketType.cci"
//#include "PacketType.ct"
#include "PacketType.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
