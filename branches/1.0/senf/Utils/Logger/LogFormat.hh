// $Id$
//
// Copyright (C) 2009 
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
    \brief LogFormat public header */

#ifndef HH_SENF_Utils_Logger_LogFormat_
#define HH_SENF_Utils_Logger_LogFormat_ 1

// Custom includes
#include <sstream>
#include "Target.hh"
#include <senf/Utils/Console/LazyDirectory.hh>

//#include "LogFormat.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {
namespace detail {

    class LogFormat
    {
    public:
        LogFormat();
        explicit LogFormat(console::ScopedDirectory<> & dir);

        void showTime(bool flag = true); ///< Enable or disable output of time field
        void showStream(bool flag = true); ///< Enable or disable output of stream field
        void showLevel(bool flag = true); ///< Enable or disable output of log level
        void showArea(bool flag = true); ///< Enable or disable output of log area

        void timeFormat(std::string const & format);
                                        ///< Set time format
                                        /**< The date formatting is set using the Boost.DateTime
                                             date_facet, e.g.
                                             \code
                                                 target.timeFormat("%Y%m%d %H:%M:%S");
                                             \endcode
                                             If the \c timeFormat is set to the empty string, the
                                             time is written out as unformatted ClockService value.

                                             By default, the date-time will be written in extended
                                             ISO format.
                                             \param[in] format Date/Time format string */

        void tag(std::string const & tag); ///< Set tag (log line prefix)

    protected:
        std::string prefix(time_type timestamp, std::string const & stream, 
                           std::string const & area, unsigned level);
        bool isPlainFormat() const;
        void consoleFormat(std::ostream & os);

    private:

        std::string tag_;
        std::string timeFormat_;
        std::stringstream datestream_;
        bool noformat_;
        bool showTime_;
        bool showStream_;
        bool showLevel_;
        bool showArea_;
        time_type timeBase_;
    };

    void quoteNonPrintable(std::string & s);
    std::string getDefaultTag();

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "LogFormat.cci"
//#include "LogFormat.ct"
//#include "LogFormat.cti"
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
