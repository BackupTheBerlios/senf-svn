// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief Connectors non-inline non-template implementation */

#include "Connectors.hh"
#include "Connectors.ih"

// Custom includes
#include "Route.hh"
#include "Module.hh"
#include "ModuleManager.hh"
#include <senf/Utils/Console/Console.hh>

//#include "Connectors.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::Connector

prefix_ void senf::ppi::connector::Connector::connect(Connector & target)
{
    // The connector is not registered -> route() or noroute() statement missing
    SENF_ASSERT( module_ && 
                 "senf::ppi::connector::Connector::connect(): (source) "
                 "Missing route() or noroute()" );
    // The connector is already connected
    SENF_ASSERT( ! peer_ &&
                 "senf::ppi::connector::Connector::connect(): (source) "
                 "duplicate connection" );
    // The target connector is not registered -> route() or noroute() statement missing
    SENF_ASSERT( target.module_ &&
                 "senf::ppi::connector::Connector::connect(): (target) "
                 "Missing route() or noroute()" );
    // The target connector is already connected
    SENF_ASSERT( ! target.peer_ &&
                 "senf::ppi::connector::Connector::connect(): (target) "
                 "duplicate connection" );
    if (! (packetTypeID() == typeid(void) ||
           target.packetTypeID() == typeid(void) || 
           packetTypeID() == target.packetTypeID()) )
        throw IncompatibleConnectorsException() 
            << ": " << prettyName(packetTypeID()) 
            << " [in module " << prettyName(typeid(*module_))  << "] "
            << ", " << prettyName(target.packetTypeID())
            << " [in module " << prettyName(typeid(*target.module_)) << "]";
            
    peer_ = & target;
    target.peer_ = this;

    if (! initializationScheduled())
        enqueueInitializable();
    if (! peer().initializationScheduled())
        peer().enqueueInitializable();
}

senf::ppi::connector::Connector::TraceState senf::ppi::connector::Connector::traceState_ (
    senf::ppi::connector::Connector::NO_TRACING);

prefix_ void senf::ppi::connector::Connector::trace(Packet const & p, char const * label)
{
    if (traceState_ ==  NO_TRACING)
        return;
    SENF_LOG_BLOCK(({
                std::string type (prettyName(p.typeId().id()));
                log << "PPI packet trace: " << label << " 0x" << std::hex << p.id() << " " 
                    << type.substr(21, type.size()-22) << " on " << & module() << " " 
                    << prettyName(typeid(module())) << " connector 0x" << this << "\n";
                if (traceState_ == TRACE_CONTENTS)
                    p.dump(log);
            }));
}

prefix_ void senf::ppi::connector::Connector::throttleTrace(char const * label,
                                                            char const * type)
{
    if (traceState_ == NO_TRACING)
        return;
    SENF_LOG_BLOCK(({
                log << "PPI throttling trace: " << label << " " << type << " on " << & module() 
                    << " " << prettyName(typeid(module())) << " connector 0x" << this << "\n";
            }));
}

namespace senf { namespace ppi { namespace connector {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER( 
        Connector, TraceState, (NO_TRACING)(TRACE_IDS)(TRACE_CONTENTS) );

}}}

namespace {

    struct ConsoleRegister
    {
        ConsoleRegister();
    };

    ConsoleRegister::ConsoleRegister()
    {
        senf::ppi::ModuleManager::instance().consoleDir()
            .add("tracing", senf::console::factory::Command(
                     SENF_FNP(senf::ppi::connector::Connector::TraceState,
                              senf::ppi::connector::Connector::tracing, ()))
                 .doc("Log every packet sent or received by any module.\n"
                      "There are three different tracing levels:\n"
                      "\n"
                      "    NO_TRACING      don't output any tracing information\n"
                      "    TRACE_IDS       trace packet id's but do not show packet contents\n"
                      "    TRACE_CONTENTS  trace complete packet contents\n"
                      "\n"
                      "A log message is generated whenever the packet traverses a connector. The\n"
                      "TRACE_IDS log message has the following format:\n"
                      "\n"
                      "    PPI packet trace: <direction> <packet-id> <packet-type>\n"
                      "                      on <module-id> <module-type> connector <connector-id>\n"
                      "    PPI throttling trace: <direction> <throttle-msg>\n"
                      "                      on <module-id> <module-type> connector <connector-id>\n"
                      "\n"
                      "The fields are:\n"
                      "\n"
                      "    direction       'IN' for packets/throttle notifications entering the module,\n"
                      "                    'OUT' for packets/throttle notifications leaving it\n"
                      "    packet-id       Numeric unique packet id. This value is unique for packets\n"
                      "                    alive at the same time, packets at different times may (and\n"
                      "                    will) share id's\n"
                      "    packet-type     The type of the packet header\n"
                      "    module-id       Unique module id\n"
                      "    module-type     Type of the module the packet is sent to/from\n"
                      "    connector-id    Unique connector id\n"
                      "    throttle-msg    Type of throttling event\n")
                );

        senf::ppi::ModuleManager::instance().consoleDir()
            .add("tracing", senf::console::factory::Command(
                     SENF_FNP(void, senf::ppi::connector::Connector::tracing,
                              (senf::ppi::connector::Connector::TraceState)))
                 .arg("state", "new tracing state")
                );
    }

