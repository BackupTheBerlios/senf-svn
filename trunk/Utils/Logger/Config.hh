// $Id$
//
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
    \brief Config public header */

#ifndef HH_Config_
#define HH_Config_ 1

// Custom includes
#include "Levels.hh"

//#include "Config.mpp"
#include "Config.ih"
///////////////////////////////hh.p////////////////////////////////////////

/** \defgroup config Configuration

    The logger infrastructure provides for very fine-grained configuration of log messages. There
    are two parts to this configuration: compile-time configuration and runtime configuration.

    <em>Compile-time</em> configuration selects, which log statements will even be compiled. If
    logging for a certain combination of stream, area and level is disabled at compile time, no code
    will be generated for any such disabled log statement. This type of configuration is done using
    \ref SENF_LOG_CONF.

    <em>Runtime</em> configuration on the other hand deals with routing all those messages, which
    are enabled at compile time to the logging targets. If a message is not routed, it will be
    discarded. This allows to additionally disable messages at run-time.

    \fixme Restructure compile-time configuration: Only allow stream based configuration using
        SENF_LOG_CONF. For more complex configuration, accept a macro SENF_LOG_CONFFILE which, if
        defined, must define the path of a file to be included. Area specific configuration must be
        done in this include file. The area must be complete (not only a predeclaration) at that
        point.
 */

namespace senf {
namespace log {

    ///\ingroup config
    ///\{

#   ifdef DOXYGEN

    /** \brief Compile time configuration

        This define symbol sets the compile time logger configuration. This symbol should normally
        be set on the compiler command line:
        <pre>
        g++ ... -DSENF_LOG_CONF="(( (senf)(log)(Debug),(_),DISABLED ))
                                 (( (senf)(log)(Debug),(foo)(SomeClass),(VERBOSE) ))
                                 (( (foo)(Transactions),(_),NOTICE ))" ...
        </pre>
        (As this option can get quite long, you might want to use the '-imacros' option instead)

        The formal syntax of this option is:

        \par ""
            <table class="ebnf">
            <tr><td>conf</td>         <td>::= \e element \e element* \n</td></tr>
            <tr><td>element</td>      <td>::= <tt>((</tt> \e stream <tt>,</tt> \e optional_area <tt>,</tt> \e level <tt>))</tt> \n</td></tr>
            <tr><td>stream</td>       <td>::= \e scope_seq \n</td></tr>
            <tr><td>optional_area</td><td>::= <tt>(_)</tt> | \e scope_seq \n</td></tr>
            <tr><td>level</td>        <td>::= \c VERBOSE | \c NOTICE | \c MESSAGE | \c IMPORTANT | \c CRITICAL | \c DISABLED \n</td></tr>
            <tr><td>scope_seq</td>    <td>::= \e scope \e scope \e scope* \n</td></tr>
            <tr><td>scope</td>        <td>::= <tt>(</tt> \e name <tt>)</tt> \n</td></tr>
            <tr><td>name</td>         <td>::= arbitrary C++ identifier</td></tr>
            </table>

        \ref SENF_LOG_CONF is a Boost.Preprocessor style sequence of 3-tuples. Each tuple applies to
        a specific stream which is defined by the first tuple element \e stream. 

        The next tuple element, \e optional_area optionally restricts the entry to match only the
        given area. 

        The last tuple element \e level defines the compile time log level. Messages with a level
        below this are discarded at compile time.

        Both \e stream and \e optional_area are given as a \e scope_seq. A scope sequence is a fully
        qualified C++ identifier placed into a sequence: <tt>foo::bar::baz</tt> is represented by
        <tt>(foo)(bar)(baz)</tt>.
     */
#   define SENF_LOG_CONF

#   endif

    /** \brief Check, if logging is enabled for stream/area/level tuple
        
        This is a template meta-function which will check, whether logging to the given combination
        of parameters \a Stream, \a Area and \a Level is compile-time enabled. The logging might
        still be disabled at runtime.
        \code
        if (senf::log::Enabled<senf::log::Debug, 
                               senf::log::DefaultArea, 
                               senf::log::VERBOSE>::value) {
            // ...
        }
        \endcode

        Since the \e value member is a compile time constant, the compiler will completely optimize
        away this block of code when logging is disabled.
     */
    template <class Stream, class Area, class Level>
    struct Enabled
    {
        static const bool value = (
            (Level::value == NONE::value ? Stream::defaultLevel::value : Level::value)
                >= detail::Config<Stream,Area>::compileLimit::value );
    };

    ///\}

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Config.cci"
//#include "Config.ct"
//#include "Config.cti"
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
