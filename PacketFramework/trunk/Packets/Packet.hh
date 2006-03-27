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
#include <vector>

#include "Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace pkf {
    
    namespace impl { template <class Container> class deque_inserter; }

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
        typedef std::list<interpreter_list_ptr> interpreter_list;
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
        typedef impl::deque_inserter<raw_container> inserter;

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

    public:
        // If this is non-public, the access control gets far to
        // complicated ...
        class PacketImpl;
    private:
        friend class PacketImpl;

        PacketImpl* impl_;
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
