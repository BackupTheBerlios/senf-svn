// $Id: ConnectedUDPSocketHandle.hh 357 2007-07-26 22:48:39Z g0dil $
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

/** \file
    \brief UDPv4SocketHandle and UDPv6SocketHandle public header

    \todo Implement possibly non-blocking connect and SO_ERROR in the
    protocol interface
 */

#ifndef HH_ConnectedUDPSocketHandle_
#define HH_ConnectedUDPSocketHandle_ 1

// Custom includes
#include "INetProtocol.hh"
#include "UDPProtocol.hh"
#include "../../../Socket/Protocols/BSDSocketProtocol.hh"
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/BufferingPolicy.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"

//#include "ConnectedUDPSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
        INet4AddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy,
        SocketBufferingPolicy
        >::policy ConnectedUDPv4Socket_Policy;   ///< Socket Policy of the UDPv4 Protocol

    /** \brief IPv4 UDP Socket Protocol, connected

        \par Socket Handle typedefs:
            \ref ConnectedUDPv4ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
            ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
            ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer(),
            ClientSocketHandle::rcvbuf(), ClientSocketHandle::sndbuf()

        \par Address Type:
            INet4Address
        
        ConnectedUDPv4SocketProtocol provides an internet protocol stream socket based on the UDP
        protocol and IPv4 addressing.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \see ConnectedUDPv6SocketProtocol
     */
    class ConnectedUDPv4SocketProtocol
        : public ConcreteSocketProtocol<ConnectedUDPv4Socket_Policy>,
          public IPv4Protocol,
          public UDPProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol,
          public senf::pool_alloc_mixin<ConnectedUDPv4SocketProtocol>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

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

        ///@}
        ///\name Abstract Interface Implementation

        std::auto_ptr<SocketProtocol> clone() const;

        ///@}
    };

    typedef ProtocolClientSocketHandle<
        ConnectedUDPv4SocketProtocol> ConnectedUDPv4ClientSocketHandle;

    typedef MakeSocketPolicy<
        ConnectedUDPv4Socket_Policy,
        INet6AddressingPolicy
        >::policy  ConnectedUDPv6Socket_Policy;

    /** \brief IPv6 UDP Socket Protocol, connected

        \par Socket Handle typedefs:
            \ref ConnectedUDPv6ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
            ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
            ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer(),
            ClientSocketHandle::rcvbuf(), ClientSocketHandle::sndbuf()

        \par Address Type:
            INet6Address

        ConnectedUDPv6SocketProtocol provides an internet protocol stream socket based on the UDP
        protocol and IPv6 addressing.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \see ConnectedUDPv4SocketProtocol
     */
    class ConnectedUDPv6SocketProtocol
        : public ConcreteSocketProtocol<ConnectedUDPv6Socket_Policy>,
          public IPv6Protocol,
          public UDPProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol,
          public senf::pool_alloc_mixin<ConnectedUDPv6SocketProtocol>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

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

        ///@}
        ///\name Abstract Interface Implementation

        std::auto_ptr<SocketProtocol> clone() const;

        ///@}
    };

    typedef ProtocolClientSocketHandle<
        ConnectedUDPv6SocketProtocol> ConnectedUDPv6ClientSocketHandle;

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ConnectedUDPSocketHandle.cci"
//#include "ConnectedUDPSocketHandle.ct"
//#include "ConnectedUDPSocketHandle.cti"
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
