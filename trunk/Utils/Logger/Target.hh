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
    \brief Target public header */

#ifndef HH_Target_
#define HH_Target_ 1

// Custom includes
#include <set>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include "../singleton.hh"
#include "../mpl.hh"
#include "StreamRegistry.hh"
#include "AreaRegistry.hh"
#include "../Exception.hh"

//#include "Target.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \defgroup targets Targets

    Targets receive log messages and write them to some destination: The console, a log file, an SQL
    DB and so on. Every target is derived from the \ref senf::log::Target base class. This base
    class provides the target with the necessary routing infrastructure. The different targets only
    differ in the way, they write the data.

    \see senf::log::Target
 */

namespace senf {
namespace log {
    
    namespace detail { class TargetRegistry; }

    /** \brief Logging target base class
        
        Targets are the final destination of %log messages. Every message is eventually routed to one
        or several targets.

        \section target_routing Routing

        Each target manages a routing table. The message meta-data (stream, area and level) is
        matched against this table. If an entry matches, the action associated with this entry is
        taken (either \c ACCEPT or \c REJECT).

        Every target manages it's own routing table. Conceptually, every routing message will be
        routed to every target where it will then be matched against each targets routing table (the
        implementation is more efficient and utilizes a routing cache).

        Each routing entry consists of the following parameters
        \li (mandatory) \e stream. The entry will match only messages directed at that stream
        \li (optional) \e area. If the area is specified, only messages directed at that area are
            matched, otherwise any area will be allowed
        \li (optional) \e level. If the log level is specified, messages will be accepted if their
            level is at least that value. If the value is not specified, the limit will be taken
            from the stream's default value.

        Each parameter (stream, area and level) has two representations: A static (compile time
        constant) representation, which is the representation also used in the log statements, and a
        dynamic representation, which may be used for manipulating the routing table.

        The static representation is used, when passing routing parameters via template arguments:
        \code
        target.route<foo::SomeStream, senf::log::NOTICE>(senf::log::Target::REJECT);
        target.route<foo::SomeStream>();
        \endcode
        The identical routing statements may be expressed using dynamic routing via:
        \code
        target.route("foo::SomeStream", "", senf::log::NOTICE::value, senf::log::Target::REJECT);
        target.route("foo::SomeStream");
        \endcode
        The static representation has the benefit of being compile-time type checked: Invalid
        routing parameters will be caught while compiling the code. The dynamic representation is
        more flexible as it allows to adjust routing from user input (e.g. configuration files).

        The different object representations are:
        \li The \e streams is statically represented by it's name, which is the name of a class
            defined with \ref SENF_LOG_DEFINE_STREAM. The dynamic representation is a string
            representation of this name.
        \li The \e area is statically represented by it's name, which again is the name of a class
            defined with \ref SENF_LOG_DEFINE_STREAM. The dynamic representation again is a string
            representation of this class's name. The dynamic representation represents an absent
            area with the empty string.
        \li The \e level is statically represented by a level class from \ref
            loglevels. Dynamically, it is represented by an unsigned integer number, the \c value
            member of that class.

        \section target_impl Implementing new targets

        To implement a new target type, you need to derive from senf::log::Target and implement the
        single \c v_write member. This member will be called whenever a message should be output. 

        The target may process in any arbitrary way: reformat, writing it into an SQL DB, whatever
        can be envisioned. However, there is one important limitation: The \c v_write call must not
        block. So for more complex scenarios, additional measures must be taken (e.g. writing a %log
        backend daemon which receives the messages via UDP and processes them). Of course, in rare
        cases messages might be lost but this cannot be avoided.

        \see \ref targets
      */
    class Target : private boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        /** \brief Routing action

            Every routing entry is associated with a routing action. This action is final (for this
            target. Each target is processed independently).
         */
        enum action_t { 
            ACCEPT /** Output message */
          , REJECT /** Suppress message output */
        };

        /** \brief Target routing entry

            A single routing entry matches messages against their \e stream, \e area and \e
            level. If the entry matches, the given \e action is performed.

            \see senf::log::Target
         */
        struct RoutingEntry 
        {
            std::string stream() const; ///< Stream to match
            std::string area() const;   ///< Area to match (empty of unspecified)
            unsigned level() const;     ///< Level to match (senf::log::NONE::value if unspecified)
            action_t action() const;    ///< Action to take
            
#           ifdef DOXYGEN
        private:
#           endif

            RoutingEntry();
            bool operator==(RoutingEntry const & other);

        private:
            RoutingEntry(detail::StreamBase const * stream, detail::AreaBase const * area, 
                         unsigned level, action_t action);

            detail::StreamBase const * stream_;
            detail::AreaBase const * area_;
            unsigned level_;
            action_t action_;
            
            friend class Target;
        };

