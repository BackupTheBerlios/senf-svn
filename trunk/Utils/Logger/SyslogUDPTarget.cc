// $Id$
//
// Copyright (C) 2008 
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
    \brief SyslogUDPTarget non-inline non-template implementation */

#include "SyslogUDPTarget.hh"
//#include "SyslogUDPTarget.ih"

// Custom includes
#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>
#include "../Console/Console.hh"

//#include "SyslogUDPTarget.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::log::SyslogUDPTarget::v_write(time_type timestamp, std::string const & stream,
                                                 std::string const & area, unsigned level,
                                                 std::string const & message)
{
    std::string m (message);
    boost::trim_right(m);
    detail::quoteNonPrintable(m);

    std::stringstream prfstream;
    // The space after the '>' is there on purpose: It ensures, that the prefix (which may be empty)
    // or message will not inadvertently be interpreted as date or hostname by a receiving syslog
    // daemon or proxy
    prfstream << '<' << (facility_ | senf::log::SyslogTarget::LEVELMAP[level]) << "> "
              << prefix(timestamp, stream, area, level);
    std::string const & prf (prfstream.str());

    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;
    Separator separator ("\n");
    Tokenizer tokenizer (m, separator);
    Tokenizer::iterator i (tokenizer.begin());
    Tokenizer::iterator const i_end (tokenizer.end());

    std::string line;
    unsigned sz (896-prf.size());
    for (; i != i_end; ++i) 
        for (unsigned j (0); j < i->size(); j += sz) {
            line = prf;
            line += std::string(*i, j, sz);
            handle_.write(line);
        }
}

namespace senf {
namespace log {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER(SyslogUDPTarget, LogFacility,
                                      (AUTHPRIV)(CRON)(DAEMON)(FTP)(KERN)(LPR)(MAIL)(NEWS)(SYSLOG)
                                      (USER)(UUCP)(LOCAL0)(LOCAL1)(LOCAL2)(LOCAL3)(LOCAL4)(LOCAL5)
                                      (LOCAL6)(LOCAL7));

}}

prefix_ senf::log::SyslogUDPTarget::RegisterConsole::RegisterConsole()
{
    namespace kw = senf::console::kw;

    detail::TargetRegistry::instance().consoleDir().add(
        "udp-target", static_cast<void (*)(INet4SocketAddress const &, LogFacility)>(
            &RegisterConsole::create))
        .arg("address", "target address to send log messages to")
        .arg("facility", "syslog facility to send messages to. One of\n"
             "                  AUTHPRIV, CRON, DAEMON, FTP, KERN, LPR, MAIL, NEWS, SYSLOG, USER,\n"
             "                  UUCP, LOCAL0, LOCAL1, LOCAL2, LOCAL3, LOCAL4, LOCAL5, LOCAL6, LOCAL7",
             kw::default_value = USER)
        .doc("Create new udp target. The {address} can be an IPv4 or IPv6 address. If the port\n"
             "number is omitted, it defaults to the default syslog port 514.");
    detail::TargetRegistry::instance().consoleDir().add(
        "udp-target", static_cast<void (*)(INet4Address const &, LogFacility)>(
            &RegisterConsole::create))
        .arg("address")
        .arg("facility", kw::default_value = USER);
    detail::TargetRegistry::instance().consoleDir().add(
        "udp-target", static_cast<void (*)(INet6SocketAddress const &, LogFacility)>(
            &RegisterConsole::create))
        .arg("address")
        .arg("facility", kw::default_value = USER);
    detail::TargetRegistry::instance().consoleDir().add(
        "udp-target", static_cast<void (*)(INet6Address const &, LogFacility)>(
            &RegisterConsole::create))
        .arg("address")
        .arg("facility", kw::default_value = USER);
}

prefix_ void
senf::log::SyslogUDPTarget::RegisterConsole::create(senf::INet4SocketAddress const & target,
                                                    LogFacility facility)
{
    detail::TargetRegistry::instance().dynamicTarget(
        std::auto_ptr<Target>(new SyslogUDPTarget(target, facility)));
}

prefix_ void
senf::log::SyslogUDPTarget::RegisterConsole::create(senf::INet4Address const & target,
                                                    LogFacility facility)
{
    detail::TargetRegistry::instance().dynamicTarget(
        std::auto_ptr<Target>(new SyslogUDPTarget(target, facility)));
}

prefix_ void
senf::log::SyslogUDPTarget::RegisterConsole::create(senf::INet6SocketAddress const & target,
                                                    LogFacility facility)
{
    detail::TargetRegistry::instance().dynamicTarget(
        std::auto_ptr<Target>(new SyslogUDPTarget(target, facility)));
}

prefix_ void
senf::log::SyslogUDPTarget::RegisterConsole::create(senf::INet6Address const & target,
                                                    LogFacility facility)
{
    detail::TargetRegistry::instance().dynamicTarget(
        std::auto_ptr<Target>(new SyslogUDPTarget(target, facility)));
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "SyslogUDPTarget.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
