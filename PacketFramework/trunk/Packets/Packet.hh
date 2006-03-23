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

#ifndef HH_Packet_
#define HH_Packet_ 1

// Custom includes
#include <boost/utility.hpp> // for boost::noncopyable
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <list>
#include <deque>

#include "Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {

    /** \brief Abstract interface of packet facades
        
        The abstract packet interface provides access to the
        uninterpreted raw packet data and to the header chain.
      */
    class Packet : boost::noncopyable
    {
        // These types are implementation details. They are however
        // needed to provide the correct typedefs for the user
        // interface. Hiding these classes would incur a huge
        // additional indirection overhead.

    public:
        typedef boost::uint8_t byte;

    private:
        typedef std::deque<byte> raw_container;
        typedef boost::shared_ptr<Packet> interpreter_list_ptr;
        typedef std::list<interpreter_list_ptr> interpreter_list;
        typedef unsigned refcount_t;

    public:

        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        template <class T> struct ptr_t { typedef boost::intrusive_ptr<T> ptr; };
        typedef ptr_t<Packet>::ptr ptr;
        typedef raw_container::iterator iterator;
        typedef raw_container::size_type size_type;

        // no public constructors
        // no copy
        // no conversion constructors
        // private destructor

        ///////////////////////////////////////////////////////////////////////////

        iterator begin() const;
        iterator end() const;
        size_t size() const;

        ptr next() const;
        ptr prev() const;
        ptr head() const;
        ptr last() const;
        
        template <class OtherPacket> bool is() const;
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr as();

        template <class OtherPacket>
        typename ptr_t<OtherPacket>::ptr reinterpret();
        template <class OtherPacket, class A0>
        typename ptr_t<OtherPacket>::ptr reinterpret(A0 const & a0);

#       define BOOST_PP_ITERATION_PARAMS_1 (4, (2, 9, "Packet.mpp", 1))
#       include BOOST_PP_ITERATE()

        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr find_next();
        template <class OtherPacket> typename ptr_t<OtherPacket>::ptr find_prev();

    private:

        virtual void v_nextInterpreter() const = 0;
        virtual void v_finalize() = 0;

    protected:
        // Construct a packet from external raw data
        template <class InputIterator>
        Packet(InputIterator begin, InputIterator end);
        Packet(raw_container::iterator begin, raw_container::iterator end,
               Packet const * parent);
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

    public:
        class PacketImpl;
    private:
        friend class PacketImpl;

        PacketImpl* impl_;
        iterator begin_;
        iterator end_;
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