    private:
        typedef std::vector<RoutingEntry> RIB;

    public:
        typedef RIB::const_iterator iterator; ///< Routing table iterator

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        Target();
        virtual ~Target();

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Routing
        ///\{

#       ifdef DOXYGEN

        template <class Stream, class Area, class Level> void route(
            action_t action = ACCEPT, int index = -1); ///< Add route (static)
                                        /**< Add a route for the given combination of \a Stream, \a
                                             Area and \a Level. The \a Stream parameter is mandatory
                                             while either \a Area or \a Level are optional (the
                                             template signature is shown simplified here):
                                             \code
                                             target.route<SomeStream>();
                                             target.route<SomeStream, SomeLevel>();
                                             target.route<SomeStream, SomeArea>();
                                             target.route<SomeStream, SomeArea, SomeLevel>();
                                             \endcode

                                             See the class description for information on the \a
                                             action and \a index parameters 

                                             \tparam Stream mandatory stream to match
                                             \tparam Area optional area to match
                                             \tparam Level optional level, matches messages with
                                                 at least the given level. 
                                             \param[in] action routing action to take
                                             \param[in] index position of new route in the routing
                                                 table */

#       endif

        void route(std::string const & stream, std::string const & area = "", 
                   unsigned level = NONE::value, action_t action = ACCEPT, int index = -1);
                                        ///< Add route (dynamic)
                                        /**< Add a route for the given combination of \a stream, \a
                                             area and \a level. The \a stream parameter is mandatory
                                             while either \a area or \a level may be left
                                             unspecified by setting them to the empty string or
                                             senf::log::NONE::value respectively.

                                             See the class description for information on the \a
                                             action and \a index parameters 

                                             \throws InvalidStreamException if the given \a stream
                                                 is not found in the StreamRegistry
                                             \throws InvalidAreaException if the given \a area is
                                                 not found in the AreaRegistry

                                             \param[in] stream mandatory stream to match
                                             \param[in] area optional area to match
                                             \param[in] level optional level, matches messages with
                                                 at least the given level.
                                             \param[in] action routing action to take
                                             \param[in] index position of new route in the routing
                                                 table */

#       ifdef DOXYGEN

        template <class Stream, class Area, class Level> 
        void unroute(action_t action = ACCEPT);
                                        ///< Remove route (static)
                                        /**< This member removes an arbitrary routing entry. The
                                             template parameters are the same as for the
                                             corresponding \ref route() call.

                                             The routing table is searched for a route exactly
                                             matching the given specification. If such a route is
                                             found, it will be removed, otherwise the call will be
                                             ignored

                                             \tparam Stream mandatory stream to match
                                             \tparam Area optional area to match
                                             \tparam Level optional level, matches messages with
                                                 at least the given level. 
                                             \param[in] action routing action to take */

#       endif

        void unroute(std::string const & stream, std::string const & area = "", 
                     unsigned level = NONE::value, action_t action = ACCEPT);
                                        ///< Remove route (dynamic)
                                        /**< This member removes an arbitrary routing entry. The \a
                                             stream parameter is mandatory while either \a area or
                                             \a level may be left unspecified by setting them to the
                                             empty string or senf::log::NONE::value respectively.

                                             The routing table is searched for a route exactly
                                             matching the given specification. If such a route is
                                             found, it will be removed, otherwise the call will be
                                             ignored

                                             \param[in] stream mandatory stream to match
                                             \param[in] area optional area to match
                                             \param[in] level optional level, matches messages with
                                                 at least the given level.
                                             \param[in] action routing action to take */
        void unroute(int index=-1);     ///< Remove route (indexed)
                                        /**< This call will remove the route with the given index.
                                             
                                             See the class documentation for more information on
                                             indexing. 

                                             \param[in] index index of routing entry to remove */

#       ifndef DOXYGEN

        template <class Stream> void route(
            action_t action = ACCEPT, int index = -1);
        template <class Stream, class Level> void route(
            action_t action = ACCEPT, int index = -1,
            typename boost::enable_if< boost::is_convertible<Level*,
                                                             detail::LevelBase *> >::type * = 0);
        template <class Stream, class Area> void route(
            action_t action = ACCEPT, int index = -1,
            typename boost::enable_if< boost::is_convertible<Area*,
                                                             detail::AreaBase *> >::type * = 0);
        template <class Stream, class AreaClass> void route(
            action_t action = ACCEPT, int index = -1,
            typename boost::enable_if< boost::is_convertible<typename AreaClass::SENFLogArea *,
                                                             detail::AreaBase *> >::type * = 0);
        template <class Stream, class Area, class Level> void route(
            action_t action = ACCEPT, int index = -1,
            typename boost::enable_if< boost::is_convertible<Area *,
                                                             detail::AreaBase *> >::type * = 0);
        template <class Stream, class AreaClass, class Level> void route(
            action_t action = ACCEPT, int index = -1,
            typename boost::enable_if< boost::is_convertible<typename AreaClass::SENFLogArea *,
                                                             detail::AreaBase *> >::type * = 0);

