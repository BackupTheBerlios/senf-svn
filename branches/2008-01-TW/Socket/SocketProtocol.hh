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
    \brief SocketProtocol and ConcreteSocketProtocol public header

    \idea We should optimize the protocol handling. Allocating a protocol instance for every socket
        body seems quite wasteful. We could derive SocketPolicy from SocketBody (probably privately,
        since private inheritance models more of 'has a' than 'is a'). This would allow to reduce
        the number of heap-allocations per socket to one which is good.
 */

// The private inheritance idea should indeed work very well: We just need to change the
// implementations of body() and protocol() and that of the ProtocolClient/ServerSocketHandle
// constructors and the SocketBody constructor. The body and the protocol would still be visible
// like several instances because of the private inheritance but we would save the backwards
// pointer.

/** \defgroup protocol_group The Protocol Classes

    \htmlonly
    <map name="protocols">
    <area shape="rect" alt="SocketPolicy" href="structsenf_1_1SocketPolicy.html" title="SocketPolicy" coords="416,50,536,68" />
    <area shape="rect" alt="ConcreteSocketProtocol" href="classsenf_1_1ConcreteSocketProtocol.html" title="ConcreteSocketProtocol" coords="268,65,456,88" />
    <area shape="rect" alt="SocketProtocol" href="classsenf_1_1SocketProtocol.html" title="SocketProtocol" coords="1,2,120,26" />
    <area shape="rect" alt="BSDSocketProtocol" href="classsenf_1_1BSDSocketProtocol.html" title="BSDSocketProtocol" coords="124,118,276,143" />
    <area shape="rect" alt="AddressableBSDSocketProtocol" href="classsenf_1_1AddressableBSDSocketProtocol.html" title="AddressableBSDSocketProtocol" coords="82,200,314,224" />
    <area shape="rect" alt="IPv4Protocol" href="classsenf_1_1IPv4Protocol.html" title="IPv4Protocol" coords="149,272,252,296" />
    <area shape="rect" alt="IPv6Protocol" href="classsenf_1_1IPv6Protocol.html" title="IPv6Protocol" coords="149,335,251,359" />
    <area shape="rect" alt="TCPProtocol" href="classsenf_1_1TCPProtocol.html" title="TCPProtocol" coords="151,398,248,420" />
    <area shape="rect" alt="TCPv4SocketProtocol" href="classsenf_1_1TCPv4SocketProtocol.html" title="TCPv4SocketProtocol" coords="288,471,405,494" />
    <area shape="rect" alt="TCPv6SocketProtocol" href="classsenf_1_1TCPv6SocketProtocol.html" title="TCPv6SocketProtocol" coords="424,470,540,494" />
    <area shape="rect" alt="PacketProtocol" href="classsenf_1_1PacketProtocol.html" title="PacketProtocol" coords="560,469,680,495" />
    </map>
    <img src="Protocols.png" border="0" alt="Protocols" usemap="#protocols">
    \endhtmlonly

    The socket handle classes and templates only implement the most important socket API methods
    using the policy framework. To access the complete API, the protocol interface is
    provided. Access to the protocol interface is only possible via senf::ProtocolClientSocketHandle
    and senf::ProtocolServerSocketHandle which have the necessary \c protocol() member. This member
    returns a reference to the protocol class instance which contains members covering all the API
    functions (mostly setsockopt/getsockopt related calls but there may be more, this is completely
    up to the implementor of the protocol class) not found in the SocketHandle interface. The
    protocol interface is specific to the protocol. It's implementation is quite free. The standard
    protocols are implemented using a simple multiple-inheritance hierarchy as shown above.

    Since the protocol class is protocol specific (how intelligent ...), the protocol class also
    defines the \e complete socket policy to be used with it's protocol. Complete meaning, that
    every policy axis must be assigned it's the most specific (that is derived) policy class to be
    used with the protocol and that no policy axis is allowed to be left unspecified.

    \see
        \ref handle_group \n
        \ref policy_group

    \todo Complete the protocol interface implementations. Better distribution of members to
        protocol facets and more precise distribution of functionality among the facets.
 */

