// $Id$
//
// Copyright (C) 2006
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
    \brief TCPv4SocketHandle and TCPv6SocketHandle public header

    \todo Implement possibly non-blocking connect and SO_ERROR in the
    protocol interface
 */

#ifndef HH_SENF_Socket_Protocols_INet_TCPSocketHandle_
#define HH_SENF_Socket_Protocols_INet_TCPSocketHandle_ 1

// Custom includes
#include "INetSocketProtocol.hh"
#include "TCPSocketProtocol.hh"
#include "INetAddressing.hh"
#include <senf/Socket/Protocols/BSDSocketProtocol.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>
#include <senf/Socket/ProtocolServerSocketHandle.hh>

//#include "TCPSocketHandle.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    //\{

    typedef MakeSocketPolicy<
        INet4AddressingPolicy,
        StreamFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy TCPv4Socket_Policy;   ///< Socket Policy of the TCPv4 Protocol

    /** \brief IPv4 TCP Socket Protocol

        \par Socket Handle typedefs:
            \ref TCPv4ClientSocketHandle (ProtocolClientSocketHandle), \ref TCPv4ServerSocketHandle
            (ProtocolServerSocketHandle)

        \par Policy Interface:
            ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
            ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer()

        \par Address Type:
            INet4SocketAddress

        TCPv4SocketProtocol provides an internet protocol stream socket based on the TCP protocol
        and IPv4 addressing.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle and
        ProtocolServerSocketHandle via the Socket Handle typedefs above.

        \see TCPv6SocketProtocol
     */
    class TCPv4SocketProtocol
        : public ConcreteSocketProtocol<TCPv4Socket_Policy,TCPv4SocketProtocol>,
          public TCPSocketProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        //\{

        void init_client() const;       ///< Create unconnected client socket
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_client(INet4SocketAddress const & address) const;
                                        ///< Create client socket and connect
                                        /**< Creates a new client socket and connects to the given
                                             address.

                                             \param[in] address remote address to connect to */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_server() const;       ///< Create server socket
                                        /**< \note This member is implicitly called from the
                                             ProtocolServerSocketHandle::ProtocolServerSocketHandle()
                                             constructor */
        void init_server(INet4SocketAddress const & address, unsigned backlog=1) const;
                                        ///< Create server socket and listen
                                        /**< Creates a new server socket, binds to \a address end
                                             starts listening for new connections with a backlog of
                                             \a backlog connections. It also enables reuseaddr().

                                             \param[in] address address to listen on
                                             \param[in] backlog size of the listen backlog */
                                        /**< \note This member is implicitly called from the
                                             ProtocolServerSocketHandle::ProtocolServerSocketHandle()
                                             constructor */

        //\}
    };

    typedef ProtocolClientSocketHandle<TCPv4SocketProtocol> TCPv4ClientSocketHandle;
    typedef ProtocolServerSocketHandle<TCPv4SocketProtocol> TCPv4ServerSocketHandle;

    typedef MakeSocketPolicy<
        TCPv4Socket_Policy,
        INet6AddressingPolicy
        >::policy TCPv6Socket_Policy;

    /** \brief IPv6 TCP Socket Protocol

        \par Socket Handle typedefs:
        \ref TCPv6ClientSocketHandle (ProtocolClientSocketHandle), \ref TCPv6ServerSocketHandle
        (ProtocolServerSocketHandle)

        \par Policy Interface:
        ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
        ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer()

        \par Address Type:
            INet6Address

        TCPv6SocketProtocol provides an internet protocol stream socket based on the TCP protocol
        and IPv6 addressing.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle and
        ProtocolServerSocketHandle via the Socket Handle typedefs above.

        \see TCPv4SocketProtocol
     */
    class TCPv6SocketProtocol
        : public ConcreteSocketProtocol<TCPv6Socket_Policy,TCPv6SocketProtocol>,
          public TCPSocketProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        //\{

        void init_client() const;       ///< Create unconnected client socket
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_client(INet6SocketAddress const & address) const;
                                        ///< Create client socket and connect
                                        /**< Creates a new client socket and connects to the given
                                             address.

                                             \param[in] address remote address to connect to */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_server() const;       ///< Create server socket
                                        /**< \note This member is implicitly called from the
                                             ProtocolServerSocketHandle::ProtocolServerSocketHandle()
                                             constructor */
        void init_server(INet6SocketAddress const & address, unsigned backlog=1) const;
                                        ///< Create server socket and listen
                                        /**< Creates a new server socket, binds to \a address end
                                             starts listening for new connections with a backlog of
                                             \a backlog connections. It also enables reuseaddr().

                                             \param[in] address address to listen on
                                             \param[in] backlog size of the listen backlog */
                                        /**< \note This member is implicitly called from the
                                             ProtocolServerSocketHandle::ProtocolServerSocketHandle()
                                             constructor */

        //\}
    };

    typedef ProtocolClientSocketHandle<TCPv6SocketProtocol> TCPv6ClientSocketHandle;
    typedef ProtocolServerSocketHandle<TCPv6SocketProtocol> TCPv6ServerSocketHandle;

    //\}

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "TCPSocketHandle.cci"
//#include "TCPSocketHandle.ct"
//#include "TCPSocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
