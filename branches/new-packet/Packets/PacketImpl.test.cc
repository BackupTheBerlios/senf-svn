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
    \brief PacketImpl.test unit tests */

//#include "PacketImpl.test.hh"
//#include "PacketImpl.test.ih"

// Custom includes
#include "PacketImpl.hh"
#include "PacketInterpreter.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

// PacketImpl cannot be tested without relying on PacketInterpreterBase.  However these unit-tests
// only validate PacketInterpreterBase as far as to ensure that a failure of one test is not due to
// an error in PacketInterpreterbase

BOOST_AUTO_UNIT_TEST(packetImpl_mem)
{
    std::auto_ptr<senf::detail::PacketImpl> p (senf::detail::PacketImpl::create());
    BOOST_CHECK_EQUAL(p->refcount(), 0);
    p->add_ref();
    BOOST_CHECK_EQUAL(p->refcount(), 1);
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 1u);
    // From now on, the object should stay alive since I manually incremented the
    // refcount ..


    p->add_ref(2);
    BOOST_CHECK_EQUAL(p->refcount(), 3);
    p->release(2);
    BOOST_CHECK_EQUAL(p->refcount(), 1);

    {
        senf::PacketInterpreterBase::ptr pi (
            senf::PacketInterpreterBase::create(0,0));
        // Hmm ... this check works as long as sizeof(PacketInterpreterBase> !=
        // sizeof(PacketImpl) ... !!
        BOOST_CHECK_EQUAL(
            senf::pool_alloc_mixin<senf::PacketInterpreterBase>::allocCounter(), 1u);
        p->appendInterpreter(pi.get());
        senf::PacketInterpreterBase::ptr pi2 (pi);
        BOOST_CHECK_EQUAL(p->refcount(), 3);
    }
    BOOST_CHECK_EQUAL(p->refcount(),1);

    {
        senf::PacketInterpreterBase::ptr pi (p->first());
        BOOST_CHECK_EQUAL(p->refcount(),2);
        p->truncateInterpreters(pi.get());
        BOOST_CHECK_EQUAL(p->refcount(),1);
    }
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::PacketInterpreterBase>::allocCounter(), 0u);
    BOOST_CHECK_EQUAL(p->refcount(),1);

    // The refcount must be one here (from incrementing the refcount above)
    // Therefore we can safely delete the object.
    BOOST_CHECK_EQUAL(p->refcount(), 1);
    p.release()->release();
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 0u);
}

BOOST_AUTO_UNIT_TEST(packetImpl_data)
{
    std::auto_ptr<senf::detail::PacketImpl> p (senf::detail::PacketImpl::create());
    p->add_ref();

    {
        senf::PacketInterpreterBase::ptr pi (
            senf::PacketInterpreterBase::create(0,0));
        p->appendInterpreter(pi.get());

        senf::detail::PacketImpl::byte data[] = 
            { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };

        p->insert(pi.get(),p->begin(),data, data+sizeof(data));
        BOOST_CHECK_EQUAL(p->size(), 8u);
        BOOST_CHECK_EQUAL(p->begin()[0], 0x00u);
        BOOST_CHECK_EQUAL(p->begin()[7], 0x07u);
        BOOST_CHECK_EQUAL(pi->size(), p->size());
        
        p->insert(pi.get(),p->begin()+2,0xf0u);
        BOOST_CHECK_EQUAL(p->size(),9u);
        BOOST_CHECK_EQUAL(p->begin()[8], 0x07u);
        BOOST_CHECK_EQUAL(p->begin()[2], 0xf0u);
        BOOST_CHECK_EQUAL(pi->size(), p->size());

        p->insert(pi.get(),p->begin()+9,8,0xffu);
        BOOST_CHECK_EQUAL(p->size(),17u);
        BOOST_CHECK_EQUAL(p->begin()[16], 0xffu);
        BOOST_CHECK_EQUAL(p->begin()[8], 0x07u);
        BOOST_CHECK_EQUAL(pi->size(), p->size());

        p->erase(pi.get(),p->begin());
        BOOST_CHECK_EQUAL(p->size(),16u);
        BOOST_CHECK_EQUAL(p->begin()[0], 0x01u);
        BOOST_CHECK_EQUAL(pi->size(), p->size());
        
        p->erase(pi.get(),p->begin()+2, p->begin()+7);
        BOOST_CHECK_EQUAL(p->size(),11u);
        BOOST_CHECK_EQUAL(p->begin()[2], 0x07u);
        BOOST_CHECK_EQUAL(p->begin()[3], 0xffu);
        BOOST_CHECK_EQUAL(pi->size(), p->size());

        BOOST_REQUIRE_EQUAL(pi->size(), p->size());
        BOOST_REQUIRE(pi->begin() == p->begin());

        p->clear(pi.get());
        BOOST_CHECK_EQUAL(p->size(), 0u);
        BOOST_CHECK_EQUAL(pi->size(), 0u);
        BOOST_CHECK(pi->begin() == p->begin());
    }

    BOOST_CHECK_EQUAL(p->refcount(), 1);
    p.release()->release();
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 0u);
}