/** \defgroup concrete_protocol_group Protocol Implementations (Concrete Protocol Classes)
    \ingroup protocol_group

    Theese protocol classes define concrete and complete protocol implementations. They inherit from
    ConcreteSocketProtocol and are used with the ProtocolClientSocketHandle and
    ProtocolServerSocketHandle templates to instantiate socket handles. Appropriate typedefs are
    always provided.

    Every protocol defines both the protocol and the policy interface provided by that protocol. See
    the documentation of the protocol classes listed below for more information on the supported
    protocols. Every protocol class documents it's policy interface. Use the 'list all members' link
    of the protocol class to find the complete policy interface.
 */

/** \defgroup protocol_facets_group Protocol Facets
    \ingroup protocol_group

    The protocol facets are classes used as building blocks to build concrete protocol classes. Each
    protocol facet will implement some functional part of the protocol interface. The protocol
    facets all inherit from SocketProtocol by public \e virtual inheritance. This ensures the
    accessibility of the socket body from all facets.
 */

#ifndef HH_SocketProtocol_
#define HH_SocketProtocol_ 1

// Custom includes
#include <boost/utility.hpp>
// Hrmpf ... I have tried very hard, but I just can't find a nice, generic way to clean
// up this include
#include "SocketHandle.ih"

//#include "SocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_group
    /// @{

    class SocketPolicyBase;

    /** \brief Socket Protocol base class

        This is the base class of all socket protocol classes. Every protocol class must directly or
        indirectly inherit from SocketProtocol

        \attention SocketProtocol must \e always be inherited using public \e virtual inheritance.
     */
    class SocketProtocol 
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        SocketProtocol();
        virtual ~SocketProtocol() = 0;

        // default default constructor
        // no copy
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        virtual SocketPolicyBase const & policy() const = 0;
        ///< Access the policy instance

        ///////////////////////////////////////////////////////////////////////////
        // Virtual interface

        virtual std::auto_ptr<SocketProtocol> clone() const = 0;
                                        ///< Polymorphically return a copy of this protocol class
                                        /**< This member will create a new copy of the protocol
                                             class on the heap.
                                             \attention This member must be implemented in every \e
                                                 leaf protocol class to return a new instance of the
                                                 appropriate type. */

        virtual unsigned available() const = 0;
                                        ///< Return number of bytes available for reading without
                                        ///< blocking
                                        /**< This member will check in a (very, sigh) protocol
                                             dependent way, how many bytes may be read from a socket
                                             in a single (non-blocking) read operation. If the
                                             socket does not support reading (viz. NotReadablePolicy
                                             is set), this member should always return \c 0.
                                             
                                             Depending on the protocol, it may not be possible to
                                             return a good value. In this case, an upper bound may
                                             be returned (e.g.: When reading from a socket which
                                             returns ethernet frames, returning 1500 from
                                             available() is ok). However, this should only be done
                                             as a last resort. Also beware, that this number should
                                             not be to large since the socket layer will always need
                                             to allocate that number of bytes for the data to be
                                             read. */

        virtual bool eof() const = 0;   ///< Check for end-of-file condition
                                        /**< This is another check which (like available()) is
                                             extremely protocol dependent. This member will return
                                             \c true only, if at end-of-file. If the protocol does
                                             not support the notion of EOF, this member should
                                             always return \c false. */

        virtual void close() const;     ///< Close socket
                                        /**< This override will automatically \c shutdown() the
                                             socket whenever it is closed.
                                             \throws senf::SystemException */
        
        virtual void terminate() const; ///< Forcibly close socket
                                        /**< This override will automatically \c shutdown() the
                                             socket whenever it is called. Additionally it will
                                             disable SO_LINGER to ensure, that v_terminate will not
                                             block. Like the overriden method, this member will ignore
                                             failures and will never throw. It is therefore safe to be
                                             called from a destructor. */

        virtual void state(SocketStateMap & map, unsigned lod) const;
                                        ///< Return socket state information
                                        /**< This member is called to add state information to the
                                             status \a map. The protocol map should provide as
                                             detailed information as possible. The amount of
                                             information to be added to the map is selected by the
                                             \a lod value with a default value of 0. The
                                             interpretation of the \a lod value is completely
                                             implementation defined.
                                             
                                             Every class derived from SocketProtocol should
                                             reimplement state(). The reimplemented method should
                                             call (all) baseclass-implementations of this
                                             member.

                                             The \a map Argument is a map which associates
                                             std:string keys with std:string-like values. The map
                                             keys are interpreted as hierarchical strings with '.'
                                             as a separator (like hostnames or struct or class
                                             members). They are automatically sorted correctly.
                                             
                                             The values are std:string with one additional feature:
                                             they allow assignment or conversion from *any* type as
                                             long as that type is streamable. This simplifies
                                             assigning non-string values to the map:
                                             
                                             \code
                                                 map["socket.protocol.ip.address"] << peer();
                                                 map["socket.protocol.tcp.backlog"] << backlog();
                                             \endcode
                                             
                                             This will work even if peer() returns an ip-address
                                             object or backlog() returns an integer. The values are
                                             automatically converted to their string representation.
                                             
                                             Additionally, if the slot the date is written to is not
                                             empty, the <tt>\<\<</tt> operator will add add a comma
                                             as separator. */

    protected:
        FileHandle fh() const;          ///< Get a FileHandle for this instance
                                        /**< This member will re turn a FileHandle instance for this
                                             protocol instance. You may cast this FileHandle
                                             instance to a ClientSocketHandle / ServerSocketHandle
                                             as long as you know some of the socket policy using
                                             static_socket_cast or dynamic_socket_cast */

        int fd() const;                 ///< Get file descriptor
                                        /**< Returns the file descriptor this protocol instance
                                             references. This is the same as <tt>fh().fd()</tt> but
                                             is implemented here since it is needed so often. */

        void fd(int) const;             ///< Initialize file descriptor
                                        /**< Assigns the file descriptor to the file handle, this
                                             protocol instance references. Only valid, if the file
                                             handle has not yet been assigned any descriptor (To
                                             change the file descriptor association later, use \c
                                             ::dup2()). */

    private:
        // backpointer to owning SocketBody instance
        
        SocketBody & body() const;

        SocketBody * body_;
        friend class SocketBody;
    };
    
    template <class Policy> class ClientSocketHandle;
    template <class Policy> class ServerSocketHandle;

    /** \brief Concrete Socket Protocol implementation base class

        ConcreteSocketProtocol is the base class of a concrete socket protocol implementation. The
        final protocol class must inherit from ConcreteSocketProtocol. The template argument \a
        SocketPolicy must be set to the complete socket policy of the protocol.

        A protocol implementation may define the protocol interface directly. It can also
        (additionally) make use of multiple inheritance to combine a set of protocol facets into a
        specific protocol implementation (i.e. TCPv4SocketProtocol inherits from
        ConcreteSocketProtocol and from the protocol facets IPv4Protocol, TCPProtocol,
        BSDSocketProtocol and AddressableBSDSocketProtocol). The protocol facets are not concrete
        protocols themselves, they are combined to build concrete protocols. This structure will
        remove a lot of code duplication. It is important to ensure, that the protocol facets do not
        overlap, since otherwise there will be problems resolving overlapping members.
        
        \doc init_client init_server
     */
    template <class SocketPolicy>
    class ConcreteSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef SocketPolicy Policy;    ///< The protocols policy

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ~ConcreteSocketProtocol() = 0;

        // no default constructor
        // no copy
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Policy const & policy() const;

    protected:
        ClientSocketHandle<Policy> clientHandle() const; 
                                        ///< Get client handle for associated socket
                                        /**< Returns a client handle for the socket associated with
                                             this protocol instance */
        ServerSocketHandle<Policy> serverHandle() const;
                                        ///< Get server handle for associated socket
                                        /**< Returns a server handle for the socket associated with
                                             this protocol instance */

    private:
        Policy policy_;

    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
#include "SocketProtocol.cci"
//#include "SocketProtocol.ct"
#include "SocketProtocol.cti"
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