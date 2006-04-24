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

/** \mainpage The SatCom Packet Framework

    \section arch Overall Architecture

    The general Architecture of the Packet Framework (pkf for short)
    is seperated into two components: The basic packet handling and
    the parser framework.

    The basic packet handling implements a packet interpreter
    chain. Every packet is represented as a chain of interpreters
    where each interpreter is a facade looking into the same
    packet. Each interpreter will interpret a specific header of a
    packet. For example, an ethernet frame might have an interpreter
    chain consisting of EthernetPacket, IPPacket, UDPPacket and
    DataPacket. Each of these interpreters will interpret a section of
    the raw data bytes. The interpreter ranges overlap since every
    packet also includes it's payload.

    The parser framework is used to interpret the raw bytes of a
    specific packet and parse the values present in that packet. For
    example, Parse_Ethernet will parse the ethernet source MAC,
    destination MAC and ethertype given any random access iterator to
    the first byte of the ethernet frame. Parsers are extremely light
    classes. They are temporary classes passed around by value. In
    most cases, they are just comprised of a single pointer adorned
    with type information.

    \section handling Packet Handling

    The packet handling is implemented within
    satcom::pkf::Packet. This class is the baseclass to all packet
    interpreter facades. To implement a new packet type, publically
    derive from satcom::pkf::Packet and implement the virtual
    interface (see the class documentation for details).

    \section framework Parser Framework

    The parser framework provides an abstract framwork to parse packet
    oriented data. A Parser is a template class taking an arbitrary
    iterator as input and allowing random access to data elements of
    the interpreted type, like source and destination MAC of an
    ethernet frame. The parser framework is to be used hierarchically
    and recursively, the parser methods should return further parsers
    which can return further parsers and so on.

    The parser framework contains some basic parsers to be used to
    build up more complex parsers:

     - ParseInt.hh: Lots of parsers for integer numbers like
       satcom::pkf::Parse_UInt8, for integer bitfields like
       satcom::pkf::Parse_UIntField and satcom::pkf::Parse_Flag to
       parse boolean flags.

     - ParseArray.hh: The satcom::pkf::Parse_Array parser to parse
       arbitrary fixed-size arrays of fixed-size elements (that is
       sub-parsers).

     - ParseVec.hh: The satcom::pkf::Parse_Vector parser to parse
       dynamically sized arrays of fixed-size elements (that is
       sub-parsers).

    \section stuff Other Utilities

    The pkf also comprises some additional utilities to support the
    development of packet classes. 

    The satcom::pkf::PacketRegistry implements a registry of packets
    keyed by an arbitrary type. The registry is used to find a packet
    type given some kind of id (like the ethertype value from the
    ethernet header). Together with it's support classes (especially
    satcom::pkf::PacketRegistryMixin) this class greatly simplifies
    implementing the needed table lookups.
 */

#ifndef HH_Packet_
#define HH_Packet_ 1

// Custom includes
#include <boost/utility.hpp> // for boost::noncopyable
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <list>
#include <vector>

#include "Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {
    
    namespace impl { template <class OtherPacket> class PkReg_EntryImpl; }
    namespace impl { class PacketImpl; }

    /** \brief Abstract interface of packet facades
        
        The abstract packet interface provides access to the
        uninterpreted raw packet data and to the header chain.
      */
    class Packet : boost::noncopyable
    {
    public:
        typedef boost::uint8_t byte;

        // These types are implementation details. They are however
        // needed to provide the correct typedefs for the user
        // interface. Hiding these classes would incur a huge
        // additional indirection overhead.

    private:
        typedef std::vector<byte> raw_container;
        typedef boost::shared_ptr<Packet> interpreter_list_ptr;
        typedef std::list<satcom::pkf::Packet::interpreter_list_ptr > interpreter_list;
        typedef unsigned refcount_t;
    public:
        struct inplace_wrapper;

    public:

        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        template <class T> struct ptr_t { typedef boost::intrusive_ptr<T> ptr; };
        typedef ptr_t<Packet>::ptr ptr;
        typedef raw_container::iterator iterator;
        typedef raw_container::size_type size_type;
        typedef raw_container::difference_type difference_type;

        // no public constructors
        // no copy
        // no conversion constructors
        // private destructor

        ///////////////////////////////////////////////////////////////////////////

        // Interpreter chain navigation

        ptr next() const;
        ptr prev() const;
        ptr head() const;
        ptr last() const;
        
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr find_next();
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr find_prev();

        template <class OtherPacket> bool is() const;
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr as();

        // Changing the interpreter chain

        template <class OtherPacket>
        typename ptr_t<OtherPacket>::ptr reinterpret();
        template <class OtherPacket, class A0>
        typename ptr_t<OtherPacket>::ptr reinterpret(A0 const & a0);

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (2, 9, "Packet.mpp", 1))
#       include BOOST_PP_ITERATE()

        // Raw data access

        iterator begin() const;
        iterator end() const;
        size_t size() const;

        // Modifying the raw packet data

        void insert(iterator pos, byte v);
        void insert(iterator pos, size_type n, byte v);
        template <class InputIterator> 
        void insert(iterator pos, InputIterator f, InputIterator l);

        void erase(iterator pos);
        void erase(iterator first, iterator last);

    private:

        virtual void v_nextInterpreter() const = 0;
        virtual void v_finalize() = 0;

    protected:
        template <class InputIterator>
        Packet(InputIterator begin, InputIterator end);
        Packet(inplace_wrapper begin, inplace_wrapper end);
        virtual ~Packet();
        
        template <class OtherPacket>
        typename ptr_t<OtherPacket>::ptr registerInterpreter(
            raw_container::iterator begin, raw_container::iterator end) const;
        template <class OtherPacket, class A0>
        typename ptr_t<OtherPacket>::ptr registerInterpreter(
            raw_container::iterator begin, raw_container::iterator end,
            A0 const & a0) const;

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (2, 9, "Packet.mpp", 3))
#       include BOOST_PP_ITERATE()

    private:

        void add_ref() const;
        bool release();
        bool unlink();
        void registerInterpreter(Packet * p) const;
        void replaceInterpreter(Packet * p);

    private:
        friend class impl::PacketImpl;
        template <class OtherPacket> friend class impl::PkReg_EntryImpl;

        impl::PacketImpl* impl_;
        size_type begin_;
        size_type end_;
        interpreter_list::iterator self_;
        mutable bool parsed_;
        mutable refcount_t refcount_;
    };

    void intrusive_ptr_add_ref(Packet const *);
    void intrusive_ptr_release(Packet *);

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Packet.cci"
#include "Packet.ct"
#include "Packet.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