        template <class Stream> void unroute(
            action_t action = ACCEPT);
        template <class Stream, class Level> void unroute(
            action_t action = ACCEPT,
            typename boost::enable_if< boost::is_convertible<Level*,
                                                             detail::LevelBase *> >::type * = 0);
        template <class Stream, class Area> void unroute(
            action_t action = ACCEPT,
            typename boost::enable_if< boost::is_convertible<Area*,
                                                             detail::AreaBase *> >::type * = 0);
        template <class Stream, class AreaClass> void unroute(
            action_t action = ACCEPT,
            typename boost::enable_if< boost::is_convertible<typename AreaClass::SENFLogArea *,
                                                             detail::AreaBase *> >::type * = 0);
        template <class Stream, class Area, class Level> void unroute(
            action_t action = ACCEPT,
            typename boost::enable_if< boost::is_convertible<Area*,
                                                             detail::AreaBase *> >::type * = 0);
        template <class Stream, class AreaClass, class Level> void unroute(
            action_t action = ACCEPT,
            typename boost::enable_if< boost::is_convertible<typename AreaClass::SENFLogArea *,
                                                             detail::AreaBase *> >::type * = 0);

#       endif

        ///\}

        /** \brief Exception: Invalid stream */
        struct InvalidStreamException : public senf::Exception
        { InvalidStreamException() 
              : senf::Exception("senf::log::Target::InvalidStreamException"){} };
        
        /** \brief Exception: Invalid area */
        struct InvalidAreaException : public senf::Exception
        { InvalidAreaException() 
              : senf::Exception("senf::log::Target::InvalidAreaException"){} };

        iterator begin() const;         ///< Iterator to beginning of routing table
        iterator end() const;           ///< Iterator past the end of routing table
        
    private:
        void route(detail::StreamBase const * stream, detail::AreaBase const * area, 
                   unsigned level, action_t action, int index);
        void unroute(detail::StreamBase const * stream, detail::AreaBase const * area, 
                     unsigned level, action_t action);

        void updateRoutingCache(detail::StreamBase const * stream, detail::AreaBase const * area);

        void write(boost::posix_time::ptime timestamp, detail::StreamBase const & stream,
                   detail::AreaBase const & area, unsigned level, std::string const & message);

#   ifdef DOXYGEN
    protected:
#   endif

        virtual void v_write(boost::posix_time::ptime timestamp, std::string const & stream, 
                             std::string const & area, unsigned level, 
                             std::string const & message) = 0;
                                        ///< Called to write out the routing message
                                        /**< This member must be defined in the derived class to
                                             somehow format and write the %log message. 

                                             Every %log message always possesses a complete set of
                                             meta information (\a stream, \a area and \a level).

                                             \note This member must \e not block since it may be
                                                 called from any unknown context. This prohibits
                                                 simple logging over NFS or many other network
                                                 protocols.

                                             \param[in] timestamp %log message timing information
                                             \param[in] stream message stream
                                             \param[in] area message area
                                             \param[in] level message level
                                             \param[in] message the message string */

#   ifdef DOXYGEN
    private:
#   endif

        RIB rib_;
        
        friend class detail::AreaBase;
        friend class detail::TargetRegistry;
    };

    /** \brief Log message time source abstract base class

        Instances derived from TimeSource provide the Logging library with the current date/time
        value. The \c operator() member must be implemented to return the current universal time
        (UTC).

        A new TimeSource may be installed using \ref senf::log::timeSource().

        \ingroup config
     */
    struct TimeSource
    {
        virtual ~TimeSource();
        virtual boost::posix_time::ptime operator()() const = 0;
    };

    /** \brief Default %log message time source

        This time source is installed by default and uses gettimeofday() (via the Boost.DateTime
        library) to get the current universal time.
        
        \ingroup config
     */
    struct SystemTimeSource : public TimeSource
    {
        virtual boost::posix_time::ptime operator()() const;
    };

    /** \brief Change %log message time source

        Set the %log message time source to \a source. The logging library will take ownership of \e
        source and will take care to free it, if necessary.

        Since the time source class will in almost all cases be default constructible, see the
        template overload for a simpler interface.

        \ingroup config
     */
    void timeSource(std::auto_ptr<TimeSource> source);

    /** \brief Change %log message time source

        Set the %log message time source to (an instance of) \a Source.  \a Source must be default
        constructible, otherwise use the non-template senf::log::timeSource() overload.

        \ingroup config
     */
    template <class Source> void timeSource();

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Target.cci"
//#include "Target.ct"
#include "Target.cti"
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
