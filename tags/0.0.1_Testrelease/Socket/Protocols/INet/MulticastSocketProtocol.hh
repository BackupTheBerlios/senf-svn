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
    \brief MulticastSocketProtocol public header */

#ifndef HH_MulticastSocketProtocol_
#define HH_MulticastSocketProtocol_ 1

// Custom includes
#include "../../../Socket/SocketProtocol.hh"
#include "INet4Address.hh"
#include "INet6Address.hh"

//#include "MulticastSocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    ///\addtogroup protocol_facets_group
    ///\{
   
    /** \brief Generic addressing type independent multicast protocol facet
     */
    class MulticastSocketProtocol 
        : public virtual SocketProtocol
    {
    public:
        void broadcastEnabled(bool v) const; ///< Enable broadcast send/receive
                                        /**< If this option is enabled, broadcast UDP messages will
                                             be received on the socket and the socket will be
                                             allowed to send out broadcast UDP messages
                                             \param[in] v \c true to enable broadcast send/receive,
                                                 \c false to disable */

        bool broadcastEnabled() const;  ///< Get broadcast send/receive state
                                        /**< \returns Current state of the broadcastEnabled()
                                             option. */


        unsigned mcTTL() const;         ///< Return current multicast TTL
        void mcTTL(unsigned value) const; ///< Set multicast TTL

        bool mcLoop() const;            ///< Return current multicast loopback state. 
        void mcLoop(bool value) const;  ///< Set multicast loopback state
        /**< If set to false via \c mcLoop(value) multicast messages will not be looped back  to local sockets. Default value is \c true (1).   */

        void mcIface(std::string const & iface = std::string()) const;
                                        ///< Set multicast send interface of the socket
                                        /**< \param[in] iface name of interface to send multicast
                                             data from */
    };

    /** \brief Multicast protocol facet for INet4 addressable multicast enabled sockets
     */
    class INet4MulticastSocketProtocol
        : public MulticastSocketProtocol
    {
    public:
        void mcAddMembership(INet4Address const & mcAddr) const;
                                        ///< Join multicast group on default interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the default
                                             interface.
                                             \param[in] mcAddr address of group to join */
        void mcAddMembership(INet4Address const & mcAddr, INet4Address const & localAddr) 
            const;
                                        ///< join multicast group on a specific interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the interface
                                             with the given local address.
                                             \deprecated Use \link mcAddMembership(INet4Address const & mcAddr, std::string const & iface) const mcAddMembership( \a mcAddr , \a iface )\endlink 
                                                 for compatibility with the IPv6 API.
                                             \param[in] mcAddr address of group to join
                                             \param[in] localAddr address of interface to join on */
        void mcAddMembership(INet4Address const & mcAddr, std::string const & iface) const;
                                        ///< join multicast group on a specific interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the given
                                             interface.
                                             \param[in] mcAddr address of group to join
                                             \param[in] iface interface name */

        void mcDropMembership(INet4Address const & mcAddr) const;
                                        ///< Leave multicast group
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             default interface.
                                             \param[in] mcAddr address of group to leave */
        void mcDropMembership(INet4Address const & mcAddr, INet4Address const & localAddr) 
            const;
                                        ///< leave multicast group on a specific interface
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             interface with the given local address.
                                             \deprecated Use \link mcDropMembership(INet4Address const & mcAddr, std::string const & iface) const mcDropMembership( \a mcAddr , \a iface )\endlink 
                                                 for compatibility with the IPv6 API.
                                             \param[in] mcAddr address of group to leave
                                             \param[in] localAddr address of interface to leave
                                                 from */
        void mcDropMembership(INet4Address const & mcAddr, std::string const & iface) 
            const;
                                        ///< leave multicast group on a specific interface
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             interface with the given local address.
                                             \param[in] mcAddr address of group to leave
                                             \param[in] iface interface name */
    };

    /** \brief Multicast protocol facet for INet6 addressable multicast enabled sockets
     */
    class INet6MulticastSocketProtocol
        : public MulticastSocketProtocol
    {
    public:
        void mcAddMembership(INet6Address const & mcAddr) const;
                                        ///< Join multicast group on default interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the default
                                             interface.
                                             \param[in] mcAddr address of group to join */
        void mcAddMembership(INet6Address const & mcAddr, INet6Address const & localAddr) 
            const;
                                        ///< join multicast group on a specific interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the interface
                                             with the given local address.
                                             \param[in] mcAddr address of group to join
                                             \param[in] localAddr address of interface to join on */
        void mcAddMembership(INet6Address const & mcAddr, std::string const & iface);
                                        ///< join multicast group on a specific interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the given
                                             interface.
                                             \param[in] mcAddr address of group to join
                                             \param[in] iface interface name */

        void mcDropMembership(INet6Address const & mcAddr) const;
                                        ///< Leave multicast group
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             default interface.
                                             \param[in] mcAddr address of group to leave */
        void mcDropMembership(INet6Address const & mcAddr, INet6Address const & localAddr) 
            const;
                                        ///< leave multicast group on a specific interface
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             interface with the given local address.
                                             \param[in] mcAddr address of group to leave
                                             \param[in] localAddr address of interface to leave
                                                 from */
        void mcDropMembership(INet6Address const & mcAddr, std::string const & iface) 
            const;
                                        ///< leave multicast group on a specific interface
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             interface with the given local address.
                                             \param[in] mcAddr address of group to leave
                                             \param[in] iface interface name */
    };

    ///\}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "MulticastSocketProtocol.cci"
//#include "MulticastSocketProtocol.ct"
//#include "MulticastSocketProtocol.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
