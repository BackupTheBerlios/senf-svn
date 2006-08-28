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

// Unit tests

//#include "TCPSocketHandle.test.hh"
//#include "TCPSocketHandle.test.ih"

// Custom includes
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "TCPSocketHandle.hh"
#include <iostream>

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    void error(char const * fn, char const * proc="")
    {
        std::cerr << "\n" << proc << fn << ": " << strerror(errno) << std::endl;
    }

    void fail(char const * fn)
    {
        error(fn,"server: ");
        _exit(1);
    }

    void server()
    {
        int serv = socket(PF_INET,SOCK_STREAM,0);
        if (serv<0) fail("socket()");
        int v = 1;
        if (setsockopt(serv,SOL_SOCKET,SO_REUSEADDR,&v,sizeof(v))<0)
            fail("setsockopt()");
        struct sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = htons(12345);
        sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (bind(serv,(struct sockaddr *)&sin,sizeof(sin))<0) fail("bind()");
        if (listen(serv,1)<0) fail("listen()");
        int sock = accept(serv,0,0);
        if (sock < 0) fail("accept()");

        ///////////////////////////////////////////////////////////////////////////

        char buffer[1024];
        while (1) {
            int n = read(sock,buffer,1024);
            if (n == 4 && strncmp(buffer,"QUIT",4) == 0)
                break;
            write(sock,buffer,n);
        }

        ///////////////////////////////////////////////////////////////////////////

        if (shutdown(sock, SHUT_RDWR) < 0) fail("shutdown()");
        if (close(sock) < 0) fail("close()");
        if (close(serv) < 0) fail("close()");
    }

    int server_pid = 0;

    void start_server()
    {
        server_pid = ::fork();
        if (server_pid < 0) BOOST_FAIL("fork()");
        if (server_pid == 0) {
            server();
            _exit(0);
        }
    }

    void wait_server()
    {
        int status;
        if (waitpid(server_pid,&status,0)<0)
            BOOST_FAIL("waitpid()");
        BOOST_CHECK_EQUAL( status , 0 );
    }

    void stop_server()
    {
        kill(server_pid,9);
        wait_server();
    }

}

BOOST_AUTO_UNIT_TEST(tcpv4ClientSocketHandle)
{
    {
        satcom::lib::TCPv4ClientSocketHandle sock;

        BOOST_CHECK_THROW( sock.connect(satcom::lib::INet4Address("127.0.0.1:12345")), satcom::lib::SystemException );
        BOOST_CHECK_THROW( sock.protocol().connect("127.0.0.1:12345"), satcom::lib::SystemException );
        BOOST_CHECK_THROW( sock.protocol().connect("127.0.0.1",12345), satcom::lib::SystemException );
    }

    {
        start_server();
        satcom::lib::TCPv4ClientSocketHandle sock;
        BOOST_CHECK_NO_THROW( sock.bind(satcom::lib::INet4Address("127.0.0.1:23456")) );
        BOOST_CHECK_NO_THROW( sock.connect(satcom::lib::INet4Address("127.0.0.1:12345")) );
        BOOST_CHECK( sock.peer() == satcom::lib::INet4Address("127.0.0.1:12345") );
        BOOST_CHECK( sock.local() == satcom::lib::INet4Address("127.0.0.1:23456") );
        BOOST_CHECK( sock.blocking() );
        BOOST_CHECK_NO_THROW( sock.rcvbuf(2048) );
        BOOST_CHECK_EQUAL( sock.rcvbuf(), 2048u );
        BOOST_CHECK_NO_THROW( sock.sndbuf(2048) );
        BOOST_CHECK_EQUAL( sock.sndbuf(), 2048u );
        BOOST_CHECK_NO_THROW( sock.write("TEST-WRITE") );
        BOOST_CHECK_EQUAL( sock.read(), "TEST-WRITE" );
        BOOST_CHECK( !sock.eof() );
        sock.write("QUIT");
        sleep(1);
        stop_server();
        BOOST_CHECK_EQUAL( sock.read(), "" );
        BOOST_CHECK( sock.eof() );
        BOOST_CHECK( !sock );
    }

}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