BOOST_AUTO_UNIT_TEST(packetImpl_interpreters)
{
    std::auto_ptr<senf::detail::PacketImpl> p (senf::detail::PacketImpl::create());
    p->add_ref();
    
    {
        senf::PacketInterpreterBase::ptr pi2 (
            senf::PacketInterpreterBase::create(0,0));
        p->appendInterpreter(pi2.get());
        senf::PacketInterpreterBase::ptr pi3 (
            senf::PacketInterpreterBase::create(0,0));
        p->appendInterpreter(pi3.get());
        senf::PacketInterpreterBase::ptr pi1 (
            senf::PacketInterpreterBase::create(0,0));
        p->prependInterpreter(pi1.get());

        BOOST_CHECK_EQUAL(p->first(), pi1.get());
        BOOST_CHECK_EQUAL(p->next(p->first()), pi2.get());
        BOOST_CHECK_EQUAL(p->next(p->next(p->first())), pi3.get());
        BOOST_CHECK( !p->next(p->next(p->next(p->first()))) );

        BOOST_CHECK_EQUAL(p->last(), pi3.get());
        BOOST_CHECK_EQUAL(p->prev(p->last()), pi2.get());
        BOOST_CHECK_EQUAL(p->prev(p->prev(p->last())), pi1.get());
        BOOST_CHECK( !p->prev(p->prev(p->prev(p->last()))) );

        p->insert(pi2.get(),p->begin(),10,0x00u);
        BOOST_CHECK_EQUAL(pi1->size(), 10u);
        BOOST_CHECK_EQUAL(pi2->size(), 10u);
        BOOST_CHECK_EQUAL(pi3->size(), 0u);
        BOOST_CHECK( pi1->begin() == p->begin() );
        BOOST_CHECK( pi2->begin() == p->begin() );
        BOOST_CHECK( pi3->begin() == p->end() );
        
        p->insert(pi3.get(),p->end(), 0x00u);
        BOOST_CHECK_EQUAL(pi1->size(), 11u);
        BOOST_CHECK_EQUAL(pi2->size(), 11u);
        BOOST_CHECK_EQUAL(pi3->size(), 1u);
        
        p->insert(pi1.get(),p->end(), 2, 0x00u);
        BOOST_CHECK_EQUAL(pi1->size(), 13u);
        BOOST_CHECK_EQUAL(pi2->size(), 11u);
        BOOST_CHECK_EQUAL(pi3->size(), 1u);
        BOOST_CHECK( pi1->end() == p->begin()+13u );
        BOOST_CHECK( pi2->end() == p->begin()+11u );
        BOOST_CHECK( pi3->end() == p->begin()+11u );

        p->clear(pi2.get());
        BOOST_CHECK_EQUAL(pi1->size(), 2u);
        BOOST_CHECK( ! p->next(p->next(p->first())) );
    }

    BOOST_CHECK_EQUAL(p->refcount(), 1);
    p.release()->release();
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 0u);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
