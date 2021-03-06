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

// Custom includes
#include <string>
#include <senf/Socket/Protocols/INet.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>

class Server
{
    senf::UDPv4ClientSocketHandle serverSock;

public:
    Server(senf::INet4Address const & host, unsigned int port)
        : serverSock(senf::INet4SocketAddress(host, port)) {}

    void run()
    {
        senf::Scheduler::instance().add(
                serverSock,
                senf::membind(&Server::readFromClient, this),
                senf::Scheduler::EV_READ);
        senf::Scheduler::instance().process();
    }

private:
    void readFromClient(senf::Scheduler::EventId event)
    {
        std::string data (serverSock.read());
        std::cout << "> " << data<<std::endl ;
    }
};

int main(int argc, char const * argv[])
{
    try {
        Server testSock(senf::INet4Address::Loopback, 4243);
        testSock.run();
    }

    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    return 0;
}