    ConsoleRegister consoleRegister;

}

prefix_ void senf::ppi::connector::Connector::disconnect()
{
    // Cannot disconnected a non-connected connector
    SENF_ASSERT( peer_ &&
                 "senf::ppi::connector::Connector::disconnect(): Not connected" );

    Connector & peer (*peer_);
    peer_ = 0;
    peer.peer_ = 0;

    if (! initializationScheduled())
        enqueueInitializable();
    if (! peer.initializationScheduled())
        peer.enqueueInitializable();

    v_disconnected();
    peer.v_disconnected();
}

prefix_ std::type_info const & senf::ppi::connector::Connector::packetTypeID()
{
    return typeid(void);
}

prefix_ void senf::ppi::connector::Connector::v_disconnected()
    const
{}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::PassiveConnector

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::PassiveConnector::v_init()
{
    Routes::const_iterator i (routes_.begin());
    Routes::const_iterator const i_end (routes_.end());
    for (; i != i_end; ++i)
        if ((*i)->throttled())
            break;
    if (i == i_end)
        remoteThrottled_ = false;
    if (throttled())
        emitThrottle();
    else
        emitUnthrottle();
}

prefix_ void senf::ppi::connector::PassiveConnector::v_unthrottleEvent()
{}

prefix_ void senf::ppi::connector::PassiveConnector::notifyUnthrottle()
{
    if (std::find_if(routes_.begin(), routes_.end(), 
                     boost::bind(&ForwardingRoute::throttled, _1)) == routes_.end()) {
        remoteThrottled_ = false;
        if (!nativeThrottled_)
            emitUnthrottle();
    } else
        throttleTrace("OUT", "not forwarding unthrottle event");
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::ActiveConnector

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::ActiveConnector::v_init()
{
    if (! connected())
        notifyThrottle();
}

prefix_ void senf::ppi::connector::ActiveConnector::notifyThrottle()
{
    throttleTrace("IN ", "throttle");
    if (! throttled_) {
        throttled_ = true;
        if (throttleCallback_)
            throttleCallback_();
        NotifyRoutes::const_iterator i (notifyRoutes_.begin());
        NotifyRoutes::const_iterator const i_end (notifyRoutes_.end());
        for (; i != i_end; ++i)
            (*i)->notifyThrottle();
    }
}

prefix_ void senf::ppi::connector::ActiveConnector::notifyUnthrottle()
{
    throttleTrace("IN ", "unthrottle");
    if (throttled_) {
        throttled_ = false;
        if (unthrottleCallback_)
            unthrottleCallback_();
        NotifyRoutes::const_iterator i (notifyRoutes_.begin());
        NotifyRoutes::const_iterator const i_end (notifyRoutes_.end());
        for (; i != i_end; ++i)
            (*i)->notifyUnthrottle();
    }
}

prefix_ void senf::ppi::connector::ActiveConnector::registerRoute(ForwardingRoute & route)
{
    notifyRoutes_.push_back(&route);
}

prefix_ void senf::ppi::connector::ActiveConnector::unregisterRoute(ForwardingRoute & route)
{
    NotifyRoutes::iterator i (std::find(notifyRoutes_.begin(), notifyRoutes_.end(), &route));
    if (i != notifyRoutes_.end())
        notifyRoutes_.erase(i);
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::InputConnector

prefix_ senf::Packet senf::ppi::connector::InputConnector::operator()()
{
    if (empty())
        v_requestEvent();
    Packet p;
    if (! empty()) {
        p = peek();
        queue_.pop_back();
        v_dequeueEvent();
    }
    trace(p, "IN ");
    return p;
}

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::InputConnector::v_requestEvent()
{}

prefix_ void senf::ppi::connector::InputConnector::v_enqueueEvent()
{}

prefix_ void senf::ppi::connector::InputConnector::v_dequeueEvent()
{}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::GenericActiveInput

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::GenericActiveInput::v_requestEvent()
{
    request();
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::GenericPassiveInput

////////////////////////////////////////
// private members 

prefix_ void senf::ppi::connector::GenericPassiveInput::v_enqueueEvent()
{
    emit();
    qdisc_->update(*this, QueueingDiscipline::ENQUEUE);
}

prefix_ void senf::ppi::connector::GenericPassiveInput::v_dequeueEvent()
{
    qdisc_->update(*this, QueueingDiscipline::DEQUEUE);
}

prefix_ void senf::ppi::connector::GenericPassiveInput::v_unthrottleEvent()
{
    size_type n (queueSize());
    while (n) {
        emit();
        size_type nn (queueSize());
        if (n == nn)
            break;
        n = nn;
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Connectors.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
