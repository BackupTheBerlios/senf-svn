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
    \brief Connectors public header */

/** \defgroup connectors Connector classes

    A connector has two independent properties
    \li it may be \e active or \e passive
    \li it may be an \e input or an \e output
    
    \e Active connectors are activated from within the module, \e passive connectors are signaled by
    the external framework. \e Input modules receive packets, \e output modules send packets.

    All passive connectors call some onRequest callback whenever I/O needs to be performed. All
    input modules possess a packet queue.

    We therefore have 4 connector types: senf::ppi::ActiveInput, senf::ppi::ActiveOutput,
    senf::ppi::PassiveInput and senf::ppi::PassiveOutput.
 */

#ifndef HH_Connectors_
#define HH_Connectors_ 1

// Custom includes
#include <deque>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include "Utils/SafeBool.hh"
#include "Packets/Packets.hh"
#include "predecl.hh"
#include "detail/Callback.hh"
#include "Queueing.hh"

//#include "Connectors.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace connector {

    /** \brief Connector baseclass

        This connector provides access to the generic connector facilities. This includes the
        connection management (access to the connected peer) and the containment management (access
        to the containing module)
     */
    class Connector
        : boost::noncopyable
    {
    public:
        Connector & peer() const;       ///< Get peer connected to this connector
        module::Module & module() const; ///< Get this connectors containing module

    protected:
        Connector();
        virtual ~Connector();

        void connect(Connector & target);

    private:
        void setModule(module::Module & module);

        Connector * peer_;
        module::Module * module_;

        friend class module::Module;
    };

    /** \brief Passive connector baseclass

        A passive connector is a connector which is activated externally whenever an I/O request
        occurs. Passive connectors are the origin of throttling notifications. Depending on the type
        of connector (output or input) the respective throttling is called forward or backward
        throttling.

        Passive connectors always handle two throttling states: 
        
        \li The \e native throttling state is set manually by the module. It is the throttling state
            originating in the current module
        \li The \e forwarded throttling state is the state as it is received by throttling
            notifications

        The accumulative throttling state is generated by combining all sub-states.
     */
    class PassiveConnector 
        : public virtual Connector
    {
    public:
        template <class Handler>
        void onRequest(Handler handler);///< Register I/O event handler
                                        /**< The registered handler will be called, whenever packets
                                             arrive or should be generated by the module depending
                                             on the connector type (input or output). The \a handler
                                             argument is either an arbitrary callable object or it
                                             is a pointer-to-member to a member of the class which
                                             holds this input. In the second case, the pointer will
                                             automatically be bound to the containing instance.
                                             
                                             \param[in] handler Handler to call, whenever an I/O
                                                 operation is to be performed. */

        
        bool throttled() const;         ///< Get accumulative throttling state
        bool nativeThrottled() const;   ///< Get native throttling state

        void throttle();                ///< Set native throttling
        void unthrottle();              ///< Revoke native throttling
        
        ActiveConnector & peer() const;

    protected:
        PassiveConnector();

        void emit();

    private:
        // Called by the routing to change the remote throttling state
        void notifyThrottle();          ///< Forward a throttling notification to this connector
        void notifyUnthrottle();        ///< Forward an unthrottling notification to this connector

        // Internal members to emit throttling notifications
        void emitThrottle();
        void emitUnthrottle();

        // Called after unthrottling the connector
        virtual void v_unthrottleEvent();

        typedef detail::Callback<>::type Callback;
        Callback callback_;

        bool remoteThrottled_;
        bool nativeThrottled_;

        friend class senf::ppi::ForwardingRoute;
    };

    /** \brief Active connector baseclass

        An active connector is a connector which emits I/O requests. Active connectors receive
        throttling notifications. Depending on the type of connector (input or output) the
        respective throttling is called forward or backward throttling.

        Active connectors do not handle any throttling state, they just receive the
        notifications. These notifications should then either be processed by the module or be
        forwarded to other connectors.
     */
    class ActiveConnector 
        : public virtual Connector
    {
        typedef detail::Callback<>::type Callback;
    public:
        template <class Handler>
        void onThrottle(Handler handler); ///< Register throttle notification handler
                                        /**< The handler register here will be called, whenever a
                                             throttle notification comes in. The \a handler argument
                                             is either an arbitrary callable object or it is a
                                             pointer-to-member to a member of the class which holds
                                             this input. In the second case, the pointer will
                                             automatically be bound to the containing instance.

                                             \param[in] handler Handler to call on throttle
                                                 notifications. */
        void onThrottle();

        template <class Handler>
        void onUnthrottle(Handler handler); ///< Register unthrottle notification handler
                                        /**< The handler register here will be called, whenever an
                                             unthrottle notification comes in. The \a handler
                                             argument is either an arbitrary callable object or it
                                             is a pointer-to-member to a member of the class which
                                             holds this input. In the second case, the pointer will
                                             automatically be bound to the containing instance.

                                             \param[in] handle Handler to call on unthrottle
                                                 notifications. */
        void onUnthrottle();

        PassiveConnector & peer() const;

    protected:
        ActiveConnector();

    private:
        // called by the peer() to forward throttling notifications
        void notifyThrottle();
        void notifyUnthrottle();

        // called by ForwardingRoute to register a new route
        void registerRoute(ForwardingRoute & route);

        Callback throttleCallback_;
        Callback unthrottleCallback_;

        typedef std::vector<ForwardingRoute*> NotifyRoutes;
        NotifyRoutes notifyRoutes_;

        friend class senf::ppi::ForwardingRoute;
        friend class PassiveConnector;
    };

    /** \brief Input connector baseclass

        An input connector receives packets. It may be either an ActiveConnector or a
        PassiveConnector. An input connector contains a packet queue. This queue enables processing
        packets in batches or generating multiple output packets from a single input packet. The
        queues have the potential to greatly simplify the module implementations.

        \implementation Which container to use?
            \li list has good insertion and deletion properties on both ends but it costs a dynamic
                memory allocation for every insertion. A very good property is, that iterators stay
                valid across insertions/deletions
            \li vector is fast and has good amortized dynamic allocation properties. However, it is
                quite unusable as a queue
            \li deque has comparable dynamic allocation properties as vector but also has good
                insertion/removal properties on both ends.

            So probably we will use a deque. I'd like a container which keeps iterators intact on
            isertion/deletion but I believe that list is just to expensive since every packet will
            be added to the queue before it can be processed.
     */
    class InputConnector 
        : public virtual Connector
    {
        typedef std::deque<Packet> Queue;
    public:
        typedef Queue::const_iterator queue_iterator; ///< Iterator type of the embedded queue
        typedef Queue::size_type size_type; ///< Unsigned type for counting queue elements

        Packet operator()();            ///< Get a packet
                                        /**< This member is the primary method to access received
                                             data. On passive connectors, this operator will just
                                             dequeue a packet from the packet queue. If the
                                             connector is active, the connector will request new
                                             packets from the connected module. If the packet
                                             request cannot be fulfilled, this is considered to be a
                                             logic error in the module implementation and an
                                             exception is raised. */

        OutputConnector & peer() const;

        queue_iterator begin() const;   ///< Access queue begin (head)
        queue_iterator end() const;     ///< Access queue past-the-end (tail)
        Packet peek() const;            ///< Return head element from the queue

        size_type queueSize() const;    ///< Return number of elements in the queue
        bool empty() const;             ///< Return queueSize() == 0

    protected:
        InputConnector();
        
    private:
        void enqueue(Packet p);
        
        virtual void v_requestEvent();
        virtual void v_enqueueEvent();
        virtual void v_dequeueEvent();

        Queue queue_;

        friend class OutputConnector;
    };
    
    /** \brief Output connector baseclass
        
        An output connector sends out packets. It may be either an ActiveConnector or a
        PassiveConnector. An output connector does \e not have an built-in queueing, it relies on
        the queueing of the connected input.
     */
    class OutputConnector 
        : public virtual Connector
    {
    public:
        void operator()(Packet p);        ///< Send out a packet

        InputConnector & peer() const;

    protected:
        OutputConnector();
    };
    
    ///@{
    ///\addtogroup connectors

    /** \brief Combination of PassiveConnector and InputConnector

        The PassiveInput automatically controls the connectors throttling state using a queueing
        discipline. The standard queueing discipline is ThresholdQueueing, which throttles the
        connection whenever the queue length reaches the high threshold and unthrottles the
        connection when the queue reaches the low threshold. The default queueing discipline is
        <tt>ThresholdQueueing(1,0)</tt> which will throttle the input whenever the queue is
        non-empty.
     */
    class PassiveInput 
        : public PassiveConnector, public InputConnector,
          public SafeBool<PassiveInput>
    {
    public:
        PassiveInput();

        ActiveOutput & peer() const;

        bool boolean_test() const;

        template <class QDisc>
        void qdisc(QDisc const & disc); ///< Change the queueing discipline
                                        /**< The queueing discipline is a class which provides the
                                             QueueingDiscipline interface.
                                             
                                             \param[in] disc New queueing discipline */

    private:
        void v_enqueueEvent();
        void v_dequeueEvent();
        void v_unthrottleEvent();

        boost::scoped_ptr<QueueingDiscipline> qdisc_;
    };

    /** \brief Combination of PassiveConnector and OutputConnector
     */
    class PassiveOutput
        : public PassiveConnector, public OutputConnector,
          public SafeBool<PassiveOutput>
    {
    public:
        ActiveInput & peer() const;

        bool boolean_test() const;

        void connect(ActiveInput & target);

        friend class ActiveInput;
    };

    /** \brief Combination of ActiveConnector and InputConnector
     */
    class ActiveInput
        : public ActiveConnector, public InputConnector,
          public SafeBool<ActiveInput>
    {
    public:
        PassiveOutput & peer() const;

        bool boolean_test() const;

        void request();                 ///< request more packets without dequeuing any packet

    private:
        void v_requestEvent();
    };

    /** \brief Combination of ActiveConnector and OutputConnector
     */
    class ActiveOutput
        : public ActiveConnector, public OutputConnector,
          public SafeBool<ActiveOutput>
    {
    public:
        PassiveInput & peer() const;

        bool boolean_test() const;

        void connect(PassiveInput & target);
    };

    ///@}

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Connectors.cci"
//#include "Connectors.ct"
#include "Connectors.cti"
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
